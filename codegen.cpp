// =====================================================================================
// Made by: Rohit Yadav
// NIT Jalandhar
//
// File: codegen.cpp
// Description:
//   - Converts high-level AST (Abstract Syntax Tree) into bytecode instructions
//     for the RohitVM.
//   - Handles expressions, variable tracking, conditional logic, loops, and labels.
//
// Why this matters:
//   - This is the heart of the compiler: where your custom language becomes executable.
// =====================================================================================

#include "codegen.h"
#include <iostream>

// =====================================================================================
// Function: generate
// Purpose:
//   - Entry point for code generation.
//   - Walks through the program's statements and emits VM instructions.
// =====================================================================================
std::vector<Instruction> Codegen::generate(const Program& program) {
    instructions.clear();
    symbolTable.clear();
    labelPlaceholders.clear();
    labelTargets.clear();
    nextRegister = 1;
    labelCounter = 0;

    for (const auto& stmt : program.statements) {
        genStatement(stmt);  // Compile each statement into bytecode
    }

    emit({Opcode::HLT});  // Add HALT at end
    patchJumps();         // Resolve all jump labels
    return instructions;
}

// =====================================================================================
// Function: emit
// Purpose: Adds a single instruction to the instruction list.
// =====================================================================================
void Codegen::emit(const Instruction& instr) {
    instructions.push_back(instr);
}

// =====================================================================================
// Label and Jump Utilities
// =====================================================================================

// Returns a new unique label ID
int Codegen::newLabel() {
    return labelCounter++;
}

// Marks current position in instruction list as a label target
void Codegen::markLabel(int labelId) {
    labelTargets[labelId] = instructions.size();
}

// Emits a jump instruction with a placeholder (to be patched later)
void Codegen::emitJumpPlaceholder(Opcode jumpOpcode, int labelId) {
    labelPlaceholders.push_back({instructions.size(), labelId});
    emit({jumpOpcode, 0});  // Placeholder argument
}

// Resolves all label placeholders using actual instruction positions
void Codegen::patchJumps() {
    for (const auto& [index, labelId] : labelPlaceholders) {
        if (labelTargets.count(labelId)) {
            instructions[index].a1 = labelTargets[labelId];
        } else {
            std::cerr << "Error: Unknown label ID " << labelId << "\n";
        }
    }
}

// =====================================================================================
// Function: genStatement
// Purpose:
//   - Translates each high-level language statement into VM instructions.
//   - Supports: variable declarations, printing, conditionals (if/else), and loops.
// =====================================================================================
void Codegen::genStatement(const StmtPtr& stmt) {
    // ---------------- Let Statement ----------------
    if (auto let = std::dynamic_pointer_cast<LetStatement>(stmt)) {
        genExpression(let->value);

        uint16_t reg;
        if (!symbolTable.count(let->name)) {
            reg = nextRegister++;
            symbolTable[let->name] = reg;
        } else {
            reg = symbolTable[let->name];
        }

        emit({Opcode::PUSH, 0});  // Push value in AX
        emit({Opcode::POP, reg}); // Store in variable register
    }

    // ---------------- Print Statement ----------------
    else if (auto print = std::dynamic_pointer_cast<PrintStatement>(stmt)) {
        genExpression(print->expr);  // Evaluate expression
        emit({Opcode::PRN});         // Print result (AX)
    }

    // ---------------- If Statement ----------------
    else if (auto ifs = std::dynamic_pointer_cast<IfStatement>(stmt)) {
        genExpression(ifs->condition);
        emit({Opcode::PUSH, 0});
        emit({Opcode::POP, 1});
        emit({Opcode::MOV, 0});
        emit({Opcode::SUB});
        emit({Opcode::STE});
        emit({Opcode::CLE});

        int elseLabel = newLabel();
        int endLabel = newLabel();

        emitJumpPlaceholder(Opcode::JZ, elseLabel);  // If AX == 0 → jump to else

        for (const auto& s : ifs->thenBranch)
            genStatement(s);

        emitJumpPlaceholder(Opcode::JMP, endLabel);  // Jump over else block

        markLabel(elseLabel);
        for (const auto& s : ifs->elseBranch)
            genStatement(s);

        markLabel(endLabel);
    }

    // ---------------- While Statement ----------------
    else if (auto wh = std::dynamic_pointer_cast<WhileStatement>(stmt)) {
        int condLabel = newLabel();
        int endLabel = newLabel();

        markLabel(condLabel);  // Loop start

        genExpression(wh->condition);
        emit({Opcode::PUSH, 0});
        emit({Opcode::POP, 1});
        emit({Opcode::MOV, 0});
        emit({Opcode::SUB});
        emit({Opcode::STE});
        emit({Opcode::CLE});

        emitJumpPlaceholder(Opcode::JZ, endLabel);  // Break loop if false

        for (const auto& s : wh->body)
            genStatement(s);

        emitJumpPlaceholder(Opcode::JMP, condLabel);  // Loop back
        markLabel(endLabel);  // Loop exit
    }
}

// =====================================================================================
// Function: genExpression
// Purpose:
//   - Translates expressions (numbers, variables, and binary operations)
//     into VM instructions.
// =====================================================================================
void Codegen::genExpression(const ExprPtr& expr) {
    // --- Number constant ---
    if (auto num = std::dynamic_pointer_cast<NumberExpr>(expr)) {
        emit({Opcode::MOV, static_cast<uint16_t>(num->value)});  // AX = value
    }

    // --- Variable access ---
    else if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr)) {
        if (!symbolTable.count(var->name)) {
            std::cerr << "Unknown variable: " << var->name << "\n";
            emit({Opcode::MOV, 0});
            return;
        }

        uint16_t reg = symbolTable[var->name];
        emit({Opcode::PUSH, reg});
        emit({Opcode::POP, 0});  // Load variable into AX
    }

    // --- Binary operation ---
    else if (auto bin = std::dynamic_pointer_cast<BinaryExpr>(expr)) {
        genExpression(bin->left);
        emit({Opcode::PUSH, 0});
        genExpression(bin->right);
        emit({Opcode::PUSH, 0});
        emit({Opcode::POP, 1});  // Right operand → BX
        emit({Opcode::POP, 0});  // Left operand  → AX

        switch (bin->op) {
            case BinaryOp::Add:     emit({Opcode::ADD}); break;
            case BinaryOp::Sub:     emit({Opcode::SUB}); break;
            case BinaryOp::Mul:     emit({Opcode::MUL}); break;
            case BinaryOp::Div:     emit({Opcode::DIV}); break;
            case BinaryOp::Equal:   emit({Opcode::SUB}); emit({Opcode::STE}); break;
            case BinaryOp::Greater: emit({Opcode::SUB}); emit({Opcode::STG}); break;
            case BinaryOp::Less:    emit({Opcode::SUB}); emit({Opcode::STL}); break;
            default:
                std::cerr << "Unknown binary operator\n";
                break;
        }
    }
}
