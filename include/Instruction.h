#pragma once

#include <stdint.h>

#include <map>
#include <string>

#include "Error.h"
#include "Helpers.h"

class Instruction : public Error {
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
        DirectRegister,
        IndirectRegister
    };

    struct InstructionDescription {
        const std::string name;
        uint8_t number_of_operands;
        const std::string allowed_source_addressing_modes;
        const std::string allowed_destination_addressing_modes;
    };

    Instruction(uint16_t);

    const std::string& name() const { return m_name; }
    OpCode opcode() const { return m_opcode; }

    AddressingMode source_addressing() const { return m_source_addressing; }
    AddressingMode destination_addressing() const { return m_destination_addressing; }
    uint8_t source_register() const { return m_source_register; }
    uint8_t destination_register() const { return m_destination_register; }

    void add_additional_word_source(uint16_t word) { m_additional_word_source = word; }
    void add_additional_word_destination(uint16_t word) { m_additional_word_destination = word; }

    uint16_t additional_word_source() const { return m_additional_word_source; }
    uint16_t additional_word_destination() const { return m_additional_word_destination; }

    const std::string source_addressing_string() const { return addressing_to_string(m_source_addressing); }
    const std::string destination_addressing_string() const { return addressing_to_string(m_destination_addressing); }

    bool needs_additional_source_word() const;
    bool needs_additional_destination_word() const;

private:
    std::string m_name;
    OpCode m_opcode;
    uint8_t m_number_of_operands;
    AddressingMode m_source_addressing;
    uint8_t m_source_register;
    AddressingMode m_destination_addressing;
    uint8_t m_destination_register;

    uint16_t m_additional_word_source;
    uint16_t m_additional_word_destination;

    const std::string addressing_to_string(AddressingMode) const;

    static std::map<Instruction::OpCode, InstructionDescription> s_opcodes;
};
