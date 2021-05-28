#pragma once

#include <stdint.h>

#include <string>
#include <map>

class Instruction {
public:
    enum class AddressingMode {
        Instant,
        Direct,
        Indirect,
        Relative,
        DirectRegister,
        IndirectRegister
    };

    Instruction(uint16_t data);

private:
    std::string m_name;
    uint8_t m_opcode;
    AddressingMode m_source_addressing;
    uint8_t m_source_register;
    AddressingMode m_destination_addressing;
    uint8_t m_destination_register;

    static std::map<uint8_t, const char *> s_opcodes;
};
