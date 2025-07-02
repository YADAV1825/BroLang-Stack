// =======================================================================================
// Made by: Rohit Yadav
// NIT Jalandhar
//
// File: parser.h
// Purpose:
//   - Defines the Parser class that transforms a list of tokens into an Abstract Syntax Tree (AST).
//   - Serves as the second major phase of your compiler (right after Lexing).
//   - Supports control flow, arithmetic expressions, and your custom 'bro' syntax.
// =======================================================================================

#pragma once

#include "lexer.h"
#include "ast.h"

// =======================================================================================
// CLASS: Parser
// Role:
//   - Consumes a vector of tokens and produces a structured AST (`Program`).
//   - Uses recursive descent parsing with precedence climbing for expressions.
// =======================================================================================
class Parser {
public:
    // Constructor: Initializes parser with a token list (from the Lexer)
    explicit Parser(std::vector<Token> tokens);

    // Entry point: Parse entire program (multiple statements)
    Program parseProgram();


private:
    std::vector<Token> tokens;  // Token stream to parse
    size_t pos = 0;             // Current token index

    // -----------------------------------------------------------------------------------
    // Token Navigation Helpers
    // -----------------------------------------------------------------------------------

    // Look at current token without consuming
    Token peek() const;

    // Consume current token and return it
    Token advance();

    // Check if current token matches expected type; advance if so
    bool match(TokenType type);

    // Same as match() but prints error if not matched
    bool expect(TokenType type, const std::string& msgIfFail);

    // Check if we've reached end of token stream
    bool isAtEnd() const;

    // -----------------------------------------------------------------------------------
    // Statement Parsers
    // -----------------------------------------------------------------------------------

    // General statement dispatcher
    StmtPtr parseStatement();

    // Parses: letbro <identifier> = <expr>;
    StmtPtr parseLet();

    // Parses: printbro(<expr>);
    StmtPtr parsePrint();

    // Parses: ifbro (...) { ... } elsebro { ... }
    StmtPtr parseIf();

    // Parses: whilebro (...) { ... }
    StmtPtr parseWhile();

    // Parses block enclosed in { ... }
    std::vector<StmtPtr> parseBlock();

    // -----------------------------------------------------------------------------------
    // Expression Parsers (with operator precedence)
    // -----------------------------------------------------------------------------------

    // Entry point for expression parsing
    ExprPtr parseExpression();

    // Handles == operator
    ExprPtr parseEquality();

    // Handles >, < operators
    ExprPtr parseComparison();

    // Handles +, - operators
    ExprPtr parseTerm();

    // Handles *, / operators
    ExprPtr parseFactor();

    // Handles literals, identifiers, and parenthesis
    ExprPtr parsePrimary();
};
