//相对地址：p_adic/ast.h
#ifndef AST_H
#define AST_H

#include <stdbool.h>

// AST节点类型枚举
typedef enum {
    NODE_NUMBER,
    NODE_VARIABLE,
    NODE_ADD,
    NODE_SUB,
    NODE_MUL,
    NODE_DIV,
    NODE_NEG,
    NODE_SQRT,
    NODE_POW,
    NODE_EXP,
    NODE_LOG,
    NODE_PAREN
} NodeType;

// 有理数结构体（从padic.h中复制过来，避免循环依赖）
typedef struct {
    int numerator;
    int denominator;
} RationalNumber;

// AST节点结构
typedef struct ASTNode {
    NodeType type;
    union {
        int int_value;
        char* var_name;
    } value;
    int sqrt_index;
    struct ASTNode* left;
    struct ASTNode* right;
} ASTNode;

// 词法分析相关结构
typedef enum {
    TOKEN_NUMBER, TOKEN_PLUS, TOKEN_MINUS, TOKEN_MUL, TOKEN_DIV,
    TOKEN_SQRT, TOKEN_POW, TOKEN_EXP, TOKEN_LOG, TOKEN_COMMA,
    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_VARIABLE,
    TOKEN_END, TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    union {
        int int_value;
        char* var_name;
    } value;
    int sqrt_index;
} Token;

typedef struct {
    const char* input;
    int position;
    Token current_token;
} ParserState;

// 函数声明
ASTNode* parse(const char* input);
void free_ast(ASTNode* node);
int evaluate_ast(ASTNode* node);
RationalNumber evaluate_ast_as_rational(ASTNode* node);
// 返回 true 表示求值成功；false 表示 AST 中含有需要 p-adic 专用处理的节点
// (sqrt / exp / log / 非整数指数的 pow / 变量) 或运算出错 (例如除零)
bool evaluate_ast_safe(ASTNode* node, RationalNumber* out);
// 是否为纯有理表达式 (只含 +-*/ 与括号和可立即求值的 pow)
bool ast_is_rational_only(ASTNode* node);
void print_ast(ASTNode* node, int indent);
void ast_to_string(ASTNode* node, char* buffer, int* pos);

// 词法分析器函数
Token get_next_token(ParserState* state);
void skip_whitespace(ParserState* state);

// 解析器函数
ASTNode* parse_expression(ParserState* state);
ASTNode* parse_term(ParserState* state);
ASTNode* parse_factor(ParserState* state);
ASTNode* parse_sqrt(ParserState* state);
ASTNode* parse_pow(ParserState* state);
ASTNode* parse_unary_func(ParserState* state, NodeType type);

// 节点创建函数
ASTNode* create_number_node(int value);
ASTNode* create_variable_node(const char* name);
ASTNode* create_unary_node(NodeType type, ASTNode* child);
ASTNode* create_binary_node(NodeType type, ASTNode* left, ASTNode* right);
ASTNode* create_sqrt_node(int index, ASTNode* expr);

// 辅助函数
const char* node_type_to_string(NodeType type);
bool is_variable_char(char c, bool first_char);
bool matches_string(ParserState* state, const char* str);
bool is_sqrt_function(ParserState* state);
Token read_number(ParserState* state);
Token read_variable(ParserState* state);

#endif // AST_H