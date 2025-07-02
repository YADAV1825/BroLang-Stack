#include "RohitVM.hpp"
#include <iostream>
#include <map>

// -----------------------------------------------------------------------------
// loadProgram: write instructions into memory
// -----------------------------------------------------------------------------
void VM::loadProgram(const std::vector<Instruction>& program) {
    auto mem = memory.raw();
    breakLine = 0;
    for (auto& instr : program) {
        mem[breakLine++] = static_cast<uint8_t>(instr.op);
        uint8_t size = getInstructionSize(instr.op);
        if (size >= 2) {
            mem[breakLine++] = instr.a1 & 0xFF;
            mem[breakLine++] = (instr.a1 >> 8) & 0xFF;
        }
        if (size == 5) {
            mem[breakLine++] = instr.a2 & 0xFF;
            mem[breakLine++] = (instr.a2 >> 8) & 0xFF;
        }
    }
}

// -----------------------------------------------------------------------------
// execute: run fetch-decode-execute loop
// -----------------------------------------------------------------------------
void VM::execute() {
    try {
        std::cout << "Starting VM Execution...\n";
        while (true) {
            auto instr = fetchNextInstruction();
            executeInstruction(instr);
            if (instr.op == Opcode::HLT) {
                std::cout << "Program Halted.\n";
                break;
            }
        }
    } catch (const std::exception& ex) {
        handleError(ex.what());
    }
}

// -----------------------------------------------------------------------------
// fetchNextInstruction: decode next bytes into Instruction
// -----------------------------------------------------------------------------
Instruction VM::fetchNextInstruction() {
    uint16_t ip = cpu.r.ip;
    Opcode op = static_cast<Opcode>(memory[ip]);
    uint8_t size = getInstructionSize(op);

    Instruction instr;
    instr.op = op;
    if (size >= 2) {
        instr.a1 = memory[ip + 1] | (memory[ip + 2] << 8);
    }
    if (size == 5) {
        instr.a2 = memory[ip + 3] | (memory[ip + 4] << 8);
    }
    cpu.r.ip += size;
    return instr;
}

// -----------------------------------------------------------------------------
// getInstructionSize: number of bytes each opcode uses
// -----------------------------------------------------------------------------
uint8_t VM::getInstructionSize(Opcode op) {
    static std::map<Opcode, uint8_t> m = {
        {Opcode::NOP, 1}, {Opcode::HLT, 1},
        {Opcode::MOV, 3}, {Opcode::MOV_BX, 3}, {Opcode::MOV_CX, 3},
        {Opcode::MOV_DX, 3}, {Opcode::MOV_SP, 3},
        {Opcode::STE, 1}, {Opcode::CLE, 1},
        {Opcode::STG, 1}, {Opcode::CLG, 1},
        {Opcode::STH, 1}, {Opcode::CLH, 1},
        {Opcode::STL, 1}, {Opcode::CLL, 1},
        {Opcode::PUSH, 3}, {Opcode::POP, 3},
        {Opcode::ADD, 1}, {Opcode::SUB, 1}, {Opcode::MUL, 1}, {Opcode::DIV, 1},
        {Opcode::PRN, 1},
        {Opcode::JMP, 3}, {Opcode::JZ, 3},  {Opcode::JNZ, 3}
    };
    return m[op];
}

// -----------------------------------------------------------------------------
// executeInstruction: perform the operation
// -----------------------------------------------------------------------------
void VM::executeInstruction(const Instruction& instr) {
    switch (instr.op) {
        case Opcode::NOP: break;

        case Opcode::HLT:
            std::cout << "System Halted\n";
            std::cout << "AX: " << cpu.r.ax << ", BX: " << cpu.r.bx
                      << ", CX: " << cpu.r.cx << ", DX: " << cpu.r.dx
                      << ", SP: " << cpu.r.sp << "\n";
            RohitUtils::printhex(memory.raw() + 0xFFFF - 32, 32, ' ');
            break;

        // --- MOVs ---
        case Opcode::MOV:    cpu.r.ax = instr.a1; break;
        case Opcode::MOV_BX: cpu.r.bx = instr.a1; break;
        case Opcode::MOV_CX: cpu.r.cx = instr.a1; break;
        case Opcode::MOV_DX: cpu.r.dx = instr.a1; break;
        case Opcode::MOV_SP: cpu.r.sp = instr.a1; break;

        // --- Arithmetic ---
        case Opcode::ADD: cpu.r.ax += cpu.r.bx; break;
        case Opcode::SUB: cpu.r.ax -= cpu.r.bx; break;
        case Opcode::MUL: cpu.r.ax *= cpu.r.bx; break;
        case Opcode::DIV:
            if (cpu.r.bx == 0) handleError("Division by zero");
            cpu.r.ax /= cpu.r.bx;
            break;

        // --- Flags ---
        case Opcode::STE: cpu.setEqual(true);  break;
        case Opcode::CLE: cpu.setEqual(false); break;
        case Opcode::STG: cpu.setGreater(true);  break;
        case Opcode::CLG: cpu.setGreater(false); break;
        case Opcode::STH: cpu.setHigher(true);  break;
        case Opcode::CLH: cpu.setHigher(false); break;
        case Opcode::STL: cpu.setLower(true);  break;
        case Opcode::CLL: cpu.setLower(false); break;

        // --- Stack ---
        case Opcode::PUSH:
            if      (instr.a1 == 0) push(cpu.r.ax);
            else if (instr.a1 == 1) push(cpu.r.bx);
            else if (instr.a1 == 2) push(cpu.r.cx);
            else if (instr.a1 == 3) push(cpu.r.dx);
            else handleError("Invalid PUSH register");
            break;

        case Opcode::POP:
            if      (instr.a1 == 0) cpu.r.ax = pop();
            else if (instr.a1 == 1) cpu.r.bx = pop();
            else if (instr.a1 == 2) cpu.r.cx = pop();
            else if (instr.a1 == 3) cpu.r.dx = pop();
            else handleError("Invalid POP register");
            break;

        // --- Print ---
        case Opcode::PRN:
            std::cout << "Output: " << cpu.r.ax << "\n";
            std::cout << "HUMAN OUTPUT: " << cpu.r.ax << "\n";
            break;

        // --- Jumps ---
        case Opcode::JMP:
            cpu.r.ip = instr.a1;
            break;

        case Opcode::JZ:
            if (cpu.r.ax == 0) cpu.r.ip = instr.a1;
            break;

        case Opcode::JNZ:
            if (cpu.r.ax != 0) cpu.r.ip = instr.a1;
            break;

        default:
            handleError("Illegal Instruction");
            break;
    }
}

// -----------------------------------------------------------------------------
// push / pop
// -----------------------------------------------------------------------------
void VM::push(uint16_t val) {
    if (cpu.r.sp < 2) handleError("Stack Overflow");
    cpu.r.sp -= 2;
    memory[cpu.r.sp]     = val & 0xFF;
    memory[cpu.r.sp + 1] = (val >> 8) & 0xFF;
}

uint16_t VM::pop() {
    if (cpu.r.sp > Memory::SIZE - 2) handleError("Stack Underflow");
    uint16_t val = memory[cpu.r.sp] | (memory[cpu.r.sp + 1] << 8);
    cpu.r.sp += 2;
    return val;
}

// -----------------------------------------------------------------------------
// handleError
// -----------------------------------------------------------------------------
void VM::handleError(const std::string& msg, bool fatal) {
    std::cerr << "VM Error: " << msg << "\n";
    if (fatal) std::exit(EXIT_FAILURE);
}
