#ifndef LANG_H
#define LANG_H

typedef enum {
    LANG_EN, LANG_ZH_CN, LANG_ZH_TW, LANG_JA,
    LANG_KO, LANG_FR, LANG_RU, LANG_ES, LANG_DE
} LangCode;

typedef enum {
    STR_PROMPT,
    STR_EXPR_PROMPT,
    STR_MENU_CHOICE,
    STR_PRESS_ENTER,
    STR_GOODBYE,
    STR_INVALID_CHOICE,
    STR_EXIT,

    STR_MENU_TITLE,
    STR_MENU_SEPARATOR,
    STR_MENU_1,
    STR_MENU_2,
    STR_MENU_3,
    STR_MENU_4,
    STR_MENU_5,
    STR_MENU_6,

    STR_HELP_FORMAT,
    STR_HELP_1,
    STR_HELP_2,
    STR_HELP_3,
    STR_HELP_COMMANDS,

    STR_ERR_FORMAT,
    STR_ERR_P_VALUE,
    STR_ERR_PARSE,
    STR_ERR_POW_FORMAT,
    STR_ERR_SQRT_FORMAT,
    STR_ERR_ONLY_SQRT,
    STR_ERR_CONVERT_BASE,
    STR_ERR_NO_SQRT,
    STR_ERR_INPUT_FORMAT,
    STR_ERR_EXPR_FORMAT,
    STR_ERR_EXPR_PARSE,

    STR_INTERACTIVE_TITLE,
    STR_INTERACTIVE_HINT_INT,
    STR_INTERACTIVE_HINT_FRAC,
    STR_INTERACTIVE_BACK,
    STR_RATIONAL_NUMBER,
    STR_PADIC_BASE,
    STR_CONVERT_FAIL,

    STR_EXPR_TITLE,
    STR_EXPR_HINT,
    STR_EXPR_LABEL,
    STR_EXPR_RESULT,

    STR_TEST_INT_TITLE,
    STR_TEST_RAT_TITLE,
    STR_TEST_CASE,
    STR_TEST_CASE_RAT,

    STR_CALC_SQRT,
    STR_FIRST_ROOT,
    STR_SECOND_ROOT,

    STR_RETURN_MENU,
    STR_CURRENT_LANG,

    STR_FORMAL_LABEL,
    STR_SERIES_LABEL,
    STR_FORMAL_LARGE_BASE,

    STR_ROOT_RESULT_TITLE,   // "方根运算结果 (精度: %d位)"

    STR_CALC_EXP,            // "Computing: exp(%d/%d) in %d-adic"
    STR_CALC_LOG,            // "Computing: log(%d/%d) in %d-adic"
    STR_ERR_EXP_FORMAT,      // "Format error. Usage: exp(x) p"
    STR_ERR_LOG_FORMAT,      // "Format error. Usage: log(x) p"
    STR_ERR_EXP_DIVERGE,     // "exp(x) does not converge in Q_p: need v_p(x) >= 1 (>=2 if p=2)"
    STR_ERR_LOG_DIVERGE,     // "log(x) does not converge in Q_p: need x ≡ 1 (mod p)"
    STR_ERR_EXPR_TO_PADIC,   // "Failed to convert expression to p-adic"
    STR_EXP_RESULT,          // "exp result:"
    STR_LOG_RESULT,          // "log result:"
    STR_HELP_4,              // "  4. exp(x) p / log(x) p  (e.g. exp(5) 5)"
    STR_HELP_EXPR,           // "  5. expression p         (e.g. 3+4*2 5, (1/3+1)*2 5)"

    STR_COUNT
} StrIndex;

void lang_init();
const char* lang_get(StrIndex index);
LangCode lang_current();

#endif // LANG_H
