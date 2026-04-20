#include "ui.h"
#include "padic.h"
#include "padic_root.h"
#include "lang.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 处理 pow 语法输入
void handle_pow_input(char* input) {
    char base_str[256];
    char exp_str[256];
    int p = 0;

    if (sscanf(input, "pow(%[^,],%[^)]) %d", base_str, exp_str, &p) != 3) {
        printf("%s\n", lang_get(STR_ERR_POW_FORMAT));
        return;
    }

    if (p <= 1) {
        printf("%s\n", lang_get(STR_ERR_P_VALUE));
        return;
    }

    RationalNumber* base_rat = parse_rational(base_str);
    RationalNumber* exp_rat  = parse_rational(exp_str);

    if (base_rat == NULL || exp_rat == NULL) {
        printf("%s\n", lang_get(STR_ERR_PARSE));
        if (base_rat) free(base_rat);
        if (exp_rat)  free(exp_rat);
        return;
    }

    if (exp_rat->numerator != 1 || exp_rat->denominator != 2) {
        printf("%s\n", lang_get(STR_ERR_ONLY_SQRT));
        free(base_rat);
        free(exp_rat);
        return;
    }

    printf(lang_get(STR_CALC_SQRT), base_rat->numerator, base_rat->denominator, p);
    printf("\n");

    int digits = 15;
    PAdicNumber* base_padic = rational_to_padic(base_rat, p, digits);

    if (base_padic == NULL) {
        printf("%s\n", lang_get(STR_ERR_CONVERT_BASE));
        free(base_rat);
        free(exp_rat);
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
    }

    printf("\n");
    free_padic(base_padic);
    free_padic(root);
    free(base_rat);
    free(exp_rat);
}

// 处理单个输入
void handle_input(char* input) {
    if (strlen(input) == 0) return;

    if (strcmp(input, "help") == 0) {
        printf("%s\n", lang_get(STR_HELP_FORMAT));
        printf("%s\n", lang_get(STR_HELP_1));
        printf("%s\n", lang_get(STR_HELP_2));
        printf("%s\n", lang_get(STR_HELP_3));
        printf("%s\n", lang_get(STR_HELP_COMMANDS));
        return;
    } else if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0 || strcmp(input, "over") == 0) {
        printf("%s\n", lang_get(STR_EXIT));
        exit(0);
    } else if (strcmp(input, "menu") == 0) {
        printf("\n");
        main_menu();
        printf("\n%s\n", lang_get(STR_RETURN_MENU));
        return;
    } else if (strcmp(input, "language") == 0) {
        printf("%s\n", lang_get(STR_CURRENT_LANG));
        return;
    }

    if (strncmp(input, "pow(", 4) == 0) {
        handle_pow_input(input);
        return;
    }

    if (strncmp(input, "sqrt(", 5) == 0) {
        char converted[256];
        char n_str[256];
        int p = 0;

        if (sscanf(input, "sqrt(%[^)]) %d", n_str, &p) == 2) {
            snprintf(converted, sizeof(converted), "pow(%s,1/2) %d", n_str, p);
            handle_pow_input(converted);
        } else {
            printf("%s\n", lang_get(STR_ERR_SQRT_FORMAT));
        }
        return;
    }

    char num_str[256];
    int p = 0;

    char* last_space = strrchr(input, ' ');
    if (last_space == NULL) {
        printf("%s\n", lang_get(STR_ERR_FORMAT));
        return;
    }

    int num_len = last_space - input;
    strncpy(num_str, input, num_len);
    num_str[num_len] = '\0';

    if (sscanf(last_space + 1, "%d", &p) != 1 || p <= 1) {
        printf("%s\n", lang_get(STR_ERR_P_VALUE));
        return;
    }

    RationalNumber* rat = parse_rational(num_str);
    if (rat == NULL) {
        printf("%s\n", lang_get(STR_ERR_PARSE));
        return;
    }

    int max_digits = 200;
    PAdicNumber* padic = rational_to_padic(rat, p, max_digits);

    if (padic != NULL) {
        print_padic_formal(padic, max_digits);
        print_padic_series(padic, max_digits);
    }

    printf("\n");
    free_padic(padic);
    free(rat);
}

int main() {
    // 初始化语言
    lang_init();

    char input[256];

    while (1) {
        printf("%s", lang_get(STR_PROMPT));
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) break;

        input[strcspn(input, "\n")] = '\0';

        handle_input(input);
    }

    return 0;
}
