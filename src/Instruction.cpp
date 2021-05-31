#include "Instruction.h"

std::map<Instruction::OpCode, Instruction::InstructionDescription> Instruction::s_opcodes = {
    {Instruction::OpCode::mov, {"mov", 2}},
    {Instruction::OpCode::cmp, {"cmp", 2}},
    {Instruction::OpCode::add, {"add", 2}},
    {Instruction::OpCode::sub, {"sub", 2}},
    {Instruction::OpCode::mul, {"mul", 2}},
    {Instruction::OpCode::div, {"div", 2}},
    {Instruction::OpCode::lea, {"lea", 2}},
    {Instruction::OpCode::inc, {"inc", 1}},
    {Instruction::OpCode::dec, {"dec", 1}},
    {Instruction::OpCode::jnz, {"jnz", 1}},
    {Instruction::OpCode::jnc, {"jnc", 1}},
    {Instruction::OpCode::shl, {"shl", 2}},
    {Instruction::OpCode::prn, {"prn", 1}},
    {Instruction::OpCode::jsr, {"jsr", 1}},
    {Instruction::OpCode::rts, {"rts", 0}},
    {Instruction::OpCode::hlt, {"hlt", 0}}
};

// FIXME: check if instruction is valid
Instruction::Instruction(uint16_t data){
    uint8_t opcode = (data >> 12) & 0xF;
    m_opcode = static_cast<OpCode>(opcode);
    m_name = std::string(s_opcodes[m_opcode].name);
    m_number_of_operands = s_opcodes[m_opcode].number_of_operands;

    uint8_t source_addr = (data >> 9) & 0x7;
    m_source_addressing = static_cast<AddressingMode>(source_addr);
    m_source_register = (data >> 6) & 0x7;

    uint8_t dest_addr = (data >> 3) & 0x7;
    m_destination_addressing = static_cast<AddressingMode>(dest_addr);
    m_destination_register = data & 0x7;
}
