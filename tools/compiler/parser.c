/**
 * DOS Java Compiler - Parser Implementation (MVP)
 * 
 * Minimal Viable Product: Parses simple Java programs
 * Supports: class, public static void main(), int variables, System.out.println()
 */

#include "parser.h"
#include "ast.h"
#include <string.h>
#include <stdlib.h>

/* Forward declarations */
static int read_token_from_file(Parser* parser, Token* token);
static uint16_t parse_if_stmt(Parser* parser);
static uint16_t parse_while_stmt(Parser* parser);
static uint16_t parse_return_stmt(Parser* parser);

/**
 * Initialize parser
 */
int parser_init(Parser* parser, const char* token_file, const char* ast_file) {
    long file_size;
    long string_pool_offset;
    
    /* Initialize all fields */
    parser->tokens = NULL;
    parser->ast_file = NULL;
    parser->node_count = 0;
    parser->total_nodes = 0;
    parser->pool_size = 0;
    parser->has_error = 0;
    parser->error_count = 0;
    
    /* Open token input file */
    parser->tokens = fopen(token_file, "rb");
    if (!parser->tokens) {
        fprintf(stderr, "Error: Cannot open token file '%s'\n", token_file);
        return -1;
    }
    
    /* Read string pool size from header */
    if (fread(&parser->pool_size, sizeof(uint16_t), 1, parser->tokens) != 1) {
        fprintf(stderr, "Error: Cannot read string pool size\n");
        fclose(parser->tokens);
        return -1;
    }
    
    /* Calculate string pool offset (at end of file) */
    fseek(parser->tokens, 0, SEEK_END);
    file_size = ftell(parser->tokens);
    string_pool_offset = file_size - parser->pool_size;
    
    /* Read string pool from end of file */
    if (parser->pool_size > 0) {
        fseek(parser->tokens, string_pool_offset, SEEK_SET);
        if (fread(parser->string_pool, 1, parser->pool_size, parser->tokens) != parser->pool_size) {
            fprintf(stderr, "Error: Cannot read string pool\n");
            fclose(parser->tokens);
            return -1;
        }
    }
    
    /* Reset to token data (after header) */
    fseek(parser->tokens, sizeof(uint16_t), SEEK_SET);
    
    /* Open AST output file */
    parser->ast_file = fopen(ast_file, "wb");
    if (!parser->ast_file) {
        fprintf(stderr, "Error: Cannot create AST file '%s'\n", ast_file);
        fclose(parser->tokens);
        return -1;
    }
    
    /* Read first two tokens */
    if (read_token_from_file(parser, &parser->current) < 0) {
        parser_cleanup(parser);
        return -1;
    }
    if (read_token_from_file(parser, &parser->lookahead) < 0) {
        parser_cleanup(parser);
        return -1;
    }
    
    return 0;
}

/**
 * Read token from file
 */
static int read_token_from_file(Parser* parser, Token* token) {
    size_t read;
    
    read = fread(token, sizeof(Token), 1, parser->tokens);
    if (read != 1) {
        if (feof(parser->tokens)) {
            token->type = TOK_EOF;
            return 0;
        }
        return -1;
    }
    
    return 0;
}

/**
 * Advance to next token
 */
int parser_next_token(Parser* parser) {
    parser->current = parser->lookahead;
    return read_token_from_file(parser, &parser->lookahead);
}

/**
 * Check if current token matches type
 */
int parser_match(Parser* parser, TokenType type) {
    return ((uint8_t)parser->current.type) == ((uint8_t)type);
}

/**
 * Consume token if it matches
 */
int parser_consume(Parser* parser, TokenType type) {
    if (((uint8_t)parser->current.type) == ((uint8_t)type)) {
        parser_next_token(parser);
        return 1;
    }
    return 0;
}

/**
 * Expect token and consume it
 */
int parser_expect(Parser* parser, TokenType type) {
    if (((uint8_t)parser->current.type) != ((uint8_t)type)) {
        fprintf(stderr, "Error: Expected %s but got %s at line %d\n",
                token_type_name((TokenType)((uint8_t)type)),
                token_type_name((TokenType)((uint8_t)parser->current.type)),
                parser->current.line);
        parser->has_error = 1;
        parser->error_count++;
        return -1;
    }
    parser_next_token(parser);
    return 0;
}

/**
 * Add string to string pool
 */
uint16_t parser_add_string(Parser* parser, const char* str) {
    uint16_t offset;
    size_t len;
    
    len = strlen(str) + 1;
    
    if (parser->pool_size + len > sizeof(parser->string_pool)) {
        return 0xFFFF;
    }
    
    offset = parser->pool_size;
    strcpy(parser->string_pool + offset, str);
    parser->pool_size += (uint16_t)len;
    
    return offset;
}

/**
 * Get string from string pool
 */
const char* parser_get_string(Parser* parser, uint16_t offset) {
    if (offset >= parser->pool_size) {
        return NULL;
    }
    return parser->string_pool + offset;
}

/**
 * Allocate new AST node
 * Note: Returns 1-based index (0 means error)
 */
uint16_t parser_alloc_node(Parser* parser, NodeType type) {
    uint16_t index;
    
    if (parser->node_count >= 512) {
        /* Flush buffer */
        if (parser_flush_nodes(parser) < 0) {
            return 0;
        }
    }
    
    index = parser->node_count;
    parser->nodes[index].type = type;
    parser->nodes[index].line = parser->current.line;
    parser->nodes[index].column = parser->current.column;
    parser->nodes[index].next_sibling = 0;
    parser->node_count++;
    
    /* Return 1-based index (total_nodes + index + 1) */
    return parser->total_nodes + index + 1;
}

/**
 * Flush node buffer to file
 */
int parser_flush_nodes(Parser* parser) {
    size_t written;
    
    if (parser->node_count == 0) {
        return 0;
    }
    
    written = fwrite(parser->nodes, sizeof(ASTNode), parser->node_count, parser->ast_file);
    if (written != parser->node_count) {
        fprintf(stderr, "Error: Failed to write AST nodes\n");
        return -1;
    }
    
    parser->total_nodes += parser->node_count;
    parser->node_count = 0;
    
    return 0;
}

/**
 * Parse entire program
 * Program -> ClassDecl
 */
uint16_t parser_parse(Parser* parser) {
    uint16_t program_node;
    uint16_t class_node;
    FILE* temp_file;
    FILE* original_ast_file;
    size_t nodes_size;
    uint8_t* nodes_buffer;
    
    /* Save original AST file pointer */
    original_ast_file = parser->ast_file;
    
    /* Create temporary file for nodes */
    temp_file = tmpfile();
    if (!temp_file) {
        fprintf(stderr, "Error: Cannot create temporary file\n");
        return 0;
    }
    
    /* Temporarily redirect node output to temp file */
    parser->ast_file = temp_file;
    
    /* Allocate program node */
    program_node = parser_alloc_node(parser, NODE_PROGRAM);
    if (program_node == 0) {
        fclose(temp_file);
        parser->ast_file = original_ast_file;
        return 0;
    }
    
    /* Parse class */
    class_node = parse_class(parser);
    if (class_node == 0) {
        fclose(temp_file);
        parser->ast_file = original_ast_file;
        return 0;
    }
    
    parser->nodes[program_node - parser->total_nodes - 1].data.program.class_node = class_node;
    /* Expect EOF */
    if (!parser_match(parser, TOK_EOF)) {
        parser_error(parser, "Expected end of file");
        fclose(temp_file);
        parser->ast_file = original_ast_file;
        return 0;
    }
    
    /* Flush remaining nodes to temp file */
    if (parser_flush_nodes(parser) < 0) {
        fclose(temp_file);
        parser->ast_file = original_ast_file;
        return 0;
    }
    
    /* Read all nodes from temp file */
    nodes_size = parser->total_nodes * sizeof(ASTNode);
    nodes_buffer = (uint8_t*)malloc(nodes_size);
    if (!nodes_buffer) {
        fprintf(stderr, "Error: Out of memory\n");
        fclose(temp_file);
        parser->ast_file = original_ast_file;
        return 0;
    }
    
    fseek(temp_file, 0, SEEK_SET);
    if (fread(nodes_buffer, 1, nodes_size, temp_file) != nodes_size) {
        fprintf(stderr, "Error: Failed to read nodes from temp file\n");
        free(nodes_buffer);
        fclose(temp_file);
        parser->ast_file = original_ast_file;
        return 0;
    }
    
    fclose(temp_file);
    
    /* Restore original AST file pointer */
    parser->ast_file = original_ast_file;
    
    /* Now write to actual AST file in correct order */
    /* Write header */
    fwrite(&parser->total_nodes, sizeof(uint16_t), 1, parser->ast_file);
    fwrite(&parser->pool_size, sizeof(uint16_t), 1, parser->ast_file);
    
    /* Write string pool */
    if (parser->pool_size > 0) {
        fwrite(parser->string_pool, 1, parser->pool_size, parser->ast_file);
    }
    
    /* Write nodes */
    fwrite(nodes_buffer, 1, nodes_size, parser->ast_file);
    
    free(nodes_buffer);
    
    return program_node;
}

/**
 * Parse class declaration
 * ClassDecl -> 'class' ID '{' MemberDecl* '}'
 */
uint16_t parse_class(Parser* parser) {
    uint16_t class_node;
    uint16_t name_offset;
    uint16_t first_member;
    uint16_t member_count;
    uint16_t member_node;
    uint16_t prev_member;
    
    /* Expect 'class' */
    if (parser_expect(parser, TOK_CLASS) < 0) {
        return 0;
    }
    
    /* Expect class name */
    if (!parser_match(parser, TOK_IDENTIFIER)) {
        parser_error(parser, "Expected class name");
        return 0;
    }
    
    /* Get class name from lexer's string pool (stored in token value) */
    name_offset = parser->current.value.str_offset;
    parser_next_token(parser);
    
    /* Allocate class node */
    class_node = parser_alloc_node(parser, NODE_CLASS);
    if (class_node == 0) {
        return 0;
    }
    
    /* Expect '{' */
    if (parser_expect(parser, TOK_LBRACE) < 0) {
        return 0;
    }
    
    /* Parse members */
    first_member = 0;
    member_count = 0;
    prev_member = 0;
    
    while (!parser_match(parser, TOK_RBRACE) && !parser_match(parser, TOK_EOF)) {
        member_node = parse_member(parser);
        if (member_node == 0) {
            return 0;
        }
        
        if (first_member == 0) {
            first_member = member_node;
        }
        
        if (prev_member != 0) {
            parser->nodes[prev_member - parser->total_nodes - 1].next_sibling = member_node;
        }
        
        prev_member = member_node;
        member_count++;
    }
    
    /* Expect '}' */
    if (parser_expect(parser, TOK_RBRACE) < 0) {
        return 0;
    }
    
    /* Fill class node */
    parser->nodes[class_node - parser->total_nodes - 1].data.class_decl.name = name_offset;
    parser->nodes[class_node - parser->total_nodes - 1].data.class_decl.member_count = member_count;
    parser->nodes[class_node - parser->total_nodes - 1].data.class_decl.first_member = first_member;
    
    return class_node;
}

/**
 * Parse member declaration (method only for MVP)
 * MemberDecl -> 'public' 'static' 'void' ID '(' ')' Block
 */
uint16_t parse_member(Parser* parser) {
    int is_public;
    int is_static;
    TypeInfo return_type;
    
    is_public = 0;
    is_static = 0;
    
    /* Initialize return_type to void by default */
    return_type.kind = TYPE_VOID;
    return_type.class_name = 0;
    
    /* Check for 'public' */
    if (parser_consume(parser, TOK_PUBLIC)) {
        is_public = 1;
    }
    
    /* Check for 'static' */
    if (parser_consume(parser, TOK_STATIC)) {
        is_static = 1;
    }
    
    /* Parse return type (optional - defaults to void) */
    if (parser_match(parser, TOK_VOID) || parser_match(parser, TOK_INT) || parser_match(parser, TOK_BOOLEAN)) {
        if (parse_type(parser, &return_type) < 0) {
            return 0;
        }
    }
    
    
    
    /* For MVP, only support methods */
    return parse_method(parser, is_public, is_static, return_type);
}

/**
 * Parse method declaration
 * MethodDecl -> ID '(' ')' Block
 */
uint16_t parse_method(Parser* parser, int is_public, int is_static, TypeInfo return_type) {
    uint16_t method_node;
    uint16_t name_offset;
    uint16_t body_node;
    
    /* Expect method name */
    if (!parser_match(parser, TOK_IDENTIFIER)) {
        parser_error(parser, "Expected method name");
        return 0;
    }
    
    name_offset = parser->current.value.str_offset;
    parser_next_token(parser);
    
    /* Allocate method node */
    method_node = parser_alloc_node(parser, NODE_METHOD);
    if (method_node == 0) {
        return 0;
    }
    
    /* Expect '(' */
    if (parser_expect(parser, TOK_LPAREN) < 0) {
        return 0;
    }
    
    /* Expect ')' (no parameters for MVP) */
    if (parser_expect(parser, TOK_RPAREN) < 0) {
        return 0;
    }
    
    /* Parse body */
    body_node = parse_block(parser);
    if (body_node == 0) {
        return 0;
    }
    
    /* Fill method node */
    parser->nodes[method_node - parser->total_nodes - 1].data.method.name = name_offset;
    parser->nodes[method_node - parser->total_nodes - 1].data.method.return_type = return_type;
    parser->nodes[method_node - parser->total_nodes - 1].data.method.is_static = is_static;
    parser->nodes[method_node - parser->total_nodes - 1].data.method.is_public = is_public;
    parser->nodes[method_node - parser->total_nodes - 1].data.method.param_count = 0;
    parser->nodes[method_node - parser->total_nodes - 1].data.method.first_param = 0;
    parser->nodes[method_node - parser->total_nodes - 1].data.method.body = body_node;
    
    return method_node;
}

/**
 * Parse type
 * Type -> 'void' | 'int' ('[' ']')? | 'boolean' ('[' ']')?
 */
int parse_type(Parser* parser, TypeInfo* type) {
    uint16_t base_kind = 0;
    
    if (parser_consume(parser, TOK_VOID)) {
        type->kind = TYPE_VOID;
        type->class_name = 0;
        return 0;
    }
    
    if (parser_consume(parser, TOK_INT)) {
        base_kind = TYPE_INT;
    } else if (parser_consume(parser, TOK_BOOLEAN)) {
        base_kind = TYPE_BOOLEAN;
    } else {
        parser_error(parser, "Expected type");
        return -1;
    }
    
    type->kind = base_kind;
    type->class_name = 0;
    
    if (parser_consume(parser, TOK_LBRACKET)) {
        if (parser_expect(parser, TOK_RBRACKET) < 0) {
            return -1;
        }
        type->kind = TYPE_ARRAY;
    }
    
    return 0;
}

/**
 * Parse block statement
 * Block -> '{' Statement* '}'
 */
uint16_t parse_block(Parser* parser) {
    uint16_t block_node;
    uint16_t first_stmt;
    uint16_t stmt_count;
    uint16_t stmt_node;
    uint16_t prev_stmt;
    
    /* Expect '{' */
    if (parser_expect(parser, TOK_LBRACE) < 0) {
        return 0;
    }
    
    /* Allocate block node */
    block_node = parser_alloc_node(parser, NODE_BLOCK);
    if (block_node == 0) {
        return 0;
    }
    
    /* Parse statements */
    first_stmt = 0;
    stmt_count = 0;
    prev_stmt = 0;
    
    while (!parser_match(parser, TOK_RBRACE) && !parser_match(parser, TOK_EOF)) {
        stmt_node = parse_statement(parser);
        if (stmt_node == 0) {
            if (parser_match(parser, TOK_RBRACE)) {
                break;
            }
            return 0;
        }
        
        if (first_stmt == 0) {
            first_stmt = stmt_node;
        }
        
        if (prev_stmt != 0) {
            parser->nodes[prev_stmt - parser->total_nodes - 1].next_sibling = stmt_node;
        }
        
        prev_stmt = stmt_node;
        stmt_count++;
    }
    
    /* Expect '}' */
    if (parser_expect(parser, TOK_RBRACE) < 0) {
        return 0;
    }
    
    /* Fill block node */
    parser->nodes[block_node - parser->total_nodes - 1].data.block.stmt_count = stmt_count;
    parser->nodes[block_node - parser->total_nodes - 1].data.block.first_stmt = first_stmt;
    
    return block_node;
}

/**
 * Parse statement
 * Statement -> Block | VarDecl | IfStmt | WhileStmt | ReturnStmt | ExprStmt
 */
uint16_t parse_statement(Parser* parser) {
    /* Block terminator is not a statement */
    if (parser_match(parser, TOK_RBRACE)) {
        return 0;
    }
    
    /* Block statement */
    if (parser_match(parser, TOK_LBRACE)) {
        return parse_block(parser);
    }
    
    /* Variable declaration */
    if (parser_match(parser, TOK_INT) || parser_match(parser, TOK_BOOLEAN)) {
        return parse_var_decl(parser);
    }
    
    /* If statement */
    if (parser_match(parser, TOK_IF)) {
        return parse_if_stmt(parser);
    }
    
    /* While statement */
    if (parser_match(parser, TOK_WHILE)) {
        return parse_while_stmt(parser);
    }
    
    /* For statement */
    if (parser_match(parser, TOK_FOR)) {
        return parse_for(parser);
    }
    
    /* Return statement */
    if (parser_match(parser, TOK_RETURN)) {
        return parse_return_stmt(parser);
    }
    
    /* Expression statement */
    return parse_expr_stmt(parser);
}

/**
 * Parse variable declaration
 * VarDecl -> Type ID ('=' Expr)? ';'
 */
uint16_t parse_var_decl(Parser* parser) {
    uint16_t var_node;
    TypeInfo type;
    uint16_t name_offset;
    uint16_t init_expr;
    
    /* Parse type */
    if (parse_type(parser, &type) < 0) {
        return 0;
    }
    
    /* Expect variable name */
    if (!parser_match(parser, TOK_IDENTIFIER)) {
        parser_error(parser, "Expected variable name");
        return 0;
    }
    
    name_offset = parser->current.value.str_offset;
    parser_next_token(parser);
    
    /* Allocate var_decl node */
    var_node = parser_alloc_node(parser, NODE_VAR_DECL);
    if (var_node == 0) {
        return 0;
    }
    
    /* Check for initializer */
    init_expr = 0;
    if (parser_consume(parser, TOK_ASSIGN)) {
        init_expr = parse_expression(parser);
        if (init_expr == 0) {
            return 0;
        }
    }
    
    /* Expect ';' */
    if (parser_expect(parser, TOK_SEMICOLON) < 0) {
        return 0;
    }
    
    /* Fill var_decl node */
    parser->nodes[var_node - parser->total_nodes - 1].data.var_decl.name = name_offset;
    parser->nodes[var_node - parser->total_nodes - 1].data.var_decl.type = type;
    parser->nodes[var_node - parser->total_nodes - 1].data.var_decl.init_expr = init_expr;
    
    return var_node;
}

/**
 * Parse expression statement
 * ExprStmt -> Expr ';'
 */
uint16_t parse_expr_stmt(Parser* parser) {
    uint16_t expr_stmt_node;
    uint16_t expr_node;
    
    /* Allocate expr_stmt node */
    expr_stmt_node = parser_alloc_node(parser, NODE_EXPR_STMT);
    if (expr_stmt_node == 0) {
        return 0;
    }
    
    /* Parse expression */
    expr_node = parse_expression(parser);
    if (expr_node == 0) {
        return 0;
    }
    
    /* Expect ';' */
    if (parser_expect(parser, TOK_SEMICOLON) < 0) {
        return 0;
    }
    
    /* Fill expr_stmt node */
    parser->nodes[expr_stmt_node - parser->total_nodes - 1].data.expr_stmt.expr = expr_node;
    
    return expr_stmt_node;
}

/**
 * Parse if statement
 * IfStmt -> 'if' '(' Expr ')' Statement ('else' Statement)?
 */
static uint16_t parse_if_stmt(Parser* parser) {
    uint16_t if_node;
    uint16_t condition;
    uint16_t then_stmt;
    uint16_t else_stmt;
    
    /* Expect 'if' */
    if (parser_expect(parser, TOK_IF) < 0) {
        return 0;
    }
    
    /* Expect '(' */
    if (parser_expect(parser, TOK_LPAREN) < 0) {
        return 0;
    }
    
    /* Parse condition */
    condition = parse_expression(parser);
    if (condition == 0) {
        return 0;
    }
    
    /* Expect ')' */
    if (parser_expect(parser, TOK_RPAREN) < 0) {
        return 0;
    }
    
    /* Parse then statement */
    then_stmt = parse_statement(parser);
    if (then_stmt == 0) {
        return 0;
    }
    
    /* Allocate if node */
    if_node = parser_alloc_node(parser, NODE_IF);
    if (if_node == 0) {
        return 0;
    }
    
    /* Parse optional else */
    else_stmt = 0;
    if (parser_consume(parser, TOK_ELSE)) {
        else_stmt = parse_statement(parser);
        if (else_stmt == 0) {
            return 0;
        }
    }
    
    /* Fill if node */
    parser->nodes[if_node - parser->total_nodes - 1].data.if_stmt.condition = condition;
    parser->nodes[if_node - parser->total_nodes - 1].data.if_stmt.then_stmt = then_stmt;
    parser->nodes[if_node - parser->total_nodes - 1].data.if_stmt.else_stmt = else_stmt;
    
    return if_node;
}

/**
 * Parse while statement
 * WhileStmt -> 'while' '(' Expr ')' Statement
 */
static uint16_t parse_while_stmt(Parser* parser) {
    uint16_t while_node;
    uint16_t condition;
    uint16_t body;
    
    /* Expect 'while' */
    if (parser_expect(parser, TOK_WHILE) < 0) {
        return 0;
    }
    
    /* Expect '(' */
    if (parser_expect(parser, TOK_LPAREN) < 0) {
        return 0;
    }
    
    /* Parse condition */
    condition = parse_expression(parser);
    if (condition == 0) {
        return 0;
    }
    
    /* Expect ')' */
    if (parser_expect(parser, TOK_RPAREN) < 0) {
        return 0;
    }
    
    /* Parse body */
    body = parse_statement(parser);
    if (body == 0) {
        return 0;
    }
    
    /* Allocate while node */
    while_node = parser_alloc_node(parser, NODE_WHILE);
    if (while_node == 0) {
        return 0;
    }
    
    /* Fill while node */
    parser->nodes[while_node - parser->total_nodes - 1].data.while_stmt.condition = condition;
    parser->nodes[while_node - parser->total_nodes - 1].data.while_stmt.body = body;
    
    return while_node;
}

/**
 * Parse for statement
 * ForStmt -> 'for' '(' (Expr | VarDecl)? ';' Expr? ';' Expr? ')' Statement
 */
uint16_t parse_for(Parser* parser) {
    uint16_t for_node;
    uint16_t init;
    uint16_t condition;
    uint16_t update;
    uint16_t body;
    
    /* Expect 'for' */
    if (parser_expect(parser, TOK_FOR) < 0) {
        return 0;
    }
    
    /* Expect '(' */
    if (parser_expect(parser, TOK_LPAREN) < 0) {
        return 0;
    }
    
    /* Parse init (can be expression or empty) */
    init = 0;
    if (!parser_match(parser, TOK_SEMICOLON)) {
        /* Parse init expression */
        init = parse_expression(parser);
        if (init == 0) {
            return 0;
        }
        
        /* Expect ';' after init */
        if (parser_expect(parser, TOK_SEMICOLON) < 0) {
            return 0;
        }
    } else {
        /* Empty init, consume ';' */
        parser_next_token(parser);
    }
    
    /* Parse condition (can be empty) */
    condition = 0;
    if (!parser_match(parser, TOK_SEMICOLON)) {
        condition = parse_expression(parser);
        if (condition == 0) {
            return 0;
        }
        
        /* Expect ';' after condition */
        if (parser_expect(parser, TOK_SEMICOLON) < 0) {
            return 0;
        }
    } else {
        /* Empty condition, consume ';' */
        parser_next_token(parser);
    }
    
    /* Parse update (can be empty) */
    update = 0;
    if (!parser_match(parser, TOK_RPAREN)) {
        update = parse_expression(parser);
        if (update == 0) {
            return 0;
        }
    }
    
    /* Expect ')' */
    if (parser_expect(parser, TOK_RPAREN) < 0) {
        return 0;
    }
    
    /* Parse body */
    body = parse_statement(parser);
    if (body == 0) {
        return 0;
    }
    
    /* Allocate for node */
    for_node = parser_alloc_node(parser, NODE_FOR);
    if (for_node == 0) {
        return 0;
    }
    
    /* Fill for node */
    parser->nodes[for_node - parser->total_nodes - 1].data.for_stmt.init = init;
    parser->nodes[for_node - parser->total_nodes - 1].data.for_stmt.condition = condition;
    parser->nodes[for_node - parser->total_nodes - 1].data.for_stmt.update = update;
    parser->nodes[for_node - parser->total_nodes - 1].data.for_stmt.body = body;
    
    return for_node;
}

/**
 * Parse return statement
 * ReturnStmt -> 'return' Expr? ';'
 */
static uint16_t parse_return_stmt(Parser* parser) {
    uint16_t return_node;
    uint16_t expr;
    
    /* Expect 'return' */
    if (parser_expect(parser, TOK_RETURN) < 0) {
        return 0;
    }
    
    /* Allocate return node */
    return_node = parser_alloc_node(parser, NODE_RETURN);
    if (return_node == 0) {
        return 0;
    }
    
    /* Parse optional expression */
    expr = 0;
    if (!parser_match(parser, TOK_SEMICOLON)) {
        expr = parse_expression(parser);
        if (expr == 0) {
            return 0;
        }
    }
    
    /* Expect ';' */
    if (parser_expect(parser, TOK_SEMICOLON) < 0) {
        return 0;
    }
    
    /* Fill return node */
    parser->nodes[return_node - parser->total_nodes - 1].data.return_stmt.expr = expr;
    
    return return_node;
}

/**
 * Parse expression (simplified for MVP)
 * Expr -> Assignment
 */
uint16_t parse_expression(Parser* parser) {
    return parse_assignment(parser);
}

/**
 * Parse assignment
 * Assignment -> LogicalOr ('=' Assignment)?
 */
uint16_t parse_assignment(Parser* parser) {
    uint16_t left;
    uint16_t assign_node;
    uint16_t right;
    
    left = parse_logical_or(parser);
    if (left == 0) {
        return 0;
    }
    
    if (parser_consume(parser, TOK_ASSIGN)) {
        right = parse_assignment(parser);
        if (right == 0) {
            return 0;
        }
        
        assign_node = parser_alloc_node(parser, NODE_ASSIGN);
        if (assign_node == 0) {
            return 0;
        }
        
        parser->nodes[assign_node - parser->total_nodes - 1].data.assign.target = left;
        parser->nodes[assign_node - parser->total_nodes - 1].data.assign.value = right;
        
        return assign_node;
    }
    
    return left;
}

/**
 * Parse logical OR expression
 * LogicalOr -> LogicalAnd ('||' LogicalAnd)*
 */
uint16_t parse_logical_or(Parser* parser) {
    uint16_t left;
    uint16_t op_node;
    uint16_t right;
    
    left = parse_logical_and(parser);
    if (left == 0) {
        return 0;
    }
    
    while (parser_consume(parser, TOK_OR)) {
        right = parse_logical_and(parser);
        if (right == 0) {
            return 0;
        }
        
        op_node = parser_alloc_node(parser, NODE_BINARY_OP);
        if (op_node == 0) {
            return 0;
        }
        
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.op = BINOP_OR;
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.left = left;
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.right = right;
        
        left = op_node;
    }
    
    return left;
}

/**
 * Parse logical AND expression
 * LogicalAnd -> Equality ('&&' Equality)*
 */
uint16_t parse_logical_and(Parser* parser) {
    uint16_t left;
    uint16_t op_node;
    uint16_t right;
    
    left = parse_equality(parser);
    if (left == 0) {
        return 0;
    }
    
    while (parser_consume(parser, TOK_AND)) {
        right = parse_equality(parser);
        if (right == 0) {
            return 0;
        }
        
        op_node = parser_alloc_node(parser, NODE_BINARY_OP);
        if (op_node == 0) {
            return 0;
        }
        
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.op = BINOP_AND;
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.left = left;
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.right = right;
        
        left = op_node;
    }
    
    return left;
}

/**
 * Parse equality expression
 * Equality -> Relational (('==' | '!=') Relational)*
 */
uint16_t parse_equality(Parser* parser) {
    uint16_t left;
    uint16_t op_node;
    uint16_t right;
    BinaryOp op;
    
    left = parse_relational(parser);
    if (left == 0) {
        return 0;
    }
    
    while (1) {
        if (parser_consume(parser, TOK_EQ)) {
            op = BINOP_EQ;
        } else if (parser_consume(parser, TOK_NE)) {
            op = BINOP_NE;
        } else {
            break;
        }
        
        right = parse_relational(parser);
        if (right == 0) {
            return 0;
        }
        
        op_node = parser_alloc_node(parser, NODE_BINARY_OP);
        if (op_node == 0) {
            return 0;
        }
        
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.op = op;
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.left = left;
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.right = right;
        
        left = op_node;
    }
    
    return left;
}

/**
 * Parse relational expression
 * Relational -> Additive (('<' | '<=' | '>' | '>=') Additive)*
 */
uint16_t parse_relational(Parser* parser) {
    uint16_t left;
    uint16_t op_node;
    uint16_t right;
    BinaryOp op;
    
    left = parse_additive(parser);
    if (left == 0) {
        return 0;
    }
    
    while (1) {
        if (parser_consume(parser, TOK_LT)) {
            op = BINOP_LT;
        } else if (parser_consume(parser, TOK_LE)) {
            op = BINOP_LE;
        } else if (parser_consume(parser, TOK_GT)) {
            op = BINOP_GT;
        } else if (parser_consume(parser, TOK_GE)) {
            op = BINOP_GE;
        } else {
            break;
        }
        
        right = parse_additive(parser);
        if (right == 0) {
            return 0;
        }
        
        op_node = parser_alloc_node(parser, NODE_BINARY_OP);
        if (op_node == 0) {
            return 0;
        }
        
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.op = op;
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.left = left;
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.right = right;
        
        left = op_node;
    }
    
    return left;
}

/**
 * Parse additive expression
 * Additive -> Multiplicative (('+' | '-') Multiplicative)*
 */
uint16_t parse_additive(Parser* parser) {
    uint16_t left;
    uint16_t op_node;
    uint16_t right;
    BinaryOp op;
    
    left = parse_multiplicative(parser);
    if (left == 0) {
        return 0;
    }
    
    while (1) {
        if (parser_consume(parser, TOK_PLUS)) {
            op = BINOP_ADD;
        } else if (parser_consume(parser, TOK_MINUS)) {
            op = BINOP_SUB;
        } else {
            break;
        }
        
        right = parse_multiplicative(parser);
        if (right == 0) {
            return 0;
        }
        
        op_node = parser_alloc_node(parser, NODE_BINARY_OP);
        if (op_node == 0) {
            return 0;
        }
        
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.op = op;
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.left = left;
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.right = right;
        
        left = op_node;
    }
    
    return left;
}

/**
 * Parse multiplicative expression
 * Multiplicative -> Unary (('*' | '/' | '%') Unary)*
 */
uint16_t parse_multiplicative(Parser* parser) {
    uint16_t left;
    uint16_t op_node;
    uint16_t right;
    BinaryOp op;
    
    left = parse_unary(parser);
    if (left == 0) {
        return 0;
    }
    
    while (1) {
        if (parser_consume(parser, TOK_STAR)) {
            op = BINOP_MUL;
        } else if (parser_consume(parser, TOK_SLASH)) {
            op = BINOP_DIV;
        } else if (parser_consume(parser, TOK_PERCENT)) {
            op = BINOP_MOD;
        } else {
            break;
        }
        
        right = parse_unary(parser);
        if (right == 0) {
            return 0;
        }
        
        op_node = parser_alloc_node(parser, NODE_BINARY_OP);
        if (op_node == 0) {
            return 0;
        }
        
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.op = op;
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.left = left;
        parser->nodes[op_node - parser->total_nodes - 1].data.binary_op.right = right;
        
        left = op_node;
    }
    
    return left;
}

/**
 * Parse unary expression
 * Unary -> ('-' | '!') Unary | Postfix
 */
uint16_t parse_unary(Parser* parser) {
    uint16_t op_node;
    uint16_t operand;
    UnaryOp op;
    
    if (parser_consume(parser, TOK_MINUS)) {
        op = UNOP_NEG;
    } else if (parser_consume(parser, TOK_NOT)) {
        op = UNOP_NOT;
    } else {
        return parse_postfix(parser);
    }
    
    operand = parse_unary(parser);
    if (operand == 0) {
        return 0;
    }
    
    op_node = parser_alloc_node(parser, NODE_UNARY_OP);
    if (op_node == 0) {
        return 0;
    }
    
    parser->nodes[op_node - parser->total_nodes - 1].data.unary_op.op = op;
    parser->nodes[op_node - parser->total_nodes - 1].data.unary_op.operand = operand;
    
    return op_node;
}

/**
 * Parse postfix expression
 * Postfix -> Primary suffix*
 */
uint16_t parse_postfix(Parser* parser) {
    uint16_t expr;
    uint16_t call_node;
    uint16_t field_name;
    uint16_t arg_node;
    uint16_t identifier_name;
    int expr_is_identifier;
    
    expr = parse_primary(parser);
    if (expr == 0) {
        return 0;
    }
    
    expr_is_identifier = 0;
    identifier_name = 0;
    if (expr > parser->total_nodes) {
        ASTNode* expr_node = &parser->nodes[expr - parser->total_nodes - 1];
        if (expr_node->type == NODE_IDENTIFIER) {
            expr_is_identifier = 1;
            identifier_name = expr_node->data.identifier.name;
        }
    }
    
    while (1) {
        if (expr_is_identifier && parser_match(parser, TOK_LPAREN)) {
            parser_next_token(parser);  /* consume '(' */
            
            call_node = parser_alloc_node(parser, NODE_CALL);
            if (call_node == 0) {
                return 0;
            }
            
            arg_node = 0;
            if (!parser_match(parser, TOK_RPAREN)) {
                arg_node = parse_expression(parser);
                if (arg_node == 0) {
                    return 0;
                }
            }
            
            if (parser_expect(parser, TOK_RPAREN) < 0) {
                return 0;
            }
            
            parser->nodes[call_node - parser->total_nodes - 1].data.call.object = 0;
            parser->nodes[call_node - parser->total_nodes - 1].data.call.method_name = identifier_name;
            parser->nodes[call_node - parser->total_nodes - 1].data.call.arg_count = (arg_node != 0) ? 1 : 0;
            parser->nodes[call_node - parser->total_nodes - 1].data.call.first_arg = arg_node;
            
            expr = call_node;
            expr_is_identifier = 0;
            identifier_name = 0;
        } else if (parser_consume(parser, TOK_DOT)) {
            if (!parser_match(parser, TOK_IDENTIFIER)) {
                parser_error(parser, "Expected field or method name");
                return 0;
            }
            
            field_name = parser->current.value.str_offset;
            parser_next_token(parser);
            
            if (parser_consume(parser, TOK_LPAREN)) {
                call_node = parser_alloc_node(parser, NODE_CALL);
                if (call_node == 0) {
                    return 0;
                }
                
                arg_node = 0;
                if (!parser_match(parser, TOK_RPAREN)) {
                    arg_node = parse_expression(parser);
                    if (arg_node == 0) {
                        return 0;
                    }
                }
                
                if (parser_expect(parser, TOK_RPAREN) < 0) {
                    return 0;
                }
                
                parser->nodes[call_node - parser->total_nodes - 1].data.call.object = expr;
                parser->nodes[call_node - parser->total_nodes - 1].data.call.method_name = field_name;
                parser->nodes[call_node - parser->total_nodes - 1].data.call.arg_count = (arg_node != 0) ? 1 : 0;
                parser->nodes[call_node - parser->total_nodes - 1].data.call.first_arg = arg_node;
                
                expr = call_node;
            } else {
                uint16_t field_node = parser_alloc_node(parser, NODE_FIELD_ACCESS);
                if (field_node == 0) {
                    return 0;
                }
                
                parser->nodes[field_node - parser->total_nodes - 1].data.field_access.object = expr;
                parser->nodes[field_node - parser->total_nodes - 1].data.field_access.field_name = field_name;
                
                expr = field_node;
            }
            expr_is_identifier = 0;
            identifier_name = 0;
        } else if (parser_consume(parser, TOK_LBRACKET)) {
            uint16_t index_node;
            uint16_t array_node;
            
            /* Parse index expression.
             * Array indices need additive arithmetic like:
             *   arr[i]
             *   arr[j + 1]
             *   arr[n - i - 1]
             * but must stop before outer relational operators in expressions like:
             *   arr[j] > arr[j + 1]
             */
            index_node = parse_additive(parser);
            if (index_node == 0) {
                return 0;
            }
            
            if (parser_expect(parser, TOK_RBRACKET) < 0) {
                return 0;
            }
            
            array_node = parser_alloc_node(parser, NODE_ARRAY_ACCESS);
            if (array_node == 0) {
                return 0;
            }
            
            parser->nodes[array_node - parser->total_nodes - 1].data.array_access.array = expr;
            parser->nodes[array_node - parser->total_nodes - 1].data.array_access.index = index_node;
            
            expr = array_node;
            expr_is_identifier = 0;
            identifier_name = 0;
        } else {
            break;
        }
    }
    
    if (parser_match(parser, TOK_PLUSPLUS) || parser_match(parser, TOK_MINUSMINUS)) {
        uint16_t postfix_node;
        TokenType op_type = parser->current.type;
        
        parser_next_token(parser);
        
        postfix_node = parser_alloc_node(parser, NODE_POSTFIX_OP);
        expr_is_identifier = 0;
        identifier_name = 0;
        if (postfix_node == 0) {
            return 0;
        }
        
        parser->nodes[postfix_node - parser->total_nodes - 1].data.postfix_op.op =
            (op_type == TOK_PLUSPLUS) ? POSTOP_INC : POSTOP_DEC;
        parser->nodes[postfix_node - parser->total_nodes - 1].data.postfix_op.operand = expr;
        
        expr = postfix_node;
    }
    
    return expr;
}

/**
 * Parse primary expression
 * Primary -> INTEGER | ID | 'new' ('int'|'boolean') '[' Expr ']' | '(' Expr ')'
 */
uint16_t parse_primary(Parser* parser) {
    uint16_t node;
    
    /* Integer literal */
    if (parser_match(parser, TOK_INTEGER)) {
        node = parser_alloc_node(parser, NODE_LITERAL_INT);
        if (node == 0) {
            return 0;
        }
        
        parser->nodes[node - parser->total_nodes - 1].data.literal_int.int_value = parser->current.value.int_value;
        parser_next_token(parser);
        
        return node;
    }
    
    /* String literal */
    if (parser_match(parser, TOK_STRING)) {
        node = parser_alloc_node(parser, NODE_LITERAL_STRING);
        if (node == 0) {
            return 0;
        }
        
        parser->nodes[node - parser->total_nodes - 1].data.literal_string.str_offset = parser->current.value.str_offset;
        parser_next_token(parser);
        
        return node;
    }
    
    /* Array creation */
    if (parser_consume(parser, TOK_NEW)) {
        uint16_t base_kind;
        uint16_t size_expr;
        
        if (parser_consume(parser, TOK_INT)) {
            base_kind = TYPE_INT;
        } else if (parser_consume(parser, TOK_BOOLEAN)) {
            base_kind = TYPE_BOOLEAN;
        } else {
            parser_error(parser, "Expected array element type after new");
            return 0;
        }
        
        if (parser_expect(parser, TOK_LBRACKET) < 0) {
            return 0;
        }
        
        size_expr = parse_expression(parser);
        if (size_expr == 0) {
            return 0;
        }
        
        if (parser_expect(parser, TOK_RBRACKET) < 0) {
            return 0;
        }
        
        node = parser_alloc_node(parser, NODE_NEW);
        if (node == 0) {
            return 0;
        }
        
        parser->nodes[node - parser->total_nodes - 1].data.new_expr.class_name = base_kind;
        parser->nodes[node - parser->total_nodes - 1].next_sibling = size_expr;
        
        return node;
    }
    
    /* Identifier */
    if (parser_match(parser, TOK_IDENTIFIER)) {
        node = parser_alloc_node(parser, NODE_IDENTIFIER);
        if (node == 0) {
            return 0;
        }
        
        parser->nodes[node - parser->total_nodes - 1].data.identifier.name = parser->current.value.str_offset;
        parser_next_token(parser);
        
        return node;
    }
    
    /* Parenthesized expression */
    if (parser_consume(parser, TOK_LPAREN)) {
        node = parse_expression(parser);
        if (node == 0) {
            return 0;
        }
        
        if (parser_expect(parser, TOK_RPAREN) < 0) {
            return 0;
        }
        
        return node;
    }
    
    parser_error(parser, "Expected expression");
    return 0;
}

/**
 * Report parse error
 */
void parser_error(Parser* parser, const char* message) {
    fprintf(stderr, "Parse error at line %d, column %d: %s\n",
            parser->current.line, parser->current.column, message);
    parser->has_error = 1;
    parser->error_count++;
}

/**
 * Cleanup parser
 */
void parser_cleanup(Parser* parser) {
    if (parser->tokens) {
        fclose(parser->tokens);
        parser->tokens = NULL;
    }
    if (parser->ast_file) {
        fclose(parser->ast_file);
        parser->ast_file = NULL;
    }
}

/* Debug helper functions */

const char* node_type_name(NodeType type) {
    switch (type) {
        case NODE_PROGRAM: return "PROGRAM";
        case NODE_CLASS: return "CLASS";
        case NODE_METHOD: return "METHOD";
        case NODE_FIELD: return "FIELD";
        case NODE_PARAM: return "PARAM";
        case NODE_BLOCK: return "BLOCK";
        case NODE_VAR_DECL: return "VAR_DECL";
        case NODE_IF: return "IF";
        case NODE_WHILE: return "WHILE";
        case NODE_FOR: return "FOR";
        case NODE_RETURN: return "RETURN";
        case NODE_EXPR_STMT: return "EXPR_STMT";
        case NODE_ASSIGN: return "ASSIGN";
        case NODE_BINARY_OP: return "BINARY_OP";
        case NODE_UNARY_OP: return "UNARY_OP";
        case NODE_CALL: return "CALL";
        case NODE_NEW: return "NEW";
        case NODE_FIELD_ACCESS: return "FIELD_ACCESS";
        case NODE_ARRAY_ACCESS: return "ARRAY_ACCESS";
        case NODE_LITERAL_INT: return "LITERAL_INT";
        case NODE_LITERAL_BOOL: return "LITERAL_BOOL";
        case NODE_LITERAL_STRING: return "LITERAL_STRING";
        case NODE_IDENTIFIER: return "IDENTIFIER";
        case NODE_THIS: return "THIS";
        default: return "UNKNOWN";
    }
}

const char* binary_op_name(BinaryOp op) {
    switch (op) {
        case BINOP_ADD: return "+";
        case BINOP_SUB: return "-";
        case BINOP_MUL: return "*";
        case BINOP_DIV: return "/";
        case BINOP_MOD: return "%";
        case BINOP_EQ: return "==";
        case BINOP_NE: return "!=";
        case BINOP_LT: return "<";
        case BINOP_LE: return "<=";
        case BINOP_GT: return ">";
        case BINOP_GE: return ">=";
        case BINOP_AND: return "&&";
        case BINOP_OR: return "||";
        default: return "?";
    }
}

const char* unary_op_name(UnaryOp op) {
    switch (op) {
        case UNOP_NEG: return "-";
        case UNOP_NOT: return "!";
        default: return "?";
    }
}

const char* type_kind_name(TypeKind kind) {
    switch (kind) {
        case TYPE_VOID: return "void";
        case TYPE_INT: return "int";
        case TYPE_BOOLEAN: return "boolean";
        case TYPE_CLASS: return "class";
        default: return "unknown";
    }
}


