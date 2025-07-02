// RohitUtils.cpp
// This file contains utility/helper functions used across the virtual machine (VM) project.
// These functions handle tasks like memory copying, byte order conversion, zeroing memory,
// printing memory contents in hex format, and converting IP addresses to readable form.

#include "RohitUtils.hpp"
#include <cstdint>  // For uint8_t, uint16_t, etc.

// All utility functions are defined inside the RohitUtils namespace
namespace RohitUtils {

    // -------------------------------
    // Function: copy
    // Purpose: Copies a block of memory from 'src' to 'dst'
    // Parameters:
    //   - dst: destination memory address
    //   - src: source memory address
    //   - size: number of bytes to copy
    // Why it's here:
    //   - Useful for copying memory (e.g., stack frames, data blocks) within the VM.
    //   - Acts like the standard memcpy function but defined manually here for control.
    // Helps the code by:
    //   - Avoiding dependency on external libraries.
    void copy(uint8_t* dst, const uint8_t* src, uint16_t size) {
        for (uint16_t i = 0; i < size; ++i)
            dst[i] = src[i];  // Copy byte-by-byte
    }

    // -------------------------------
    // Function: nstoh (Network Short to Host)
    // Purpose: Converts a 16-bit number from network byte order to host byte order
    // Parameters:
    //   - srcport: the 16-bit number (typically a port) in network byte order
    // Why it's here:
    //   - Network uses Big Endian. Most CPUs (like x86) use Little Endian.
    // Helps the code by:
    //   - Ensuring cross-platform correctness when interpreting binary data
    uint16_t nstoh(uint16_t srcport) {
        // Swap high and low bytes (manual swap)
        uint8_t high = (srcport & 0xFF00) >> 8;
        uint8_t low  = (srcport & 0x00FF);
        return (low << 8) | high;
    }

    // -------------------------------
    // Function: zero
    // Purpose: Clears (sets to zero) a block of memory
    // Parameters:
    //   - str: pointer to the memory block
    //   - size: number of bytes to set to zero
    // Why it's here:
    //   - Useful to initialize memory (e.g., RAM, registers, buffers) before use.
    // Helps the code by:
    //   - Avoiding undefined behavior by making sure all values start from 0.
    void zero(uint8_t* str, uint16_t size) {
        for (uint16_t i = 0; i < size; ++i)
            str[i] = 0;
    }

    // -------------------------------
    // Function: printhex
    // Purpose: Prints a memory block as hexadecimal numbers
    // Parameters:
    //   - str: pointer to the memory block
    //   - size: number of bytes to print
    //   - delim: optional character to print between hex bytes (e.g., space or dash)
    // Why it's here:
    //   - Helps in debugging by allowing developers to see the memory contents in readable hex.
    // Helps the code by:
    //   - Allowing the VM to show what's stored in memory in a clean and readable way.
    void printhex(const uint8_t* str, uint16_t size, char delim) {
        for (uint16_t i = 0; i < size; ++i) {
            printf("%.02x", str[i]);  // Print each byte in 2-digit hex format
            if (delim)
                printf("%c", delim);  // Add delimiter if set
        }
        printf("\n");
        fflush(stdout);  // Ensure immediate output
    }

    // -------------------------------
    // Function: todotted
    // Purpose: Converts a 32-bit IP address (in_addr_t) to a human-readable dotted decimal format
    //          Example: 3232235777 -> "192.168.1.1"
    // Parameters:
    //   - ip: IPv4 address as a 32-bit integer
    // Returns:
    //   - A string representing the IP in dotted-decimal format
    // Why it's here:
    //   - Some instructions or debug outputs may involve IP addresses.
    // Helps the code by:
    //   - Turning low-level binary IP into something human-readable.
    const char* todotted(uint32_t ip) {
        static char buf[16];  // Enough for "255.255.255.255"
        snprintf(buf, sizeof(buf), "%u.%u.%u.%u",
            (ip & 0xFF000000) >> 24,
            (ip & 0x00FF0000) >> 16,
            (ip & 0x0000FF00) >> 8,
            (ip & 0x000000FF));
        return buf;
    }

} // namespace RohitUtils
