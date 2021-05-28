#pragma once

#include <stdint.h>

#include <string>
#include <map>

class Instruction {
public:
    enum class OpCode : uint8_t {
        mov = 0,
        cmp,
        add,
        sub,
        mul,
        div,
        lea,
        inc,
        dec,
        jnz,
        jnc,
        shl,
        prn,
        jsr,
        rts,
        hlt
    };

    enum class AddressingMode : uint8_t {
        Instant = 0,
        Direct,
        Indirect,
        Relative,
        DirectRegister,
        IndirectRegister
    };

    Instruction(uint16_t);

    const std::string & name() const { return m_name; }
    OpCode opcode() const { return m_opcode; }
    AddressingMode source_addressing() const { return m_source_addressing; }
    AddressingMode destination_addressing() const { return m_destination_addressing; }

    void add_additional_word(uint16_t);

    uint16_t additional_word();

private:
    std::string m_name;
    OpCode m_opcode;
    AddressingMode m_source_addressing;
    uint8_t m_source_register;
    AddressingMode m_destination_addressing;
    uint8_t m_destination_register;

    uint16_t m_additional_word1;
    uint16_t m_additional_word2;

    bool m_additional_word1_set {false};
    bool m_additional_word1_get {false};

    static std::map<Instruction::OpCode, const char *> s_opcodes;
};
