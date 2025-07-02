// =======================================================================================
// Made by: Rohit Yadav
// NIT Jalandhar
//
// File: parser.cpp
// Purpose:
//   - Implements the Parser, which turns tokens into an Abstract Syntax Tree (AST).
//   - Follows Recursive Descent Parsing with basic precedence handling for expressions.
//   - Supports: variable declarations, arithmetic, print statements, conditionals, loops.
// =======================================================================================

#include "parser.h"
#include <stdexcept>
#include <iostream>

// =======================================================================================
// SECTION: Token Navigation Utilities
// =======================================================================================

// Constructor: Takes ownership of the token stream from the Lexer
Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

// Peek at the current token without consuming it
Token Parser::peek() const {
    return pos < tokens.size() ? tokens[pos] : Token(TokenType::EndOfFile, "");
}

// Consume and return the current token
Token Parser::advance() {
    return isAtEnd() ? peek() : tokens[pos++];
}

// Check if we've reached the end of the token stream
bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

// Try to match a specific token type and advance if matched
bool Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

// Like match(), but prints error if token not found
bool Parser::expect(TokenType type, const std::string& msgIfFail) {
    if (!match(type)) {
        std::cerr << "Parse error: " << msgIfFail << "\n";
        return false;
    }
    return true;
}

// =======================================================================================
// SECTION: Program Parser (Top-level)
// =======================================================================================

// Parses a full program = list of statements until EOF
Program Parser::parseProgram() {
    Program prog;
    while (!isAtEnd()) {
        StmtPtr stmt = parseStatement();
        if (stmt) prog.statements.push_back(stmt);
    }
    return prog;
}

// =======================================================================================
// SECTION: Statement Parsers
// =======================================================================================

// Dispatch based on keyword: letbro, printbro, ifbro, whilebro
StmtPtr Parser::parseStatement() {
    if (match(TokenType::LetBro))    return parseLet();
    if (match(TokenType::PrintBro))  return parsePrint();
    if (match(TokenType::IfBro))     return parseIf();
    if (match(TokenType::WhileBro))  return parseWhile();

    std::cerr << "Unexpected token: " << peek().text << "\n";
    advance(); // Skip bad token
    return nullptr;
}

// letbro a = 5;
StmtPtr Parser::parseLet() {
    if (peek().type != TokenType::Identifier) {
        std::cerr << "Expected variable name after letbro\n";
        return nullptr;
    }

    std::string name = advance().text;

    if (!expect(TokenType::Assign, "Expected '=' after variable name")) return nullptr;
    ExprPtr value = parseExpression();
    if (!expect(TokenType::Semicolon, "Expected ';' after expression")) return nullptr;

    return std::make_shared<LetStatement>(name, value);
}

// printbro(a + b);
StmtPtr Parser::parsePrint() {
    if (!expect(TokenType::LParen, "Expected '(' after printbro")) return nullptr;
    ExprPtr expr = parseExpression();
    if (!expect(TokenType::RParen, "Expected ')' after expression")) return nullptr;
    if (!expect(TokenType::Semicolon, "Expected ';' after printbro")) return nullptr;
    return std::make_shared<PrintStatement>(expr);
}

// ifbro (condition) { ... } elsebro { ... }
StmtPtr Parser::parseIf() {
    if (!expect(TokenType::LParen, "Expected '(' after ifbro")) return nullptr;
    ExprPtr condition = parseExpression();
    if (!expect(TokenType::RParen, "Expected ')' after condition")) return nullptr;
    if (!expect(TokenType::LBrace, "Expected '{' after ifbro")) return nullptr;

    auto thenBlock = parseBlock();

    std::vector<StmtPtr> elseBlock;
    if (match(TokenType::ElseBro)) {
        if (!expect(TokenType::LBrace, "Expected '{' after elsebro")) return nullptr;
        elseBlock = parseBlock();
    }

    return std::make_shared<IfStatement>(condition, thenBlock, elseBlock);
}

// whilebro (condition) { ... }
StmtPtr Parser::parseWhile() {
    if (!expect(TokenType::LParen, "Expected '(' after whilebro")) return nullptr;
    ExprPtr condition = parseExpression();
    if (!expect(TokenType::RParen, "Expected ')' after condition")) return nullptr;
    if (!expect(TokenType::LBrace, "Expected '{' to begin whilebro block")) return nullptr;

    auto body = parseBlock();
    return std::make_shared<WhileStatement>(condition, body);
}

// { stmt1; stmt2; }
std::vector<StmtPtr> Parser::parseBlock() {
    std::vector<StmtPtr> stmts;
    while (!isAtEnd() && !match(TokenType::RBrace)) {
        auto stmt = parseStatement();
        if (stmt) stmts.push_back(stmt);
    }
    return stmts;
}

// =======================================================================================
// SECTION: Expression Parsing — handles precedence (==, >, <, +, *, etc.)
// =======================================================================================

// Start of expression parsing (top-level call)
ExprPtr Parser::parseExpression() {
    return parseEquality(); // Root: check for ==
}

// Handles: a == b
ExprPtr Parser::parseEquality() {
    auto expr = parseComparison();

    while (match(TokenType::Equal)) {
        auto right = parseComparison();
        expr = std::make_shared<BinaryExpr>(BinaryOp::Equal, expr, right);
    }

    return expr;
}

// Handles: a > b or a < b
ExprPtr Parser::parseComparison() {
    auto expr = parseTerm();

    while (true) {
        if (match(TokenType::Greater)) {
            auto right = parseTerm();
            expr = std::make_shared<BinaryExpr>(BinaryOp::Greater, expr, right);
        } else if (match(TokenType::Less)) {
            auto right = parseTerm();
            expr = std::make_shared<BinaryExpr>(BinaryOp::Less, expr, right);
        } else {
            break;
        }
    }

    return expr;
}

// Handles: a + b or a - b
ExprPtr Parser::parseTerm() {
    auto expr = parseFactor();

    while (true) {
        if (match(TokenType::Plus)) {
            auto right = parseFactor();
            expr = std::make_shared<BinaryExpr>(BinaryOp::Add, expr, right);
        } else if (match(TokenType::Minus)) {
            auto right = parseFactor();
            expr = std::make_shared<BinaryExpr>(BinaryOp::Sub, expr, right);
        } else {
            break;
        }
    }

    return expr;
}

// Handles: a * b or a / b
ExprPtr Parser::parseFactor() {
    auto expr = parsePrimary();

    while (true) {
        if (match(TokenType::Star)) {
            auto right = parsePrimary();
            expr = std::make_shared<BinaryExpr>(BinaryOp::Mul, expr, right);
        } else if (match(TokenType::Slash)) {
            auto right = parsePrimary();
            expr = std::make_shared<BinaryExpr>(BinaryOp::Div, expr, right);
        } else {
            break;
        }
    }

    return expr;
}

// Handles literals, identifiers, and parenthesis
ExprPtr Parser::parsePrimary() {
    if (match(TokenType::Number)) {
        return std::make_shared<NumberExpr>(std::stoi(tokens[pos - 1].text));
    }

    if (match(TokenType::Identifier)) {
        return std::make_shared<VariableExpr>(tokens[pos - 1].text);
    }

    if (match(TokenType::LParen)) {
        auto expr = parseExpression();
        expect(TokenType::RParen, "Expected ')' after expression");
        return expr;
    }

    std::cerr << "Unexpected token in expression: " << peek().text << "\n";
    advance();
    return nullptr;
}
