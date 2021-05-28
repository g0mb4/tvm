#pragma once

#include <stdint.h>
#include <string.h>

#include "Bus.h"

class CPU
{
public:
    CPU(const std::shared_ptr<Bus> & bus);

    void reset();

    const uint16_t * registers() const { return m_registers; }
    uint16_t program_counter() const { return m_program_counter; }
    uint16_t stack_pointer() const { return m_stack_pointer; }
    bool carry_flag() const { return m_carry_flag; }
    bool zero_flag() const { return m_zero_flag; }

private:
    uint16_t m_registers[8];
    uint16_t m_program_counter;
    uint16_t m_stack_pointer;
    bool m_carry_flag, m_zero_flag;

    std::shared_ptr<Bus> m_bus;
};

