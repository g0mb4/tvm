#include "CPU.h"

CPU::CPU(const std::shared_ptr<Bus> & bus)
 : m_bus(bus)
{
    reset();
}

void CPU::reset(){
    memset(m_registers, 0, sizeof(uint16_t) * 8);
    m_program_counter = 0;
    m_stack_pointer = 1800;
    m_carry_flag = 0;
    m_zero_flag = 0;
}
