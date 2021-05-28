#include "Instruction.h"

std::map<uint8_t, const char *> Instruction::s_opcodes = {
    {0x0, "mov"},
    {0x1, "cmp"},
    {0x2, "add"},
    {0x3, "sub"},
    {0x4, "mul"},
    {0x5, "div"},
    {0x6, "lea"},
    {0x7, "inc"},
    {0x8, "dec"},
    {0x9, "jnz"},
    {0xa, "jnc"},
    {0xb, "shl"},
    {0xc, "prn"},
    {0xd, "jsr"},
    {0xe, "rts"},
    {0xf, "hlt"}
};

Instruction::Instruction(uint16_t data){
    m_opcode = (data >> 12) & 0xF;
    m_name = std::string(s_opcodes[m_opcode]);

    uint8_t source_addr = (data >> 9) & 0x7;
    m_source_addressing = static_cast<AddressingMode>(source_addr);
    m_source_register = (data >> 6) & 0x7;

    uint8_t dest_addr = (data >> 3) & 0x7;
    m_destination_addressing = static_cast<AddressingMode>(dest_addr);
    m_destination_register = data & 0x7;
}
