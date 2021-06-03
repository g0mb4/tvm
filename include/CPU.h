#pragma once

#include <stdint.h>
#include <string.h>

#include "Bus.h"
#include "Error.h"
#include "Helpers.h"
#include "Instruction.h"
#include "Memory.h"
#include "Stack.h"

class CPU : public Error {
public:
    CPU(const std::shared_ptr<Bus>& bus, const std::shared_ptr<Stack>& stack);

    void reset();

    void step();

    const uint16_t* registers() const { return m_registers; }
    uint16_t program_counter() const { return m_program_counter; }
    uint16_t stack_pointer() const { return m_stack_pointer; }
    bool carry_flag() const { return m_carry_flag; }
    bool zero_flag() const { return m_zero_flag; }

    const std::shared_ptr<Instruction>& current_instruction() const { return m_current_instruction; }

    bool is_halted() const { return m_halted; }

private:
    uint16_t m_registers[8];
    uint16_t m_program_counter;
    uint16_t m_stack_pointer;
    bool m_carry_flag, m_zero_flag;

    std::shared_ptr<Bus> m_bus;
    std::shared_ptr<Stack> m_stack;
    std::shared_ptr<Instruction> m_current_instruction;
    uint16_t m_current_raw_instruction;

    bool m_halted { false };

    void fetch();
    uint16_t fetch_data();
    void decode();
    void execute();

    void mov();
    void lea();
    void jnz();
    void hlt();
    void prn();
    void sub();
    void inc();
    void jnc();
    void jsr();
    void rts();
};
