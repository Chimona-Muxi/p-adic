//相对地址：p_adic/padic.h
#ifndef PADIC_H
#define PADIC_H

#include <stdbool.h>

#include "ast.h"

// p-adic数结构
typedef struct {
    int* digits;    // 数字数组
    int length;     // 数组长度（总位数）
    int p;          // 基数
    int min_power;  // 新增：digits[0] 所对应的 p 的指数。例如，-1 表示 digits[0] 是 p^{-1} 的系数。
    bool is_negative;
} PAdicNumber;

// 循环节信息
typedef struct {
    int start;
    int length;
    bool is_cyclic;
} CycleInfo;

// 数学辅助函数
int gcd(int a, int b);


// 在 padic.h 中添加
PAdicNumber* padic_neg(PAdicNumber* x, int max_digits);

// p-adic 基本算术
PAdicNumber* padic_clone(PAdicNumber* a);
PAdicNumber* padic_add(PAdicNumber* a, PAdicNumber* b, int max_digits);
PAdicNumber* padic_sub(PAdicNumber* a, PAdicNumber* b, int max_digits);
PAdicNumber* padic_mul(PAdicNumber* a, PAdicNumber* b, int max_digits);
PAdicNumber* padic_div_int(PAdicNumber* a, int n, int max_digits);

// p-adic 赋值比较
bool padic_is_zero(PAdicNumber* a);

// 计算 p-adic 数的 valuation (首个非零位的指数)；零返回 INT_MAX
int padic_valuation_of(PAdicNumber* n);

// 标准化：把前导 0 位合并进 min_power
void padic_normalize(PAdicNumber* a);

// 截断到指定位数（丢弃尾部，修改 length）
PAdicNumber* padic_truncate(PAdicNumber* a, int new_length);

// 整数p-adic转换
PAdicNumber* int_to_padic(int n, int p, int max_digits);

// 有理数p-adic转换
PAdicNumber* rational_to_padic(RationalNumber* rat, int p, int max_digits);
RationalNumber* parse_rational(const char* input);
// 严格解析：必须完全消费字符串，否则返回 NULL
RationalNumber* parse_rational_strict(const char* input);

// 内存管理
void free_padic(PAdicNumber* padic);

// 输出函数
void print_padic_series(PAdicNumber* padic, int max_digits);
void print_padic_formal(PAdicNumber* padic, int max_digits);

// 循环节检测
CycleInfo find_cycle(PAdicNumber* padic, int max_digits);
bool is_all_zeros_from(PAdicNumber* padic, int start_pos, int max_digits);

#endif