#ifndef PADIC_ROOT_H
#define PADIC_ROOT_H

#include "padic.h"


// 方根存在性判断函数
bool has_padic_root(PAdicNumber* n, int exponent, int p);
bool has_padic_square_root(PAdicNumber* n, int p);

// 方根计算函数
PAdicNumber* padic_sqrt(PAdicNumber* n, int p, int digits);
PAdicNumber* padic_root(PAdicNumber* n, int exponent, int p, int digits);

// 指数 / 对数
// padic_exp: 要求 v_p(x) >= 1 (p>2) 或 v_p(x) >= 2 (p=2)
PAdicNumber* padic_exp(PAdicNumber* x, int p, int digits);
// padic_log: 要求 x ≡ 1 (mod p) (p>2) 或 x ≡ 1 (mod 4) (p=2)
PAdicNumber* padic_log(PAdicNumber* x, int p, int digits);

// 辅助函数
int padic_valuation(PAdicNumber* n, int p);
int legendre_symbol(int a, int p);
int modular_inverse(int a, int p);
bool is_prime(int p);
int mod_pow(int base, int exp, int mod);

// 专门用于方根的输出函数
void print_padic_root_result(PAdicNumber* n, PAdicNumber* root, int exponent, int p, int digits);

#endif