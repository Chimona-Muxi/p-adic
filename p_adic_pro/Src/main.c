#include "ui.h"
#include "padic.h"
#include "padic_root.h"
#include "ast.h"
#include "lang.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEFAULT_DIGITS 15

// 从输入中剥离出末尾的 "p"（最后一个空格后的整数）
// 成功：把表达式写入 expr_out，p 写入 *p_out，返回 true
static bool split_expr_and_p(const char* input, char* expr_out, int expr_cap, int* p_out) {
    const char* last_space = strrchr(input, ' ');
    if (last_space == NULL) return false;
    int p = 0;
    if (sscanf(last_space + 1, "%d", &p) != 1 || p <= 1) return false;

    int len = (int)(last_space - input);
    if (len <= 0 || len >= expr_cap) return false;
    memcpy(expr_out, input, len);
    expr_out[len] = '\0';

    // 去掉右侧空白
    while (len > 0 && isspace((unsigned char)expr_out[len - 1])) {
        expr_out[--len] = '\0';
    }
    if (len == 0) return false;

    *p_out = p;
    return true;
}

// 把一个 AST 表达式作为 p-adic 数输出
// top 表示该表达式是否可能是 sqrt/exp/log 的顶层调用（调用方已处理特殊分支，这里只处理纯有理）
static void evaluate_and_print_rational_padic(ASTNode* ast, int p, int max_digits) {
    RationalNumber rat;
    if (!evaluate_ast_safe(ast, &rat)) {
        printf("%s\n", lang_get(STR_ERR_EXPR_TO_PADIC));
        return;
    }
    if (rat.denominator == 0) {
        printf("%s\n", lang_get(STR_ERR_EXPR_TO_PADIC));
        return;
    }

    printf(lang_get(STR_EXPR_RESULT), rat.numerator, rat.denominator);
    printf("\n");
    printf(lang_get(STR_PADIC_BASE), p);
    printf("\n\n");

    PAdicNumber* padic = rational_to_padic(&rat, p, max_digits);
    if (padic != NULL) {
        print_padic_formal(padic, max_digits);
        print_padic_series(padic, max_digits);
    } else {
        printf("%s\n", lang_get(STR_CONVERT_FAIL));
    }
    free_padic(padic);
}

// sqrt(expr) p —— 计算 p-adic 平方根
static void handle_sqrt_via_ast(const char* inner_expr, int p) {
    ASTNode* ast = parse(inner_expr);
    if (ast == NULL) {
        printf("%s\n", lang_get(STR_ERR_EXPR_PARSE));
        return;
    }
    RationalNumber rat;
    if (!evaluate_ast_safe(ast, &rat) || rat.denominator == 0) {
        printf("%s\n", lang_get(STR_ERR_EXPR_PARSE));
        free_ast(ast);
        return;
    }
    free_ast(ast);

    printf(lang_get(STR_CALC_SQRT), rat.numerator, rat.denominator, p);
    printf("\n");

    int digits = DEFAULT_DIGITS;
    PAdicNumber* base_padic = rational_to_padic(&rat, p, digits);
    if (base_padic == NULL) {
        printf("%s\n", lang_get(STR_ERR_CONVERT_BASE));
        return;
    }

    PAdicNumber* root = padic_sqrt(base_padic, p, digits);
    if (root == NULL) {
        printf(lang_get(STR_ERR_NO_SQRT), p);
        printf("\n");
    } else {
        printf("\n%s\n", lang_get(STR_FIRST_ROOT));
        print_padic_formal(root, digits);
        print_padic_series(root, digits);

        printf("\n%s\n", lang_get(STR_SECOND_ROOT));
        PAdicNumber* neg_root = padic_neg(root, digits);
        print_padic_formal(neg_root, digits);
        print_padic_series(neg_root, digits);
        free_padic(neg_root);
        free_padic(root);
    }

    printf("\n");
    free_padic(base_padic);
}

// pow(base, exp) p
//   - 若 exp 为 1/2 -> 转 sqrt
//   - 若 exp 为整数 -> 用有理幂计算后再转 p-adic
//   - 否则报错
static void handle_pow_input(const char* input) {
    // 先分离出末尾的 p
    char tail[512];
    int p = 0;
    if (!split_expr_and_p(input, tail, sizeof(tail), &p)) {
        printf("%s\n", lang_get(STR_ERR_POW_FORMAT));
        return;
    }

    // tail 现在形如 "pow(base, exp)"
    ASTNode* ast = parse(tail);
    if (ast == NULL || ast->type != NODE_POW) {
        printf("%s\n", lang_get(STR_ERR_POW_FORMAT));
        free_ast(ast);
        return;
    }

    RationalNumber base_r, exp_r;
    if (!evaluate_ast_safe(ast->left, &base_r) ||
        !evaluate_ast_safe(ast->right, &exp_r)) {
        printf("%s\n", lang_get(STR_ERR_POW_FORMAT));
        free_ast(ast);
        return;
    }
    free_ast(ast);

    int digits = DEFAULT_DIGITS;

    // 指数为整数：有理幂
    if (exp_r.denominator == 1) {
        // base^exp
        RationalNumber r = {1, 1};
        long long num_acc = 1, den_acc = 1;
        int n = exp_r.numerator;
        long long b_num = base_r.numerator, b_den = base_r.denominator;
        if (n < 0) {
            if (b_num == 0) { printf("%s\n", lang_get(STR_ERR_POW_FORMAT)); return; }
            long long t = b_num; b_num = b_den; b_den = t;
            n = -n;
        }
        for (int i = 0; i < n; i++) {
            num_acc *= b_num;
            den_acc *= b_den;
        }
        r.numerator   = (int)num_acc;
        r.denominator = (int)den_acc;

        printf(lang_get(STR_EXPR_RESULT), r.numerator, r.denominator);
        printf("\n");
        printf(lang_get(STR_PADIC_BASE), p);
        printf("\n\n");

        PAdicNumber* padic = rational_to_padic(&r, p, digits);
        if (padic != NULL) {
            print_padic_formal(padic, digits);
            print_padic_series(padic, digits);
        }
        free_padic(padic);
        printf("\n");
        return;
    }

    // 指数 1/2：视为 sqrt
    if (exp_r.numerator == 1 && exp_r.denominator == 2) {
        printf(lang_get(STR_CALC_SQRT), base_r.numerator, base_r.denominator, p);
        printf("\n");
        PAdicNumber* base_padic = rational_to_padic(&base_r, p, digits);
        if (base_padic == NULL) { printf("%s\n", lang_get(STR_ERR_CONVERT_BASE)); return; }

        PAdicNumber* root = padic_sqrt(base_padic, p, digits);
        if (root == NULL) {
            printf(lang_get(STR_ERR_NO_SQRT), p); printf("\n");
        } else {
            printf("\n%s\n", lang_get(STR_FIRST_ROOT));
            print_padic_formal(root, digits);
            print_padic_series(root, digits);

            printf("\n%s\n", lang_get(STR_SECOND_ROOT));
            PAdicNumber* neg_root = padic_neg(root, digits);
            print_padic_formal(neg_root, digits);
            print_padic_series(neg_root, digits);
            free_padic(neg_root);
            free_padic(root);
        }
        printf("\n");
        free_padic(base_padic);
        return;
    }

    printf("%s\n", lang_get(STR_ERR_ONLY_SQRT));
}

// exp(expr) p
static void handle_exp_input(const char* input) {
    char tail[512];
    int p = 0;
    if (!split_expr_and_p(input, tail, sizeof(tail), &p)) {
        printf("%s\n", lang_get(STR_ERR_EXP_FORMAT));
        return;
    }
    ASTNode* ast = parse(tail);
    if (ast == NULL || ast->type != NODE_EXP) {
        printf("%s\n", lang_get(STR_ERR_EXP_FORMAT));
        free_ast(ast);
        return;
    }
    RationalNumber arg;
    if (!evaluate_ast_safe(ast->left, &arg) || arg.denominator == 0) {
        printf("%s\n", lang_get(STR_ERR_EXP_FORMAT));
        free_ast(ast);
        return;
    }
    free_ast(ast);

    printf(lang_get(STR_CALC_EXP), arg.numerator, arg.denominator, p);
    printf("\n");

    int digits = DEFAULT_DIGITS;
    PAdicNumber* xp = rational_to_padic(&arg, p, digits);
    if (xp == NULL) { printf("%s\n", lang_get(STR_ERR_CONVERT_BASE)); return; }

    PAdicNumber* r = padic_exp(xp, p, digits);
    if (r == NULL) {
        printf("%s\n", lang_get(STR_ERR_EXP_DIVERGE));
    } else {
        printf("\n%s\n", lang_get(STR_EXP_RESULT));
        print_padic_formal(r, digits);
        print_padic_series(r, digits);
        free_padic(r);
    }
    free_padic(xp);
    printf("\n");
}

// log(expr) p
static void handle_log_input(const char* input) {
    char tail[512];
    int p = 0;
    if (!split_expr_and_p(input, tail, sizeof(tail), &p)) {
        printf("%s\n", lang_get(STR_ERR_LOG_FORMAT));
        return;
    }
    ASTNode* ast = parse(tail);
    if (ast == NULL || ast->type != NODE_LOG) {
        printf("%s\n", lang_get(STR_ERR_LOG_FORMAT));
        free_ast(ast);
        return;
    }
    RationalNumber arg;
    if (!evaluate_ast_safe(ast->left, &arg) || arg.denominator == 0) {
        printf("%s\n", lang_get(STR_ERR_LOG_FORMAT));
        free_ast(ast);
        return;
    }
    free_ast(ast);

    printf(lang_get(STR_CALC_LOG), arg.numerator, arg.denominator, p);
    printf("\n");

    int digits = DEFAULT_DIGITS;
    PAdicNumber* xp = rational_to_padic(&arg, p, digits);
    if (xp == NULL) { printf("%s\n", lang_get(STR_ERR_CONVERT_BASE)); return; }

    PAdicNumber* r = padic_log(xp, p, digits);
    if (r == NULL) {
        printf("%s\n", lang_get(STR_ERR_LOG_DIVERGE));
    } else {
        printf("\n%s\n", lang_get(STR_LOG_RESULT));
        print_padic_formal(r, digits);
        print_padic_series(r, digits);
        free_padic(r);
    }
    free_padic(xp);
    printf("\n");
}

// 处理单个输入
static void handle_input(char* input) {
    if (strlen(input) == 0) return;

    if (strcmp(input, "help") == 0) {
        printf("%s\n", lang_get(STR_HELP_FORMAT));
        printf("%s\n", lang_get(STR_HELP_1));
        printf("%s\n", lang_get(STR_HELP_2));
        printf("%s\n", lang_get(STR_HELP_3));
        printf("%s\n", lang_get(STR_HELP_4));
        printf("%s\n", lang_get(STR_HELP_EXPR));
        printf("%s\n", lang_get(STR_HELP_COMMANDS));
        return;
    }
    if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0 || strcmp(input, "over") == 0) {
        printf("%s\n", lang_get(STR_EXIT));
        exit(0);
    }
    if (strcmp(input, "menu") == 0) {
        printf("\n");
        main_menu();
        printf("\n%s\n", lang_get(STR_RETURN_MENU));
        return;
    }
    if (strcmp(input, "language") == 0) {
        printf("%s\n", lang_get(STR_CURRENT_LANG));
        return;
    }

    // 特殊前缀函数
    if (strncmp(input, "pow(", 4) == 0)  { handle_pow_input(input);  return; }
    if (strncmp(input, "exp(", 4) == 0)  { handle_exp_input(input);  return; }
    if (strncmp(input, "log(", 4) == 0)  { handle_log_input(input);  return; }
    if (strncmp(input, "ln(",  3) == 0)  { handle_log_input(input);  return; }
    if (strncmp(input, "sqrt(", 5) == 0) {
        char tail[512];
        int p = 0;
        if (!split_expr_and_p(input, tail, sizeof(tail), &p)) {
            printf("%s\n", lang_get(STR_ERR_SQRT_FORMAT));
            return;
        }
        // tail 是 "sqrt(...)", 取出内部
        char* open = strchr(tail, '(');
        char* close = strrchr(tail, ')');
        if (open == NULL || close == NULL || close <= open + 1) {
            printf("%s\n", lang_get(STR_ERR_SQRT_FORMAT));
            return;
        }
        *close = '\0';
        handle_sqrt_via_ast(open + 1, p);
        return;
    }

    // 分离表达式和 p
    char expr[512];
    int p = 0;
    if (!split_expr_and_p(input, expr, sizeof(expr), &p)) {
        printf("%s\n", lang_get(STR_ERR_FORMAT));
        return;
    }

    int max_digits = 200;

    // 优先尝试严格有理数 (更快，覆盖 "5" 和 "1/3" 等简单输入)
    RationalNumber* rat_strict = parse_rational_strict(expr);
    if (rat_strict != NULL) {
        PAdicNumber* padic = rational_to_padic(rat_strict, p, max_digits);
        if (padic != NULL) {
            print_padic_formal(padic, max_digits);
            print_padic_series(padic, max_digits);
        }
        printf("\n");
        free_padic(padic);
        free(rat_strict);
        return;
    }

    // 一般表达式：用 AST 求值
    ASTNode* ast = parse(expr);
    if (ast == NULL) {
        printf("%s\n", lang_get(STR_ERR_EXPR_PARSE));
        return;
    }
    if (!ast_is_rational_only(ast)) {
        // 含有 sqrt / exp / log / 非整指数 pow 等无法求为有理数的节点
        printf("%s\n", lang_get(STR_ERR_EXPR_TO_PADIC));
        free_ast(ast);
        return;
    }

    evaluate_and_print_rational_padic(ast, p, max_digits);
    printf("\n");
    free_ast(ast);
}

int main() {
    lang_init();

    char input[512];

    while (1) {
        printf("%s", lang_get(STR_PROMPT));
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) break;

        input[strcspn(input, "\n")] = '\0';

        handle_input(input);
    }

    return 0;
}
