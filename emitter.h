// =======================================================================================
// Made by: Rohit Yadav
// NIT Jalandhar
//
// File: emitter.h
// Purpose:
//   - Declaration for the Emitter class responsible for writing compiled bytecode
//     (i.e., vector of `Instruction`) into a valid C++ source file (like `prog.cpp`).
//
//   - This output can be directly compiled and run using your virtual machine (RohitVM).
// =======================================================================================

#pragma once

#include "RohitVM.hpp"  // Includes definitions for `Instruction` and `Opcode`
#include <vector>
#include <string>

// =======================================================================================
// Class: Emitter
// Role:
//   - Static utility class with a single job: write a list of Instructions to a .cpp file.
//
// Usage:
//   Emitter::writeToFile("prog.cpp", instructions);
// =======================================================================================
class Emitter {
public:
    // -----------------------------------------------------------------------------------
    // Function: writeToFile
    // Description:
    //   - Serializes the list of compiled Instructions into a C++ source file
    //   - Each instruction is emitted in a readable, compilable format
    // Parameters:
    //   - filename: destination C++ file path (e.g., "prog.cpp")
    //   - instructions: list of VM bytecode instructions to emit
    // Returns:
    //   - true on success, false on file open failure
    // -----------------------------------------------------------------------------------
    static bool writeToFile(const std::string& filename, const std::vector<Instruction>& instructions);
};
