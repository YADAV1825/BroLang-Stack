#pragma once

#include <string>
#include <unordered_map>

// ===================================================================
// TokenType:
// Enum representing all possible tokens in the BroLang language.
// ===================================================================
enum class TokenType {
    // Special Tokens
    EndOfFile,     // End of input
    Invalid,       // Unrecognized character

    // Keywords
    LetBro,        // letbro
    IfBro,         // ifbro
    ElseBro,       // elsebro
    WhileBro,      // whilebro
    PrintBro,      // printbro

    // Identifiers & Literals
    Identifier,    // Variable names
    Number,        // Numeric literals

    // Operators
    Plus,          // +
    Minus,         // -
    Star,          // *
    Slash,         // /
    Assign,        // =
    Equal,         // ==
    Greater,       // >
    Less,          // <

    // Symbols / Punctuation
    Semicolon,     // ;
    LParen,        // (
    RParen,        // )
    LBrace,        // {
    RBrace         // }
};

// ===================================================================
// Utility: Convert a TokenType to its string representation
// ===================================================================
inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::EndOfFile:   return "EOF";
        case TokenType::Invalid:     return "Invalid";

        // Keywords
        case TokenType::LetBro:      return "letbro";
        case TokenType::IfBro:       return "ifbro";
        case TokenType::ElseBro:     return "elsebro";
        case TokenType::WhileBro:    return "whilebro";
        case TokenType::PrintBro:    return "printbro";

        // Identifiers & Literals
        case TokenType::Identifier:  return "Identifier";
        case TokenType::Number:      return "Number";

        // Operators
        case TokenType::Plus:        return "+";
        case TokenType::Minus:       return "-";
        case TokenType::Star:        return "*";
        case TokenType::Slash:       return "/";
        case TokenType::Assign:      return "=";
        case TokenType::Equal:       return "==";
        case TokenType::Greater:     return ">";
        case TokenType::Less:        return "<";

        // Symbols
        case TokenType::Semicolon:   return ";";
        case TokenType::LParen:      return "(";
        case TokenType::RParen:      return ")";
        case TokenType::LBrace:      return "{";
        case TokenType::RBrace:      return "}";

        default:                     return "Unknown";
    }
}
