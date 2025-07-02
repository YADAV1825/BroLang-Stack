#pragma once  // Ensures this header is only included once during compilation

#include <cstdint>      // For fixed-width integer types
#include <vector>       // For std::vector
#include <cstdlib>      // For exit()
#include <cstdio>       // For printf()
#include <stdexcept>    // For exceptions
#include "RohitUtils.hpp"

// -----------------------------------------------------------------------------
// Registers and CPU/Memory Definitions
// -----------------------------------------------------------------------------

class Registers {
public:
    uint16_t ax = 0;   // Accumulator
    uint16_t bx = 0;   // Base
    uint16_t cx = 0;   // Counter
    uint16_t dx = 0;   // Data
    uint16_t sp = 0xFFFF; // Stack Pointer
    uint16_t ip = 0x0000; // Instruction Pointer
    uint16_t flags = 0x0000; // Flags register

    enum Flag {
        Equal   = 0x08,
        Greater = 0x04,
        Higher  = 0x02,
        Lower   = 0x01
    };
};

class CPU {
public:
    Registers r;

    bool isEqual()   const { return r.flags & Registers::Equal; }
    bool isGreater() const { return r.flags & Registers::Greater; }
    bool isHigher()  const { return r.flags & Registers::Higher; }
    bool isLower()   const { return r.flags & Registers::Lower; }

    void setEqual(bool v)   { setFlag(Registers::Equal,   v); }
    void setGreater(bool v) { setFlag(Registers::Greater, v); }
    void setHigher(bool v)  { setFlag(Registers::Higher,  v); }
    void setLower(bool v)   { setFlag(Registers::Lower,   v); }

private:
    void setFlag(uint16_t mask, bool v) {
        if (v) r.flags |=  mask;
        else  r.flags &= ~mask;
    }
};

class Memory {
public:
    static constexpr size_t SIZE = 65536;
    std::vector<uint8_t> data;
    Memory() : data(SIZE, 0) {}

    uint8_t& operator[](uint16_t addr)       { return data.at(addr); }
    const uint8_t& operator[](uint16_t addr) const { return data.at(addr); }
    uint8_t* raw() { return data.data(); }
};

// -----------------------------------------------------------------------------
// Opcodes
// -----------------------------------------------------------------------------

enum class Opcode : uint8_t {
    NOP   = 0x01,
    HLT   = 0x02,

    MOV   = 0x08, MOV_BX = 0x09, MOV_CX = 0x0A, MOV_DX = 0x0B, MOV_SP = 0x0C,

    STE   = 0x10, CLE   = 0x11,
    STG   = 0x12, CLG   = 0x13,
    STH   = 0x14, CLH   = 0x15,
    STL   = 0x16, CLL   = 0x17,

    PUSH  = 0x1A, POP   = 0x1B,

    ADD   = 0x20, SUB   = 0x21, MUL   = 0x22, DIV   = 0x23,

    PRN   = 0x30,        // Print AX

    JMP   = 0x31,        // Unconditional jump
    JZ    = 0x32,        // Jump if AX == 0
    JNZ   = 0x33         // Jump if AX != 0
};

// -----------------------------------------------------------------------------
// Instruction Struct
// -----------------------------------------------------------------------------

struct Instruction {
    Opcode   op;
    uint16_t a1 = 0;
    uint16_t a2 = 0;
};

// -----------------------------------------------------------------------------
// VM Class
// -----------------------------------------------------------------------------

class VM {
public:
    CPU cpu;
    Memory memory;
    uint16_t breakLine = 0;

    VM() = default;

    void loadProgram(const std::vector<Instruction>& program);
    void execute();

private:
    Instruction fetchNextInstruction();
    void executeInstruction(const Instruction& instr);
    void handleError(const std::string& msg, bool fatal = true);
    uint8_t getInstructionSize(Opcode op);
    void push(uint16_t val);
    uint16_t pop();
};
