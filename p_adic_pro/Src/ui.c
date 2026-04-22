#include "ui.h"
#include "lang.h"
#include "ast.h"
#include "padic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 测试整数p-adic转换
void test_padic_conversion() {
    printf("%s\n\n", lang_get(STR_TEST_INT_TITLE));

    int test_cases[][2] = {
        {5, 7}, {-5, 7}, {10, 3}, {-10, 3},
        {1, 5}, {-1, 5}, {25, 5}, {-25, 5},
        {0, 7}, {17, 2}, {-17, 2}, {3, 2},
        {6, 5}, {7, 3}, {-7, 3}, {1, 3},
        {-1, 3}, {1, 7}, {-1, 7}, {23, 7},
        {0, 0}
    };

    int max_digits = 20;

    for (int i = 0; test_cases[i][1] != 0; i++) {
        int n = test_cases[i][0];
        int p = test_cases[i][1];

        printf(lang_get(STR_TEST_CASE), i + 1, n, p);
        printf("\n");

        PAdicNumber* padic = int_to_padic(n, p, max_digits);
        print_padic_formal(padic, max_digits);
        print_padic_series(padic, max_digits);

        printf("\n");
        free_padic(padic);
    }
}

// 测试有理数p-adic转换
void test_rational_padic_conversion() {
    printf("%s\n\n", lang_get(STR_TEST_RAT_TITLE));

    int test_cases[][3] = {
        {1, 3, 5}, {-1, 3, 5}, {2, 3, 5}, {1, 2, 5},
        {3, 4, 5}, {1, 5, 5}, {2, 5, 5}, {1, 7, 5},
        {3, 2, 7}, {4, 3, 7},
        {0, 0, 0}
    };

    int max_digits = 20;

    for (int i = 0; test_cases[i][2] != 0; i++) {
        int num = test_cases[i][0];
        int den = test_cases[i][1];
        int p   = test_cases[i][2];

        RationalNumber rat = {num, den};

        printf(lang_get(STR_TEST_CASE_RAT), i + 1, num, den, p);
        printf("\n");

        PAdicNumber* padic = rational_to_padic(&rat, p, max_digits);

        if (padic != NULL) {
            print_padic_formal(padic, max_digits);
            print_padic_series(padic, max_digits);
        } else {
            printf("%s\n", lang_get(STR_CONVERT_FAIL));
        }

        printf("\n");
        free_padic(padic);
    }
}

// 交互式p-adic转换
void interactive_padic() {
    printf("%s\n", lang_get(STR_INTERACTIVE_TITLE));
    printf("%s\n", lang_get(STR_INTERACTIVE_HINT_INT));
    printf("%s\n", lang_get(STR_INTERACTIVE_HINT_FRAC));
    printf("%s\n\n", lang_get(STR_INTERACTIVE_BACK));

    char input[256];
    while (1) {
        printf("%s", lang_get(STR_PROMPT));
        if (fgets(input, sizeof(input), stdin) == NULL) break;

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "back") == 0 || strcmp(input, "quit") == 0 ||
            strcmp(input, "menu") == 0) break;

        if (strlen(input) == 0) continue;

        char num_str[256];
        int p = 0;

        char* last_space = strrchr(input, ' ');
        if (last_space == NULL) {
            printf("%s\n", lang_get(STR_ERR_INPUT_FORMAT));
            continue;
        }

        int num_len = last_space - input;
        strncpy(num_str, input, num_len);
        num_str[num_len] = '\0';

        if (sscanf(last_space + 1, "%d", &p) != 1 || p <= 1) {
            printf("%s\n", lang_get(STR_ERR_P_VALUE));
            continue;
        }

        RationalNumber* rat = parse_rational(num_str);
        if (rat == NULL) {
            printf("%s\n", lang_get(STR_ERR_PARSE));
            continue;
        }

        int max_digits = 20;
        printf("\n");
        printf(lang_get(STR_RATIONAL_NUMBER), rat->numerator, rat->denominator);
        printf("\n");
        printf(lang_get(STR_PADIC_BASE), p);
        printf("\n\n");

        PAdicNumber* padic = rational_to_padic(rat, p, max_digits);

        if (padic != NULL) {
            print_padic_formal(padic, max_digits);
            print_padic_series(padic, max_digits);
        }

        printf("\n");
        free_padic(padic);
        free(rat);
    }
}

// 表达式求值并转换为p-adic
void expression_to_padic() {
    printf("%s\n", lang_get(STR_EXPR_TITLE));
    printf("%s\n", lang_get(STR_EXPR_HINT));
    printf("%s\n\n", lang_get(STR_INTERACTIVE_BACK));

    char input[256];
    while (1) {
        printf("%s", lang_get(STR_EXPR_PROMPT));
        if (fgets(input, sizeof(input), stdin) == NULL) break;

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "back") == 0 || strcmp(input, "quit") == 0 ||
            strcmp(input, "menu") == 0) break;

        if (strlen(input) == 0) continue;

        char expr[256];
        int p = 0;

        char* last_space = strrchr(input, ' ');
        if (last_space == NULL) {
            printf("%s\n", lang_get(STR_ERR_EXPR_FORMAT));
            continue;
        }

        int expr_len = last_space - input;
        strncpy(expr, input, expr_len);
        expr[expr_len] = '\0';

        if (sscanf(last_space + 1, "%d", &p) != 1 || p <= 1) {
            printf("%s\n", lang_get(STR_ERR_P_VALUE));
            continue;
        }

        ASTNode* ast = parse(expr);
        if (ast == NULL) {
            printf("%s\n", lang_get(STR_ERR_EXPR_PARSE));
            continue;
        }

        RationalNumber rat_result = evaluate_ast_as_rational(ast);

        printf("\n");
        printf(lang_get(STR_EXPR_LABEL), expr);
        printf("\n");
        printf(lang_get(STR_EXPR_RESULT), rat_result.numerator, rat_result.denominator);
        printf("\n");
        printf(lang_get(STR_PADIC_BASE), p);
        printf("\n\n");

        int max_digits = 20;
        PAdicNumber* padic = NULL;

        if (rat_result.denominator == 1) {
            padic = int_to_padic(rat_result.numerator, p, max_digits);
        } else {
            padic = rational_to_padic(&rat_result, p, max_digits);
        }

        print_padic_formal(padic, max_digits);
        print_padic_series(padic, max_digits);

        printf("\n");
        free_padic(padic);
        free_ast(ast);
    }
}

// 主菜单
void main_menu() {
    printf("%s (精度: 20位)\n", lang_get(STR_MENU_TITLE));
    printf("%s\n", lang_get(STR_MENU_SEPARATOR));
    printf("%s\n", lang_get(STR_MENU_1));
    printf("%s\n", lang_get(STR_MENU_2));
    printf("%s\n", lang_get(STR_MENU_3));
    printf("%s\n", lang_get(STR_MENU_4));
    printf("%s\n", lang_get(STR_MENU_5));
    printf("%s\n\n", lang_get(STR_MENU_6));

    char input[256];
    while (1) {
        printf("%s", lang_get(STR_MENU_CHOICE));
        if (fgets(input, sizeof(input), stdin) == NULL) break;

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "1") == 0) {
            // 语法解析器测试（预留）
        } else if (strcmp(input, "2") == 0) {
            test_padic_conversion();
            printf("%s", lang_get(STR_PRESS_ENTER));
            getchar();
        } else if (strcmp(input, "3") == 0) {
            test_rational_padic_conversion();
            printf("%s", lang_get(STR_PRESS_ENTER));
            getchar();
        } else if (strcmp(input, "4") == 0) {
            interactive_padic();
        } else if (strcmp(input, "5") == 0) {
            expression_to_padic();
        } else if (strcmp(input, "6") == 0 ||
                   strcmp(input, "quit") == 0 ||
                   strcmp(input, "exit") == 0) {
            printf("%s\n", lang_get(STR_GOODBYE));
            exit(0);
        } else {
            printf("%s\n", lang_get(STR_INVALID_CHOICE));
        }

        printf("\n");
    }
}
