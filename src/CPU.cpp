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

void CPU::step(){
    fetch();
    decode();
    execute();
}

void CPU::fetch(){
    m_current_raw_instruction = m_bus->read(m_program_counter);
    m_program_counter++;
}

void CPU::decode(){
    m_current_instruction = std::make_shared<Instruction>(m_current_raw_instruction);
}

void CPU::execute(){

}
