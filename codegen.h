// =====================================================================================
// Made by: Rohit Yadav
// NIT Jalandhar
//
// File: codegen.h
// Description:
//   - This is the header file for the Codegen class, responsible for transforming
//     parsed AST nodes into RohitVM bytecode instructions.
//
// Purpose:
//   - It connects the high-level language (Brolang) to low-level virtual CPU instructions.
//   - It manages variable symbol tables, jump labels, and emits final instruction streams.
// =====================================================================================

#pragma once

#include "ast.h"           // Abstract Syntax Tree structures
#include "RohitVM.hpp"     // Includes Instruction and Opcode definitions
#include <vector>
#include <map>
#include <string>

// =====================================================================================
// Class: Codegen
// Purpose:
//   - Central class responsible for converting a parsed Program (AST)
//     into executable bytecode for the virtual machine.
// =====================================================================================
class Codegen {
public:
    // Main entry point: Generates VM instructions from a full program
    std::vector<Instruction> generate(const Program& program);

private:
    // Emits a single instruction into the instruction buffer
    void emit(const Instruction& instr);

    // Processes individual high-level statements (let, print, if, while)
    void genStatement(const StmtPtr& stmt);

    // Processes high-level expressions (arithmetic, comparison, variables, constants)
    void genExpression(const ExprPtr& expr);

    // ================= Label + Control Flow Utilities =================

    // Creates a new unique label ID
    int newLabel();

    // Marks a specific position in the instruction stream as a label target
    void markLabel(int labelId);

    // Emits a jump with a placeholder (to be patched after final positions are known)
    void emitJumpPlaceholder(Opcode jumpOpcode, int labelId);

    // Fills in the correct jump targets after code generation is done
    void patchJumps();

    // ================= Internal State =================

    std::vector<Instruction> instructions;              // Final output instruction list
    std::map<std::string, uint16_t> symbolTable;        // Tracks variables to registers
    std::map<int, size_t> labelTargets;                 // Label ID → instruction index
    std::vector<std::pair<size_t, int>> labelPlaceholders; // Unresolved jumps

    int nextRegister = 1;   // Used to allocate registers to new variables
    int labelCounter = 0;   // Used to create unique label IDs
};
