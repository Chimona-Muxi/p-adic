#include "padic_root.h"
#include "lang.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

static char digit_to_char_local(int digit) {
    if (digit >= 0 && digit <= 9) return '0' + digit;
    else if (digit >= 10 && digit <= 35) return 'A' + (digit - 10);
    else return '?';
}

bool is_prime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

int mod_pow(int base, int exp, int mod) {
    int result = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp & 1) result = (result * base) % mod;
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return result;
}

int legendre_symbol(int a, int p) {
    if (a % p == 0) return 0;
    return mod_pow(a, (p - 1) / 2, p);
}

int modular_inverse(int a, int p) {
    return mod_pow(a, p - 2, p);
}

int padic_valuation(PAdicNumber* n, int p) {
    (void)p;
    if (n == NULL) return 0;
    for (int i = 0; i < n->length; i++) {
        if (n->digits[i] != 0) return n->min_power + i;
    }
    return 999999;
}

bool has_padic_square_root(PAdicNumber* n, int p) {
    if (n == NULL) return false;
    int v = padic_valuation(n, p);
    if (v == 999999) return true;
    if (v % 2 != 0) return false;
    if (p > 2) {
        int unit = 0, power = 1;
        for (int i = 0; i < 1; i++) {
            int idx = v - n->min_power + i;
            if (idx >= 0 && idx < n->length) unit += n->digits[idx] * power;
            power *= p;
        }
        return (legendre_symbol(unit, p) == 1);
    } else {
        int n_mod_8 = 0, power = 1;
        for (int i = 0; i < 3 && (v + i - n->min_power) < n->length; i++) {
            int idx = v + i - n->min_power;
            if (idx >= 0 && idx < n->length) n_mod_8 += n->digits[idx] * power;
            power *= 2;
        }
        n_mod_8 %= 8;
        return (v % 2 == 0) ? (n_mod_8 == 1) : false;
    }
}

PAdicNumber* padic_sqrt(PAdicNumber* n, int p, int digits) {
    if (n == NULL) return NULL;
    if (!has_padic_square_root(n, p)) return NULL;

    PAdicNumber* result = (PAdicNumber*)malloc(sizeof(PAdicNumber));
    result->p = p;
    result->digits = (int*)malloc(digits * sizeof(int));
    result->length = digits;
    result->is_negative = false;

    int v = padic_valuation(n, p);
    result->min_power = v / 2;
    for (int i = 0; i < digits; i++) result->digits[i] = 0;

    if (v == 999999) return result;

    int unit = 0, power = 1;
    for (int i = 0; i < 1 && (v + i - n->min_power) < n->length; i++) {
        int idx = v + i - n->min_power;
        if (idx >= 0 && idx < n->length) unit += n->digits[idx] * power;
        power *= p;
    }
    unit %= p;

    int r0 = 0;
    if (p > 2) {
        if (p % 4 == 3) {
            r0 = mod_pow(unit, (p + 1) / 4, p);
        } else {
            for (int x = 0; x < p; x++) {
                if ((x * x) % p == unit) { r0 = x; break; }
            }
        }
    } else {
        r0 = 1;
    }

    result->digits[0] = r0;

    for (int k = 1; k < digits; k++) {
        long long current = 0, p_pow = 1;
        for (int i = 0; i < k; i++) {
            current += result->digits[i] * p_pow;
            p_pow *= p;
        }

        long long n_k = 0;
        p_pow = 1;
        for (int i = 0; i <= k && (v + i - n->min_power) < n->length; i++) {
            int idx = v + i - n->min_power;
            if (idx >= 0 && idx < n->length) n_k += n->digits[idx] * p_pow;
            p_pow *= p;
        }

        long long fx = (current * current) - n_k;
        long long p_k = 1;
        for (int i = 0; i < k; i++) p_k *= p;
        long long p_k1 = p_k * p;

        fx = fx % p_k1;
        if (fx < 0) fx += p_k1;

        if (fx == 0) {
            result->digits[k] = 0;
        } else {
            int fprime = (2 * result->digits[0]) % p;
            if (fprime == 0) {
                result->digits[k] = 0;
            } else {
                int fprime_inv = modular_inverse(fprime, p);
                long long fx_div = fx / p_k;
                int t = (-fx_div * fprime_inv) % p;
                if (t < 0) t += p;
                result->digits[k] = t;
            }
        }
    }

    return result;
}

void print_padic_root_result(PAdicNumber* n, PAdicNumber* root, int exponent, int p, int digits) {
    (void)n;
    (void)exponent;

    if (root == NULL) {
        printf(lang_get(STR_ERR_NO_SQRT), p);
        printf("\n");
        return;
    }

    printf(lang_get(STR_ROOT_RESULT_TITLE), digits);
    printf("\n");

    if (p >= 36) {
        printf("%s\n", lang_get(STR_FORMAL_LARGE_BASE));
    } else {
        printf("%s", lang_get(STR_FORMAL_LABEL));
        for (int i = 0; i < digits && i < root->length; i++) {
            printf("%c", digit_to_char_local(root->digits[i]));
        }
        printf("...)₍%d₎\n", p);
    }

    printf("%s", lang_get(STR_SERIES_LABEL));
    bool first = true;
    for (int i = 0; i < digits && i < root->length; i++) {
        int coeff = root->digits[i];
        if (coeff != 0) {
            if (!first) printf(" + ");
            int exp = root->min_power + i;
            if (exp == 0) printf("%d", coeff);
            else if (exp == 1) printf("%d*%d", coeff, p);
            else printf("%d*%d^%d", coeff, p, exp);
            first = false;
        }
    }
    if (first) printf("0");
    printf("\n");
}
