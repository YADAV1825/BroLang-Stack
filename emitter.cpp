// =======================================================================================
// Made by: Rohit Yadav
// NIT Jalandhar
//
// File: emitter.cpp
// Purpose:
//   - Converts compiled bytecode (Instruction vector) into a C++ file (`prog.cpp`)
//   - This emitted file can be compiled and linked with your VM to simulate execution.
//
// Core Idea:
//   - Acts like a backend code emitter that serializes your custom instructions
//   - Converts IR (Instruction representation) into a runnable C++ source file
// =======================================================================================

#include "emitter.h"
#include <fstream>
#include <iostream>
#include <sstream>

// =======================================================================================
// Function: writeToFile
// Description:
//   - Takes a list of compiled Instructions and writes them to a `.cpp` file.
//   - The emitted file includes: `#include "RohitVM.hpp"` and a global vector `prog`
// Parameters:
//   - filename: output file path (usually "prog.cpp")
//   - instructions: compiled bytecode to emit
// Returns:
//   - true if successfully written, false otherwise
// =======================================================================================
bool Emitter::writeToFile(const std::string& filename, const std::vector<Instruction>& instructions) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Failed to open output file: " << filename << "\n";
        return false;
    }

    // --- Emit header and global vector declaration ---
    out << "#include \"RohitVM.hpp\"\n";
    out << "std::vector<Instruction> prog = {\n";

    // --- Emit each instruction ---
    for (const auto& instr : instructions) {
        out << "    {Opcode::";

        // Convert Opcode enum to string manually
        switch (instr.op) {
            case Opcode::NOP:     out << "NOP"; break;
            case Opcode::HLT:     out << "HLT"; break;
            case Opcode::MOV:     out << "MOV"; break;
            case Opcode::MOV_BX:  out << "MOV_BX"; break;
            case Opcode::MOV_CX:  out << "MOV_CX"; break;
            case Opcode::MOV_DX:  out << "MOV_DX"; break;
            case Opcode::MOV_SP:  out << "MOV_SP"; break;
            case Opcode::ADD:     out << "ADD"; break;
            case Opcode::SUB:     out << "SUB"; break;
            case Opcode::MUL:     out << "MUL"; break;
            case Opcode::DIV:     out << "DIV"; break;
            case Opcode::PUSH:    out << "PUSH"; break;
            case Opcode::POP:     out << "POP"; break;
            case Opcode::STE:     out << "STE"; break;
            case Opcode::CLE:     out << "CLE"; break;
            case Opcode::STG:     out << "STG"; break;
            case Opcode::CLG:     out << "CLG"; break;
            case Opcode::STH:     out << "STH"; break;
            case Opcode::CLH:     out << "CLH"; break;
            case Opcode::STL:     out << "STL"; break;
            case Opcode::CLL:     out << "CLL"; break;
            case Opcode::PRN:     out << "PRN"; break;
            default:              out << "NOP"; break; // fallback to prevent failure
        }

        // Emit operand if the instruction expects one (e.g. MOV, PUSH)
        switch (instr.op) {
            case Opcode::MOV:
            case Opcode::MOV_BX:
            case Opcode::MOV_CX:
            case Opcode::MOV_DX:
            case Opcode::MOV_SP:
            case Opcode::PUSH:
            case Opcode::POP:
                out << ", " << instr.a1;
                break;
            default:
                break;
        }

        out << "},\n";
    }

    // --- Close the vector ---
    out << "};\n";
    out.close();

    std::cout << "Wrote program to " << filename << "\n";
    return true;
}
