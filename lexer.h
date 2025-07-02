// =======================================================================================
// Made by: Rohit Yadav
// NIT Jalandhar
//
// File: lexer.h
// Purpose:
//   - This header defines the structure of the Lexer and the Token class used to convert
//     source code into a list of meaningful tokens (lexical units).
//   - It is the first stage of the compilation pipeline (Lexical Analysis).
//   - Tokens produced by this lexer are passed to the parser.
// =======================================================================================

#pragma once

#include "tokens.h"   // Includes token types (keywords, symbols, etc.)
#include <string>
#include <vector>
#include <cctype>     // For isspace, isdigit, isalpha

// ---------------------------------------------------------------------------------------
// Struct: Token
// Represents a lexical token, which has:
//   - `type`: its classification (e.g., Identifier, Number, Plus)
//   - `text`: the actual text from the source (e.g., "letbro", "123", "a")
// ---------------------------------------------------------------------------------------
struct Token {
    TokenType type;
    std::string text;

    Token(TokenType t, const std::string& s) : type(t), text(s) {}
};

// ---------------------------------------------------------------------------------------
// Class: Lexer
// Converts the raw source code string into a sequence of tokens for the parser.
// Performs lexical analysis using FSM-style scanning.
//
// Example:
//   Source Code:  letbro a = 5;
//   Output Tokens: [LetBro, Identifier(a), Assign, Number(5), Semicolon]
// ---------------------------------------------------------------------------------------
class Lexer {
public:
    // Constructor: Takes in full source code as a string
    explicit Lexer(const std::string& source);

    // Returns the next token and advances the internal position
    Token nextToken();

    // Returns the next token but does not consume it (lookahead)
    Token peekToken();

private:
    std::string src;  // The full source code string
    size_t pos = 0;   // Current position in the source string

    // Internal helpers
    char peek() const;               // Peek current character
    char advance();                  // Consume and return current character
    bool match(char expected);       // Match specific character and consume it if true
    void skipWhitespace();           // Skip whitespace (tabs, spaces, newlines)
    Token identifierOrKeyword();     // Handle identifiers and reserved words (e.g., letbro)
    Token number();                  // Handle numeric literals
};
