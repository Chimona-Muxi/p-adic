//相对地址：p_adic/ast.c
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// 在 ast.c 开头添加
// 最大公约数函数（从padic.c复制过来，避免循环依赖）
static int ast_gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

static RationalNumber add_rational(RationalNumber a, RationalNumber b) {
    int num = a.numerator * b.denominator + b.numerator * a.denominator;
    int den = a.denominator * b.denominator;
    int g = ast_gcd(abs(num), abs(den));
    return (RationalNumber){num/g, den/g};
}
// 类似实现 subtract_rational, multiply_rational, divide_rational

// 有理数减法
static RationalNumber subtract_rational(RationalNumber a, RationalNumber b) {
    int num = a.numerator * b.denominator - b.numerator * a.denominator;
    int den = a.denominator * b.denominator;
    int g = ast_gcd(abs(num), abs(den));
    return (RationalNumber){num/g, den/g};
}

// 有理数乘法
static RationalNumber multiply_rational(RationalNumber a, RationalNumber b) {
    int num = a.numerator * b.numerator;
    int den = a.denominator * b.denominator;
    int g = ast_gcd(abs(num), abs(den));
    return (RationalNumber){num/g, den/g};
}

// 有理数除法
static RationalNumber divide_rational(RationalNumber a, RationalNumber b) {
    if (b.numerator == 0) {
        fprintf(stderr, "Error: Division by zero in rational number\n");
        return (RationalNumber){0, 1};
    }
    int num = a.numerator * b.denominator;
    int den = a.denominator * b.numerator;
    int g = ast_gcd(abs(num), abs(den));
    return (RationalNumber){num/g, den/g};
}

// 有理数取负
static RationalNumber negate_rational(RationalNumber a) {
    return (RationalNumber){-a.numerator, a.denominator};
}

RationalNumber evaluate_ast_as_rational(ASTNode* node) {
    if (node == NULL) return (RationalNumber){0, 1};
    
    switch (node->type) {
        case NODE_NUMBER:
            return (RationalNumber){node->value.int_value, 1};
            
        case NODE_ADD: {
            RationalNumber left = evaluate_ast_as_rational(node->left);
            RationalNumber right = evaluate_ast_as_rational(node->right);
            return add_rational(left, right);
        }
            
        case NODE_SUB: {
            RationalNumber left = evaluate_ast_as_rational(node->left);
            RationalNumber right = evaluate_ast_as_rational(node->right);
            return subtract_rational(left, right);
        }
            
        case NODE_MUL: {
            RationalNumber left = evaluate_ast_as_rational(node->left);
            RationalNumber right = evaluate_ast_as_rational(node->right);
            return multiply_rational(left, right);
        }
            
        case NODE_DIV: {
            RationalNumber left = evaluate_ast_as_rational(node->left);
            RationalNumber right = evaluate_ast_as_rational(node->right);
            return divide_rational(left, right);
        }
            
        case NODE_NEG: {
            RationalNumber val = evaluate_ast_as_rational(node->left);
            return negate_rational(val);
        }
            
        case NODE_PAREN:
            return evaluate_ast_as_rational(node->left);
            
        case NODE_POW: {
            // 暂时不支持幂运算
            fprintf(stderr, "Warning: pow() not fully supported in rational evaluation\n");
            return (RationalNumber){0, 1};
        }
            
        // 对于 SQRT 和 VARIABLE，暂时返回整数0
        case NODE_SQRT:
        case NODE_VARIABLE:
            fprintf(stderr, "Warning: sqrt and variable not fully supported in rational evaluation\n");
            return (RationalNumber){0, 1};
            
        default:
            return (RationalNumber){0, 1};
    }
}

// 解析入口函数
ASTNode* parse(const char* input) {
    ParserState state;
    state.input = input;
    state.position = 0;
    
    // 获取第一个标记
    state.current_token = get_next_token(&state);
    
    // 解析表达式
    return parse_expression(&state);
}

// 释放AST内存
void free_ast(ASTNode* node) {
    if (node == NULL) return;
    
    free_ast(node->left);
    free_ast(node->right);
    
    if (node->type == NODE_VARIABLE && node->value.var_name != NULL) {
        free(node->value.var_name);
    }
    
    free(node);
}

// 计算AST的值（简化版，只处理整数和基本运算）
int evaluate_ast(ASTNode* node) {
    if (node == NULL) return 0;
    
    switch (node->type) {
        case NODE_NUMBER:
            return node->value.int_value;
            
        case NODE_ADD:
            return evaluate_ast(node->left) + evaluate_ast(node->right);
            
        case NODE_SUB:
            return evaluate_ast(node->left) - evaluate_ast(node->right);
            
        case NODE_MUL:
            return evaluate_ast(node->left) * evaluate_ast(node->right);
            
        case NODE_DIV: {
            int right_val = evaluate_ast(node->right);
            if (right_val == 0) {
                fprintf(stderr, "Error: Division by zero\n");
                return 0;
            }
            return evaluate_ast(node->left) / right_val;
        }
            
        case NODE_NEG:
            return -evaluate_ast(node->left);
            
        case NODE_PAREN:
            return evaluate_ast(node->left);
            
        case NODE_POW: {
            // 暂时不支持幂运算
            fprintf(stderr, "Error: Power evaluation not supported yet\n");
            return 0;
        }
            
        case NODE_SQRT: {
            int val = evaluate_ast(node->left);
            if (val < 0 && node->sqrt_index % 2 == 0) {
                fprintf(stderr, "Error: Even root of negative number\n");
                return 0;
            }
            
            // 简化：返回整数平方根
            if (node->sqrt_index == 2) {
                return (int)sqrt(val);
            } else {
                // 对于其他根，返回近似值
                return (int)pow(val, 1.0 / node->sqrt_index);
            }
        }
            
        case NODE_VARIABLE:
            fprintf(stderr, "Error: Variable evaluation not supported yet\n");
            return 0;
            
        default:
            return 0;
    }
}

// 打印AST（树形结构）
void print_ast(ASTNode* node, int indent) {
    if (node == NULL) return;
    
    // 打印缩进
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
    
    // 打印节点信息
    switch (node->type) {
        case NODE_NUMBER:
            printf("NUMBER: %d\n", node->value.int_value);
            break;
            
        case NODE_VARIABLE:
            printf("VARIABLE: %s\n", node->value.var_name);
            break;
            
        case NODE_ADD:
            printf("ADD\n");
            print_ast(node->left, indent + 1);
            print_ast(node->right, indent + 1);
            break;
            
        case NODE_SUB:
            printf("SUB\n");
            print_ast(node->left, indent + 1);
            print_ast(node->right, indent + 1);
            break;
            
        case NODE_MUL:
            printf("MUL\n");
            print_ast(node->left, indent + 1);
            print_ast(node->right, indent + 1);
            break;
            
        case NODE_DIV:
            printf("DIV\n");
            print_ast(node->left, indent + 1);
            print_ast(node->right, indent + 1);
            break;
            
        case NODE_NEG:
            printf("NEG\n");
            print_ast(node->left, indent + 1);
            break;
            
        case NODE_SQRT:
            printf("SQRT[%d]\n", node->sqrt_index);
            print_ast(node->left, indent + 1);
            break;
            
        case NODE_POW:
            printf("POW\n");
            print_ast(node->left, indent + 1);
            print_ast(node->right, indent + 1);
            break;
            
        case NODE_PAREN:
            printf("PAREN\n");
            print_ast(node->left, indent + 1);
            break;
    }
}

// 将AST转换为字符串表达式
void ast_to_string(ASTNode* node, char* buffer, int* pos) {
    if (node == NULL) return;
    
    switch (node->type) {
        case NODE_NUMBER: {
            int n = snprintf(buffer + *pos, 100, "%d", node->value.int_value);
            *pos += n;
            break;
        }
            
        case NODE_VARIABLE: {
            int n = snprintf(buffer + *pos, 100, "%s", node->value.var_name);
            *pos += n;
            break;
        }
            
        case NODE_ADD:
            ast_to_string(node->left, buffer, pos);
            int n = snprintf(buffer + *pos, 100, " + ");
            *pos += n;
            ast_to_string(node->right, buffer, pos);
            break;
            
        case NODE_SUB:
            ast_to_string(node->left, buffer, pos);
            n = snprintf(buffer + *pos, 100, " - ");
            *pos += n;
            ast_to_string(node->right, buffer, pos);
            break;
            
        case NODE_MUL:
            ast_to_string(node->left, buffer, pos);
            n = snprintf(buffer + *pos, 100, " * ");
            *pos += n;
            ast_to_string(node->right, buffer, pos);
            break;
            
        case NODE_DIV:
            ast_to_string(node->left, buffer, pos);
            n = snprintf(buffer + *pos, 100, " / ");
            *pos += n;
            ast_to_string(node->right, buffer, pos);
            break;
            
        case NODE_NEG:
            n = snprintf(buffer + *pos, 100, "-");
            *pos += n;
            ast_to_string(node->left, buffer, pos);
            break;
            
        case NODE_SQRT:
            if (node->sqrt_index != 2) {
                n = snprintf(buffer + *pos, 100, "%d", node->sqrt_index);
                *pos += n;
            }
            n = snprintf(buffer + *pos, 100, "sqrt(");
            *pos += n;
            ast_to_string(node->left, buffer, pos);
            n = snprintf(buffer + *pos, 100, ")");
            *pos += n;
            break;
            
        case NODE_POW:
            n = snprintf(buffer + *pos, 100, "pow(");
            *pos += n;
            ast_to_string(node->left, buffer, pos);
            n = snprintf(buffer + *pos, 100, ",");
            *pos += n;
            ast_to_string(node->right, buffer, pos);
            n = snprintf(buffer + *pos, 100, ")");
            *pos += n;
            break;
            
        case NODE_PAREN:
            n = snprintf(buffer + *pos, 100, "(");
            *pos += n;
            ast_to_string(node->left, buffer, pos);
            n = snprintf(buffer + *pos, 100, ")");
            *pos += n;
            break;
    }
}

// 获取下一个标记
Token get_next_token(ParserState* state) {
    skip_whitespace(state);
    
    if (state->input[state->position] == '\0') {
        Token token;
        token.type = TOKEN_END;
        return token;
    }
    
    char current = state->input[state->position];
    

    // 检查是否是 pow 函数
    if (is_variable_char(current, true)) {
        // 检查是否是 pow 函数
        if (matches_string(state, "pow")) {
            Token token;
            token.type = TOKEN_POW;
            state->position += 3;
            return token;
        }
        // 检查是否是 sqrt 函数
        else if (is_sqrt_function(state)) {
            Token token;
            token.type = TOKEN_SQRT;
            token.sqrt_index = 2;
            state->position += 4;
            return token;
        }
        return read_variable(state);
    }

    // 处理数字
    if (isdigit(current)) {
        int start_pos = state->position;
        Token num_token = read_number(state);
        
        // 检查数字后面是否紧跟着sqrt
        skip_whitespace(state);
        
        if (is_sqrt_function(state)) {
            // 这是一个数字+sqrt组合，如3sqrt
            // 创建sqrt token
            Token token;
            token.type = TOKEN_SQRT;
            token.sqrt_index = num_token.value.int_value;  // 使用数字作为根指数
            
            // 跳过"sqrt"
            state->position += 4;
            
            return token;
        } else {
            // 不是数字+sqrt组合，返回数字token
            // 需要恢复位置，因为read_number已经移动了位置
            state->position = start_pos;
            return read_number(state);
        }
    }
    
    // 处理变量
    if (is_variable_char(current, true)) {
        // 检查是否是sqrt函数
        if (is_sqrt_function(state)) {
            Token token;
            token.type = TOKEN_SQRT;
            token.sqrt_index = 2;  // 默认平方根
            
            // 跳过"sqrt"
            state->position += 4;
            
            return token;
        }
        return read_variable(state);
    }
    
    // 处理运算符
    Token token;
    token.sqrt_index = 2;
    
    switch (current) {
        case '+':
            token.type = TOKEN_PLUS;
            state->position++;
            break;
            
        case '-':
            token.type = TOKEN_MINUS;
            state->position++;
            break;
            
        case '*':
            token.type = TOKEN_MUL;
            state->position++;
            break;
            
        case '/':
            token.type = TOKEN_DIV;
            state->position++;
            break;
            
        case '(':
            token.type = TOKEN_LPAREN;
            state->position++;
            break;
            
        case ')':
            token.type = TOKEN_RPAREN;
            state->position++;
            break;
            
        default:
            token.type = TOKEN_ERROR;
            state->position++;
    }
    
    return token;
}

// 跳过空白字符
void skip_whitespace(ParserState* state) {
    while (state->input[state->position] == ' ' || 
           state->input[state->position] == '\t' ||
           state->input[state->position] == '\n') {
        state->position++;
    }
}

// 解析表达式：+ 和 -
ASTNode* parse_expression(ParserState* state) {
    ASTNode* node = parse_term(state);
    
    while (state->current_token.type == TOKEN_PLUS || 
           state->current_token.type == TOKEN_MINUS) {
        Token op = state->current_token;
        state->current_token = get_next_token(state);
        
        ASTNode* right = parse_term(state);
        
        if (op.type == TOKEN_PLUS) {
            node = create_binary_node(NODE_ADD, node, right);
        } else {
            node = create_binary_node(NODE_SUB, node, right);
        }
    }
    
    return node;
}

// 解析项：* 和 /
ASTNode* parse_term(ParserState* state) {
    ASTNode* node = parse_factor(state);
    
    while (state->current_token.type == TOKEN_MUL || 
           state->current_token.type == TOKEN_DIV) {
        Token op = state->current_token;
        state->current_token = get_next_token(state);
        
        ASTNode* right = parse_factor(state);
        
        if (op.type == TOKEN_MUL) {
            node = create_binary_node(NODE_MUL, node, right);
        } else {
            node = create_binary_node(NODE_DIV, node, right);
        }
    }
    
    return node;
}

// 解析因子：数字、变量、括号表达式、sqrt表达式
ASTNode* parse_factor(ParserState* state) {
    Token token = state->current_token;
    ASTNode* node = NULL;
    
    if (token.type == TOKEN_NUMBER) {
        node = create_number_node(token.value.int_value);
        state->current_token = get_next_token(state);
    }
    else if (token.type == TOKEN_VARIABLE) {
        node = create_variable_node(token.value.var_name);
        state->current_token = get_next_token(state);
    }
    else if (token.type == TOKEN_MINUS) {
        // 一元负号
        state->current_token = get_next_token(state);
        ASTNode* factor = parse_factor(state);
        node = create_unary_node(NODE_NEG, factor);
    }
    else if (token.type == TOKEN_LPAREN) {
        state->current_token = get_next_token(state);
        ASTNode* expr = parse_expression(state);
        
        if (state->current_token.type != TOKEN_RPAREN) {
            fprintf(stderr, "Error: Expected ')'\n");
            free_ast(expr);
            return NULL;
        }
        
        state->current_token = get_next_token(state);
        
        // 创建括号节点
        ASTNode* paren_node = (ASTNode*)malloc(sizeof(ASTNode));
        paren_node->type = NODE_PAREN;
        paren_node->left = expr;
        paren_node->right = NULL;
        node = paren_node;
    }
    else if (token.type == TOKEN_SQRT) {
        node = parse_sqrt(state);
    }
    else {
        fprintf(stderr, "Error: Unexpected token in factor\n");
        return NULL;
    }
    
    return node;
}

// 解析sqrt表达式
ASTNode* parse_sqrt(ParserState* state) {
    Token token = state->current_token;
    int sqrt_index = token.sqrt_index;
    
    // 消耗sqrt标记
    state->current_token = get_next_token(state);
    
    // 跳过可能的空白
    skip_whitespace(state);
    
    // 检查是否有括号
    if (state->current_token.type != TOKEN_LPAREN) {
        fprintf(stderr, "Error: Expected '(' after sqrt\n");
        return NULL;
    }
    
    // 消耗左括号
    state->current_token = get_next_token(state);
    
    // 解析括号内的表达式
    ASTNode* expr = parse_expression(state);
    
    if (state->current_token.type != TOKEN_RPAREN) {
        fprintf(stderr, "Error: Expected ')' after sqrt expression\n");
        free_ast(expr);
        return NULL;
    }
    
    // 消耗右括号
    state->current_token = get_next_token(state);
    
    return create_sqrt_node(sqrt_index, expr);
}

// 创建AST节点
ASTNode* create_number_node(int value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_NUMBER;
    node->value.int_value = value;
    node->sqrt_index = 2;  // 默认平方根
    node->left = NULL;
    node->right = NULL;
    return node;
}

ASTNode* create_variable_node(const char* name) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_VARIABLE;
    node->value.var_name = strdup(name);
    node->sqrt_index = 2;
    node->left = NULL;
    node->right = NULL;
    return node;
}

ASTNode* create_unary_node(NodeType type, ASTNode* child) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->sqrt_index = 2;
    node->left = child;
    node->right = NULL;
    return node;
}

ASTNode* create_binary_node(NodeType type, ASTNode* left, ASTNode* right) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->sqrt_index = 2;
    node->left = left;
    node->right = right;
    return node;
}

ASTNode* create_sqrt_node(int index, ASTNode* expr) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_SQRT;
    node->sqrt_index = index;
    node->left = expr;  // 被开方表达式
    node->right = NULL;
    return node;
}

// 打印AST节点类型
const char* node_type_to_string(NodeType type) {
    switch (type) {
        case NODE_NUMBER: return "NUMBER";
        case NODE_VARIABLE: return "VARIABLE";
        case NODE_ADD: return "ADD";
        case NODE_SUB: return "SUB";
        case NODE_MUL: return "MUL";
        case NODE_DIV: return "DIV";
        case NODE_NEG: return "NEG";
        case NODE_SQRT: return "SQRT";
        case NODE_PAREN: return "PAREN";
        default: return "UNKNOWN";
    }
}

// 检查是否为变量名（字母开头，可包含数字）
bool is_variable_char(char c, bool first_char) {
    if (first_char) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    } else {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
               (c >= '0' && c <= '9') || c == '_';
    }
}

// 检查字符串是否匹配
bool matches_string(ParserState* state, const char* str) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (state->input[state->position + i] != str[i]) {
            return false;
        }
    }
    return true;
}

// 检查是否遇到sqrt函数
bool is_sqrt_function(ParserState* state) {
    return matches_string(state, "sqrt");
}

// 读取数字
Token read_number(ParserState* state) {
    Token token;
    token.type = TOKEN_NUMBER;
    token.sqrt_index = 2;
    
    int start = state->position;
    
    // 读取整数部分
    while (isdigit(state->input[state->position])) {
        state->position++;
    }
    
    // 检查是否有小数点
    if (state->input[state->position] == '.') {
        state->position++;
        while (isdigit(state->input[state->position])) {
            state->position++;
        }
    }
    
    int len = state->position - start;
    char* num_str = (char*)malloc(len + 1);
    strncpy(num_str, state->input + start, len);
    num_str[len] = '\0';
    
    // 转换为整数（暂时只处理整数）
    token.value.int_value = atoi(num_str);
    free(num_str);
    
    return token;
}

// 读取变量名
Token read_variable(ParserState* state) {
    Token token;
    token.type = TOKEN_VARIABLE;
    token.sqrt_index = 2;
    
    int start = state->position;
    
    // 读取第一个字符
    state->position++;
    
    // 读取剩余字符
    while (is_variable_char(state->input[state->position], false)) {
        state->position++;
    }
    
    int len = state->position - start;
    token.value.var_name = (char*)malloc(len + 1);
    strncpy(token.value.var_name, state->input + start, len);
    token.value.var_name[len] = '\0';
    
    return token;
}
