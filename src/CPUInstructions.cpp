#include "CPU.h"

void CPU::mov()
{
    uint16_t source, destination, address, reg;

    switch (m_current_instruction->source_addressing()) {
    case Instruction::AddressingMode::Instant:
        source = m_current_instruction->additional_word_source();
        break;
    case Instruction::AddressingMode::Direct:
        address = m_current_instruction->additional_word_source();
        source = m_bus->read(address);
        break;
    default:
        m_error_string = "Unimplemented 'mov' source addressing: " + m_current_instruction->source_addressing_string();
        return;
    };

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::DirectRegister:
        destination = m_current_instruction->destination_register();
        m_registers[destination] = source;
        break;
    default:
        m_error_string = "Unimplemented 'mov' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    };
}

void CPU::lea()
{
    uint16_t source, destination;

    switch (m_current_instruction->source_addressing()) {
    case Instruction::AddressingMode::Direct:
        source = m_current_instruction->additional_word_source();
        break;
    default:
        m_error_string = "Unimplemented 'lea' source addressing: " + m_current_instruction->source_addressing_string();
        return;
    };

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::DirectRegister:
        destination = m_current_instruction->destination_register();
        m_registers[destination] = source;
        break;
    default:
        m_error_string = "Unimplemented 'lea' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    };
}

void CPU::jnz()
{
    if (m_zero_flag) {
        return;
    }

    uint16_t destination;

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::Direct:
        destination = m_current_instruction->additional_word_destination();
        m_program_counter = destination;
        break;
    default:
        m_error_string = "Unimplemented 'jnz' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    };
}

void CPU::hlt()
{
    m_halted = true;
}

void CPU::prn()
{
    uint16_t destination, address, reg;

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::IndirectRegister:
        reg = m_current_instruction->destination_register();
        address = m_registers[reg];
        destination = m_bus->read(address);
        m_bus->write(Display::address, destination);
        break;
    default:
        m_error_string = "Unimplemented 'mov' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    }
}

void CPU::sub()
{
    uint16_t source, destination, address, reg;

    switch (m_current_instruction->source_addressing()) {
    case Instruction::AddressingMode::Instant:
        source = m_current_instruction->additional_word_source();
        break;
    case Instruction::AddressingMode::Direct:
        address = m_current_instruction->additional_word_source();
        source = m_bus->read(address);
        break;
    default:
        m_error_string = "Unimplemented 'sub' source addressing: " + m_current_instruction->source_addressing_string();
        return;
    };

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::DirectRegister:
        destination = m_current_instruction->destination_register();

        m_carry_flag = source > m_registers[destination];
        m_registers[destination] -= source;
        m_zero_flag = m_registers[destination] == 0;

        break;
    default:
        m_error_string = "Unimplemented 'sub' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    };
}

void CPU::inc()
{
    uint16_t destination;

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::DirectRegister:
        destination = m_current_instruction->destination_register();
        m_registers[destination] += 1;
        break;
    default:
        m_error_string = "Unimplemented 'inc' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    };
}

void CPU::jnc()
{
    if (m_carry_flag) {
        return;
    }

    uint16_t destination;

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::Direct:
        destination = m_current_instruction->additional_word_destination();
        m_program_counter = destination;
        break;
    default:
        m_error_string = "Unimplemented 'jnc' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    };
}

// FIXME: remove spagetti-code
void CPU::jsr()
{
    uint16_t destination, address, reg;

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::Direct:
        destination = m_current_instruction->additional_word_destination();
        goto jsr_main;
    case Instruction::AddressingMode::Indirect:
        address = m_current_instruction->additional_word_destination();
        destination = m_bus->read(address);

        if (m_bus->has_error()) {
            m_error_string = m_bus->error_string();
            return;
        }

        goto jsr_main;
    case Instruction::AddressingMode::IndirectRegister:
        reg = m_current_instruction->destination_register();
        address = m_registers[reg];
        destination = m_bus->read(address);

        if (m_bus->has_error()) {
            m_error_string = m_bus->error_string();
            return;
        }

        goto jsr_main;
    jsr_main:
        m_stack->push(m_stack_pointer, m_program_counter);

        if (m_stack->has_error()) {
            m_error_string = m_stack->error_string();
            return;
        }

        m_program_counter = destination;
        break;
    default:
        m_error_string = "Invalid 'jsr' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    };
}

void CPU::rts()
{
    uint16_t return_point = m_stack->pop(m_stack_pointer);
    if (m_stack->has_error()) {
        m_error_string = m_stack->error_string();
        return;
    }

    m_program_counter = return_point;
}
