#include "Instruction.h"

std::map<Instruction::OpCode, Instruction::InstructionDescription> Instruction::s_opcodes = {
    { Instruction::OpCode::mov, { "mov", 2, "01234", "1234" } },
    { Instruction::OpCode::cmp, { "cmp", 2, "01234", "01234" } },
    { Instruction::OpCode::add, { "add", 2, "01234", "1234" } },
    { Instruction::OpCode::sub, { "sub", 2, "01234", "1234" } },
    { Instruction::OpCode::mul, { "mul", 2, "01234", "1234" } },
    { Instruction::OpCode::div, { "div", 2, "01234", "1234" } },
    { Instruction::OpCode::lea, { "lea", 2, "1", "1234" } },
    { Instruction::OpCode::inc, { "inc", 1, "", "1234" } },
    { Instruction::OpCode::dec, { "dec", 1, "", "1234" } },
    { Instruction::OpCode::jnz, { "jnz", 1, "", "124" } },
    { Instruction::OpCode::jnc, { "jnc", 1, "", "124" } },
    { Instruction::OpCode::shl, { "shl", 2, "1234", "01234" } },
    { Instruction::OpCode::prn, { "prn", 1, "", "01234" } },
    { Instruction::OpCode::jsr, { "jsr", 1, "", "124" } },
    { Instruction::OpCode::rts, { "rts", 0, "", "" } },
    { Instruction::OpCode::hlt, { "hlt", 0, "", "" } }
};

Instruction::Instruction(uint16_t data)
{
    uint8_t opcode = (data >> 12) & 0xF;
    m_opcode = static_cast<OpCode>(opcode);

    auto it = s_opcodes.find(m_opcode);
    if (it == s_opcodes.end()) {
        m_error_string = "Opcode is not valid: " + Helpers::value_to_hex_string(opcode, 1);
        return;
    }

    auto instruction = it->second;
    m_name = instruction.name;
    m_number_of_operands = instruction.number_of_operands;

    if (m_number_of_operands == 2) {
        const std::string& allowed_source_mode = instruction.allowed_source_addressing_modes;
        uint8_t source_addr = (data >> 9) & 0x7;

        bool valid_source = false;
        for (char c : allowed_source_mode) {
            if (c - '0' == source_addr) {
                valid_source = true;
                false;
            }
        }

        m_source_addressing = static_cast<AddressingMode>(source_addr);

        if (!valid_source) {
            m_error_string = "Invalid source addressing for '" + m_name + "': " + source_addressing_string();
            return;
        }

        m_source_register = (data >> 6) & 0x7;

        if (m_source_register > 7) {
            m_error_string = "Invalid source register for '" + m_name + "': " + std::to_string(m_source_register);
            return;
        }
    }

    if (m_number_of_operands == 2 || m_number_of_operands == 1) {
        const std::string& allowed_dest_mode = instruction.allowed_destination_addressing_modes;
        uint8_t dest_addr = (data >> 3) & 0x7;

        bool valid_dest = false;
        for (char c : allowed_dest_mode) {
            if (c - '0' == dest_addr) {
                valid_dest = true;
                false;
            }
        }

        m_destination_addressing = static_cast<AddressingMode>(dest_addr);

        if (!valid_dest) {
            m_error_string = "Invalid destination addressing for '" + m_name + "': " + destination_addressing_string();
            return;
        }

        m_destination_register = data & 0x7;

        if (m_destination_register > 7) {
            m_error_string = "Invalid destination register for '" + m_name + "': " + std::to_string(m_destination_register);
            return;
        }
    }
}

const std::string Instruction::addressing_to_string(AddressingMode mode) const
{
    switch (mode) {
    case AddressingMode::Instant:
        return "Istant";
    case AddressingMode::Direct:
        return "Direct";
    case AddressingMode::Indirect:
        return "Indirect";
    case AddressingMode::DirectRegister:
        return "Direct Register";
    case AddressingMode::IndirectRegister:
        return "Indirect Register";
    default:
        return "Unknown";
    }
}
