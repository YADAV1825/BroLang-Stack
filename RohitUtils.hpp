#pragma once  // Ensures the header is only included once during compilation

#include <cstdint>     // Fixed-width integer types
#include <cstdio>      // printf and related functions
#include <string>      // Optional for future use
#include <arpa/inet.h> // For in_addr_t (IP representation)

// =====================
// Author: Rohit Yadav
// Purpose: Declares helper utility functions used in the VM project
// =====================

namespace RohitUtils {

    // Use standard types for clarity and portability
    void copy(uint8_t* dst, const uint8_t* src, uint16_t size);

    // Convert 16-bit network byte order to host byte order
    uint16_t nstoh(uint16_t srcport);

    // Zero out a memory region
    void zero(uint8_t* str, uint16_t size);

    // Print memory block as hex values
    void printhex(const uint8_t* str, uint16_t size, char delim = ' ');

    // Convert a 32-bit IP address to dotted decimal format
    const char* todotted(uint32_t ip);

} // namespace RohitUtils
