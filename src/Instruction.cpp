#include "Instruction.h"

std::map<Instruction::OpCode, const char *> Instruction::s_opcodes = {
    {Instruction::OpCode::mov, "mov"},
    {Instruction::OpCode::cmp, "cmp"},
    {Instruction::OpCode::add, "add"},
    {Instruction::OpCode::sub, "sub"},
    {Instruction::OpCode::mul, "mul"},
    {Instruction::OpCode::div, "div"},
    {Instruction::OpCode::lea, "lea"},
    {Instruction::OpCode::inc, "inc"},
    {Instruction::OpCode::dec, "dec"},
    {Instruction::OpCode::jnz, "jnz"},
    {Instruction::OpCode::jnc, "jnc"},
    {Instruction::OpCode::shl, "shl"},
    {Instruction::OpCode::prn, "prn"},
    {Instruction::OpCode::jsr, "jsr"},
    {Instruction::OpCode::rts, "rts"},
    {Instruction::OpCode::hlt, "hlt"}
};

// FIXME: check if instruction is valid
Instruction::Instruction(uint16_t data){
    uint8_t opcode = (data >> 12) & 0xF;
    m_opcode = static_cast<OpCode>(opcode);
    m_name = std::string(s_opcodes[m_opcode]);

    uint8_t source_addr = (data >> 9) & 0x7;
    m_source_addressing = static_cast<AddressingMode>(source_addr);
    m_source_register = (data >> 6) & 0x7;

    uint8_t dest_addr = (data >> 3) & 0x7;
    m_destination_addressing = static_cast<AddressingMode>(dest_addr);
    m_destination_register = data & 0x7;
}
