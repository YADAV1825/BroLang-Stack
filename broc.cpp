// ===================================================================================
// Made by: Rohit Yadav
// NIT Jalandhar
//
// File: broc.cpp
// Purpose:
//   - This is the main driver for the **Brolang compiler**.
//   - It reads a `.bro` source file, tokenizes it, parses it into an AST,
//     generates bytecode, and finally emits that bytecode as a C++ file
//     runnable on the RohitVM.
// Phases:
//   1. Read source code
//   2. Lexical analysis (tokenization)
//   3. Parsing (build AST)
//   4. Code Generation (convert AST → VM instructions)
//   5. Emission (write the instructions to a .cpp file)
// ===================================================================================

#include "lexer.h"     // Lexical analysis (tokens)
#include "parser.h"    // Parsing to AST
#include "codegen.h"   // AST to VM instruction generation
#include "emitter.h"   // Writes VM instructions to C++ output

#include <iostream>
#include <fstream>
#include <sstream>

// -----------------------------------------------------------------------------------
// Function: showUsage
// Purpose: Displays correct command-line usage format when arguments are wrong.
// -----------------------------------------------------------------------------------
void showUsage() {
    std::cout << "Usage:\n";
    std::cout << "  ./broc input.bro -o output/prog.cpp\n";
}

// -----------------------------------------------------------------------------------
// Function: readFile
// Purpose: Loads the full source code from the provided `.bro` file path.
// Why:
//   - Needed to feed the lexer with the entire source as a string.
// -----------------------------------------------------------------------------------
std::string readFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("Failed to open input file: " + path);
    }
    std::ostringstream ss;
    ss << in.rdbuf();  // Read entire file into stringstream
    return ss.str();
}

// -----------------------------------------------------------------------------------
// Function: main
// Purpose: Entry point of the Brolang compiler.
// Steps:
//   1. Validate command-line args
//   2. Read the source file
//   3. Lexing → Parsing → Codegen → Emission
// -----------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    // ------------------ Step 0: Validate command-line args ------------------
    if (argc != 4 || std::string(argv[2]) != "-o") {
        showUsage();
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[3];

    try {
        // ------------------ Step 1: Read Source Code ------------------
        std::string source = readFile(inputFile);

        // ------------------ Step 2: Lexical Analysis ------------------
        Lexer lexer(source);
        std::vector<Token> tokens;
        while (true) {
            Token t = lexer.nextToken();
            if (t.type == TokenType::EndOfFile) break;
            tokens.push_back(t);
        }

        // ------------------ Step 3: Parse Tokens into AST ------------------
        Parser parser(tokens);
        Program program = parser.parseProgram();

        // ------------------ Step 4: Generate VM Instructions ------------------
        Codegen codegen;
        std::vector<Instruction> bytecode = codegen.generate(program);

        // ------------------ Step 5: Emit to C++ Source File ------------------
        if (!Emitter::writeToFile(outputFile, bytecode)) {
            return 1; // Failed to write
        }

        std::cout << "✅ Compilation complete.\n";
        return 0;

    } catch (const std::exception& ex) {
        std::cerr << "Compiler error: " << ex.what() << "\n";
        return 1;
    }
}
