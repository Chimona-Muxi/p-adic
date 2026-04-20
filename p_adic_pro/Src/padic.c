#include "padic.h"
#include "lang.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// 将数字转换为字符（0-9用数字，10-35用字母A-Z）
static char digit_to_char(int digit) {
    if (digit >= 0 && digit <= 9) {
        return '0' + digit;
    } else if (digit >= 10 && digit <= 35) {
        return 'A' + (digit - 10);
    } else {
        return '?';
    }
}

// 最大公约数函数
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// 计算真分数 a/b 的 p-adic 展开
static PAdicNumber* _unit_fraction_to_padic(int a, int b, int p, int max_digits) {
    PAdicNumber* padic = (PAdicNumber*)malloc(sizeof(PAdicNumber));
    padic->p = p;
    padic->min_power = 0;
    padic->is_negative = false;
    padic->digits = (int*)malloc(max_digits * sizeof(int));
    padic->length = max_digits;

    for (int i = 0; i < max_digits; i++) padic->digits[i] = 0;

    int inv_b = 0;
    for (int i = 1; i < p; i++) {
        if ((b * i) % p == 1) { inv_b = i; break; }
    }

    if (inv_b == 0) {
        fprintf(stderr, "Error: Cannot find modular inverse of %d mod %d\n", b, p);
        free(padic->digits);
        free(padic);
        return NULL;
    }

    int remainder = a;
    for (int i = 0; i < max_digits; i++) {
        padic->digits[i] = (remainder * inv_b) % p;
        if (padic->digits[i] < 0) padic->digits[i] += p;
        remainder = (remainder - b * padic->digits[i]) / p;
    }

    return padic;
}

// 计算p-adic数的相反数
PAdicNumber* padic_neg(PAdicNumber* x, int max_digits) {
    PAdicNumber* result = (PAdicNumber*)malloc(sizeof(PAdicNumber));
    result->p = x->p;
    result->digits = (int*)malloc(max_digits * sizeof(int));
    result->length = max_digits;
    result->min_power = x->min_power;
    result->is_negative = !x->is_negative;

    int carry = 1;
    for (int i = 0; i < max_digits; i++) {
        int digit = (x->p - 1 - x->digits[i] + carry);
        result->digits[i] = digit % x->p;
        carry = digit / x->p;
    }

    return result;
}

// 将整数转换为p-adic数
PAdicNumber* int_to_padic(int n, int p, int max_digits) {
    PAdicNumber* padic = (PAdicNumber*)malloc(sizeof(PAdicNumber));
    padic->p = p;
    padic->is_negative = (n < 0);
    padic->digits = (int*)malloc(max_digits * sizeof(int));
    padic->length = max_digits;
    padic->min_power = 0;

    for (int i = 0; i < max_digits; i++) padic->digits[i] = 0;

    if (n == 0) return padic;

    int m = abs(n);
    for (int i = 0; i < max_digits; i++) {
        padic->digits[i] = m % p;
        m = m / p;
    }

    if (padic->is_negative) {
        int carry = 1;
        for (int i = 0; i < max_digits; i++) {
            int digit = (p - 1) - padic->digits[i] + carry;
            padic->digits[i] = digit % p;
            carry = digit / p;
        }
    }

    return padic;
}

// 将有理数转换为p-adic数
PAdicNumber* rational_to_padic(RationalNumber* rat, int p, int max_digits) {
    if (rat->numerator == 0) {
        PAdicNumber* result = (PAdicNumber*)malloc(sizeof(PAdicNumber));
        result->p = p;
        result->digits = (int*)malloc(max_digits * sizeof(int));
        result->length = max_digits;
        result->min_power = 0;
        result->is_negative = false;
        for (int i = 0; i < max_digits; i++) result->digits[i] = 0;
        return result;
    }

    bool is_negative = (rat->numerator < 0) ^ (rat->denominator < 0);
    int num = abs(rat->numerator);
    int den = abs(rat->denominator);

    int g = gcd(num, den);
    num /= g;
    den /= g;

    int integer_part = num / den;
    int fractional_part = num % den;

    PAdicNumber* int_part = int_to_padic(integer_part, p, max_digits);

    if (fractional_part == 0) {
        if (is_negative) {
            PAdicNumber* neg_result = padic_neg(int_part, max_digits);
            free_padic(int_part);
            return neg_result;
        }
        return int_part;
    }

    int p_power = 0;
    int den_no_p = den;
    while (den_no_p % p == 0) { p_power++; den_no_p /= p; }

    PAdicNumber* frac_part = NULL;
    if (den_no_p == 1) {
        frac_part = (PAdicNumber*)malloc(sizeof(PAdicNumber));
        frac_part->p = p;
        frac_part->digits = (int*)malloc(max_digits * sizeof(int));
        frac_part->length = max_digits;
        frac_part->is_negative = false;
        for (int i = 0; i < max_digits; i++) frac_part->digits[i] = 0;
        int temp = fractional_part;
        for (int i = 0; i < max_digits; i++) {
            frac_part->digits[i] = temp % p;
            temp /= p;
        }
        frac_part->min_power = -p_power;
    } else {
        frac_part = _unit_fraction_to_padic(fractional_part, den_no_p, p, max_digits);
        frac_part->min_power = -p_power;
    }

    int result_min_power = (int_part->min_power < frac_part->min_power) ?
                           int_part->min_power : frac_part->min_power;

    PAdicNumber* result = (PAdicNumber*)malloc(sizeof(PAdicNumber));
    result->p = p;
    result->digits = (int*)malloc(max_digits * sizeof(int));
    result->length = max_digits;
    result->min_power = result_min_power;
    result->is_negative = false;
    for (int i = 0; i < max_digits; i++) result->digits[i] = 0;

    int int_offset = int_part->min_power - result_min_power;
    for (int i = 0; i < max_digits; i++) {
        int dst_idx = i + int_offset;
        if (dst_idx >= 0 && dst_idx < max_digits)
            result->digits[dst_idx] = int_part->digits[i];
    }

    int frac_offset = frac_part->min_power - result_min_power;
    int carry = 0;
    for (int i = 0; i < max_digits; i++) {
        int dst_idx = i + frac_offset;
        if (dst_idx >= 0 && dst_idx < max_digits) {
            int sum = result->digits[dst_idx] + frac_part->digits[i] + carry;
            result->digits[dst_idx] = sum % p;
            carry = sum / p;
        }
    }

    for (int i = frac_offset + max_digits; i < max_digits && carry > 0; i++) {
        if (i >= 0 && i < max_digits) {
            int sum = result->digits[i] + carry;
            result->digits[i] = sum % p;
            carry = sum / p;
        }
    }

    free_padic(int_part);
    free_padic(frac_part);

    if (is_negative) {
        PAdicNumber* neg_result = padic_neg(result, max_digits);
        free_padic(result);
        return neg_result;
    }

    return result;
}

// 解析有理数输入
RationalNumber* parse_rational(const char* input) {
    RationalNumber* rat = (RationalNumber*)malloc(sizeof(RationalNumber));
    int a, b;
    if (sscanf(input, "%d/%d", &a, &b) == 2) {
        rat->numerator = a;
        rat->denominator = b;
    } else if (sscanf(input, "%d", &a) == 1) {
        rat->numerator = a;
        rat->denominator = 1;
    } else {
        fprintf(stderr, "Error: Cannot parse rational number: %s\n", input);
        free(rat);
        return NULL;
    }
    return rat;
}

// 打印p-adic数的形式表示
void print_padic_formal(PAdicNumber* padic, int max_digits) {
    if (padic->p >= 36) {
        printf("%s\n", lang_get(STR_FORMAL_LARGE_BASE));
        return;
    }

    printf("%s", lang_get(STR_FORMAL_LABEL));

    CycleInfo cycle = find_cycle(padic, max_digits);
    int dot_pos = -padic->min_power;

    if (padic->min_power < 0) {
        if (cycle.is_cyclic) {
            for (int i = 0; i < dot_pos; i++) {
                printf("%c", i < max_digits ? digit_to_char(padic->digits[i]) : '0');
            }
            printf(".");
            int start_idx = (dot_pos > 0) ? dot_pos : 0;
            for (int i = start_idx; i < cycle.start; i++) {
                if (i < max_digits) printf("%c", digit_to_char(padic->digits[i]));
            }
            int repeat_times = 2;
            if (cycle.length == 1) repeat_times = 4;
            else if (cycle.length == 2) repeat_times = 3;
            for (int r = 0; r < repeat_times; r++) {
                for (int i = 0; i < cycle.length; i++) {
                    int idx = cycle.start + i;
                    if (idx < max_digits) printf("%c", digit_to_char(padic->digits[idx]));
                }
            }
            printf("...");
        } else {
            for (int i = 0; i < dot_pos; i++) {
                printf("%c", i < max_digits ? digit_to_char(padic->digits[i]) : '0');
            }
            printf(".");
            int start_idx = (dot_pos > 0) ? dot_pos : 0;
            int last_nonzero = -1;
            for (int i = start_idx; i < max_digits; i++) {
                if (padic->digits[i] != 0) last_nonzero = i;
            }
            if (last_nonzero < start_idx) {
                printf("0");
            } else {
                for (int i = start_idx; i <= last_nonzero; i++)
                    printf("%c", digit_to_char(padic->digits[i]));
            }
            if (last_nonzero == -1) {
            } else if (last_nonzero >= max_digits - 2) {
                printf("...");
            } else if (!is_all_zeros_from(padic, last_nonzero + 1, max_digits)) {
                printf("...");
            }
        }
    } else {
        if (cycle.is_cyclic) {
            for (int i = 0; i < cycle.start; i++)
                printf("%c", digit_to_char(padic->digits[i]));
            int repeat_times = 2;
            if (cycle.length == 1) repeat_times = 4;
            else if (cycle.length == 2) repeat_times = 3;
            for (int r = 0; r < repeat_times; r++) {
                for (int i = 0; i < cycle.length; i++) {
                    int idx = cycle.start + i;
                    if (idx < max_digits) printf("%c", digit_to_char(padic->digits[idx]));
                }
            }
            printf("...");
        } else {
            int last_nonzero = -1;
            for (int i = 0; i < max_digits; i++) {
                if (padic->digits[i] != 0) last_nonzero = i;
            }
            if (last_nonzero == -1) {
                printf("0");
            } else {
                for (int i = 0; i <= last_nonzero; i++)
                    printf("%c", digit_to_char(padic->digits[i]));
            }
            if (last_nonzero == -1) {
            } else if (last_nonzero >= max_digits - 2) {
                printf("...");
            } else if (!is_all_zeros_from(padic, last_nonzero + 1, max_digits)) {
                printf("...");
            }
        }
    }

    printf(")₍%d₎\n", padic->p);
}

// 打印p-adic数的级数表示
void print_padic_series(PAdicNumber* padic, int max_digits) {
    printf("%s", lang_get(STR_SERIES_LABEL));

    CycleInfo cycle = find_cycle(padic, max_digits);

    bool all_zero = true;
    for (int i = 0; i < max_digits; i++) {
        if (padic->digits[i] != 0) { all_zero = false; break; }
    }

    if (all_zero) {
        printf("0");
    } else if (cycle.is_cyclic) {
        bool first_term = true;
        for (int i = 0; i < cycle.start; i++) {
            int digit = padic->digits[i];
            if (digit != 0) {
                if (!first_term) printf(" + ");
                int exp = padic->min_power + i;
                if (exp == 0) printf("%d", digit);
                else if (exp == 1) printf("%d*%d", digit, padic->p);
                else printf("%d*%d^%d", digit, padic->p, exp);
                first_term = false;
            }
        }
        int repeat_times = 2;
        if (cycle.length == 1) repeat_times = 4;
        else if (cycle.length == 2) repeat_times = 3;
        for (int r = 0; r < repeat_times; r++) {
            for (int i = 0; i < cycle.length; i++) {
                int digit = padic->digits[cycle.start + i];
                if (digit != 0) {
                    int pos = cycle.start + i + r * cycle.length;
                    if (!first_term) printf(" + ");
                    int exp = padic->min_power + pos;
                    if (exp == 0) printf("%d", digit);
                    else if (exp == 1) printf("%d*%d", digit, padic->p);
                    else printf("%d*%d^%d", digit, padic->p, exp);
                    first_term = false;
                }
            }
        }
        printf(" + ...");
    } else {
        bool first_term = true;
        bool has_terms = false;
        int last_nonzero = -1;
        for (int i = 0; i < max_digits; i++) {
            if (padic->digits[i] != 0) last_nonzero = i;
        }
        for (int i = 0; i <= last_nonzero; i++) {
            int digit = padic->digits[i];
            if (digit != 0) {
                if (!first_term) printf(" + ");
                int exp = padic->min_power + i;
                if (exp == 0) printf("%d", digit);
                else if (exp == 1) printf("%d*%d", digit, padic->p);
                else printf("%d*%d^%d", digit, padic->p, exp);
                first_term = false;
                has_terms = true;
            }
        }
        if (!has_terms) {
            printf("0");
        } else {
            if (last_nonzero == -1) {
            } else if (last_nonzero >= max_digits - 2) {
                printf(" + ...");
            } else if (!is_all_zeros_from(padic, last_nonzero + 1, max_digits)) {
                printf(" + ...");
            }
        }
    }

    printf("\n");
}

// 寻找循环节
CycleInfo find_cycle(PAdicNumber* padic, int max_digits) {
    CycleInfo info = {0, 0, false};

    bool all_zero = true;
    for (int i = 0; i < max_digits; i++) {
        if (padic->digits[i] != 0) { all_zero = false; break; }
    }
    if (all_zero) return info;

    int last_nonzero = -1;
    for (int i = 0; i < max_digits; i++) {
        if (padic->digits[i] != 0) last_nonzero = i;
    }

    if (last_nonzero < max_digits - 1) {
        bool all_zeros_after = true;
        for (int i = last_nonzero + 1; i < max_digits; i++) {
            if (padic->digits[i] != 0) { all_zeros_after = false; break; }
        }
        if (all_zeros_after) return info;
    }

    for (int start = 0; start < max_digits/2; start++) {
        for (int len = 1; len <= (max_digits - start)/2; len++) {
            bool is_cycle = true;
            for (int i = 0; i < len; i++) {
                if (start + i + len >= max_digits) { is_cycle = false; break; }
                if (padic->digits[start + i] != padic->digits[start + i + len]) {
                    is_cycle = false; break;
                }
            }
            if (is_cycle) {
                bool all_zero_cycle = true;
                for (int i = 0; i < len; i++) {
                    if (padic->digits[start + i] != 0) { all_zero_cycle = false; break; }
                }
                if (all_zero_cycle) continue;

                bool continues = true;
                for (int i = start; i < max_digits; i++) {
                    if (i + len < max_digits &&
                        padic->digits[i] != padic->digits[i + len]) {
                        continues = false; break;
                    }
                }
                if (continues) {
                    info.is_cyclic = true;
                    info.start = start;
                    info.length = len;
                    return info;
                }
            }
        }
    }

    return info;
}

// 检查从指定位置开始是否全是0
bool is_all_zeros_from(PAdicNumber* padic, int start_pos, int max_digits) {
    for (int i = start_pos; i < max_digits; i++) {
        if (padic->digits[i] != 0) return false;
    }
    return true;
}

// 释放p-adic数内存
void free_padic(PAdicNumber* padic) {
    if (padic == NULL) return;
    if (padic->digits != NULL) free(padic->digits);
    free(padic);
}
