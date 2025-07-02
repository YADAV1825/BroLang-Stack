// =======================================================================================
// Made by: Rohit Yadav
// NIT Jalandhar
//
// File: lexer.cpp
// Purpose:
//   - This is the implementation of the lexical analyzer (lexer) for your custom
//     programming language (BroLang).
//   - The lexer scans the raw source code and produces a stream of tokens
//     which are then used by the parser.
//
//   - It handles:
//       → Identifiers and keywords (e.g., letbro, ifbro, whilebro)
//       → Numbers
//       → Operators and punctuation
//       → Whitespace skipping and error handling for invalid characters
// =======================================================================================

#include "lexer.h"
#include <unordered_map>

// Constructor: Initialize lexer with source code
Lexer::Lexer(const std::string& source) : src(source) {}

// ---------------------------------------------------------------------------------------
// peek()
// Returns the current character from the source without consuming it.
// If at end of source, returns null character '\0'.
// ---------------------------------------------------------------------------------------
char Lexer::peek() const {
    return pos < src.size() ? src[pos] : '\0';
}

// ---------------------------------------------------------------------------------------
// advance()
// Moves forward by one character in the source and returns it.
// If already at end, returns '\0'.
// ---------------------------------------------------------------------------------------
char Lexer::advance() {
    return pos < src.size() ? src[pos++] : '\0';
}

// ---------------------------------------------------------------------------------------
// match(expected)
// If the current character matches the expected char, consumes it and returns true.
// Otherwise returns false.
// Used for detecting double-char operators like '=='.
// ---------------------------------------------------------------------------------------
bool Lexer::match(char expected) {
    if (peek() == expected) {
        pos++;
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------------------
// skipWhitespace()
// Skips over any spaces, tabs, or newlines in the source.
// ---------------------------------------------------------------------------------------
void Lexer::skipWhitespace() {
    while (std::isspace(peek())) advance();
}

// ---------------------------------------------------------------------------------------
// nextToken()
// Main driver function: Scans the source code and returns the next token.
// Recognizes operators, symbols, numbers, identifiers, and keywords.
// Falls back to Invalid token if unknown character is found.
// ---------------------------------------------------------------------------------------
Token Lexer::nextToken() {
    skipWhitespace();

    char c = peek();
    if (c == '\0') return Token(TokenType::EndOfFile, "");

    // Recognize one-character symbols
    if (c == '+') { advance(); return Token(TokenType::Plus, "+"); }
    if (c == '-') { advance(); return Token(TokenType::Minus, "-"); }
    if (c == '*') { advance(); return Token(TokenType::Star, "*"); }
    if (c == '/') { advance(); return Token(TokenType::Slash, "/"); }
    if (c == ';') { advance(); return Token(TokenType::Semicolon, ";"); }
    if (c == '(') { advance(); return Token(TokenType::LParen, "("); }
    if (c == ')') { advance(); return Token(TokenType::RParen, ")"); }
    if (c == '{') { advance(); return Token(TokenType::LBrace, "{"); }
    if (c == '}') { advance(); return Token(TokenType::RBrace, "}"); }

    // Handle assignment '=' or comparison '=='
    if (c == '=') {
        advance();
        if (match('=')) return Token(TokenType::Equal, "==");
        return Token(TokenType::Assign, "=");
    }

    // Comparison operators
    if (c == '>') { advance(); return Token(TokenType::Greater, ">"); }
    if (c == '<') { advance(); return Token(TokenType::Less, "<"); }

    // Number literals
    if (std::isdigit(c)) return number();

    // Identifiers and keywords
    if (std::isalpha(c)) return identifierOrKeyword();

    // Unrecognized character
    advance();
    return Token(TokenType::Invalid, std::string(1, c));
}

// ---------------------------------------------------------------------------------------
// identifierOrKeyword()
// Parses an alphanumeric sequence (letters/digits).
// If it matches a keyword like "letbro", it returns a keyword token.
// Otherwise, it's treated as a user-defined identifier.
// ---------------------------------------------------------------------------------------
Token Lexer::identifierOrKeyword() {
    size_t start = pos;
    while (std::isalnum(peek())) advance();
    std::string text = src.substr(start, pos - start);

    static std::unordered_map<std::string, TokenType> keywords = {
        {"letbro",    TokenType::LetBro},
        {"ifbro",     TokenType::IfBro},
        {"elsebro",   TokenType::ElseBro},
        {"whilebro",  TokenType::WhileBro},
        {"printbro",  TokenType::PrintBro}
    };

    auto it = keywords.find(text);
    if (it != keywords.end()) return Token(it->second, text);
    return Token(TokenType::Identifier, text);
}

// ---------------------------------------------------------------------------------------
// number()
// Parses a sequence of digits into a numeric token.
// Example: "123" becomes Token(Number, "123").
// ---------------------------------------------------------------------------------------
Token Lexer::number() {
    size_t start = pos;
    while (std::isdigit(peek())) advance();
    return Token(TokenType::Number, src.substr(start, pos - start));
}

// ---------------------------------------------------------------------------------------
// peekToken()
// Optional utility: Allows peeking ahead at the next token without consuming it.
// Used by parser to look ahead if needed.
// ---------------------------------------------------------------------------------------
Token Lexer::peekToken() {
    size_t saved = pos;
    Token t = nextToken();
    pos = saved;
    return t;
}
