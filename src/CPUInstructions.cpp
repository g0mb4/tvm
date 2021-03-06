#include "CPU.h"

#define BUS_ERROR()                             \
    if (m_bus->has_error()) {                   \
        m_error_string = m_bus->error_string(); \
        return;                                 \
    }

#define BUS_ERROR_0()                           \
    if (m_bus->has_error()) {                   \
        m_error_string = m_bus->error_string(); \
        return 0;                               \
    }

uint16_t CPU::get_source_operand_value()
{
    uint16_t source, address, address_indirect, reg;

    switch (m_current_instruction->source_addressing()) {
    case Instruction::AddressingMode::Instant:
        source = m_current_instruction->additional_word_source();
        break;

    case Instruction::AddressingMode::Direct:
        address = m_current_instruction->additional_word_source();
        source = m_bus->read(address);

        BUS_ERROR_0();
        break;

    case Instruction::AddressingMode::Indirect:
        address_indirect = m_current_instruction->additional_word_source();
        address = m_bus->read(address_indirect);

        BUS_ERROR_0();

        source = m_bus->read(address);

        BUS_ERROR_0();
        break;

    case Instruction::AddressingMode::DirectRegister:
        reg = m_current_instruction->source_register();
        source = m_registers[reg];
        break;

    case Instruction::AddressingMode::IndirectRegister:
        reg = m_current_instruction->source_register();
        address = m_registers[reg];

        source = m_bus->read(address);

        BUS_ERROR_0();
        break;

    default:
        m_error_string = "Invalid source addressing: " + std::to_string(static_cast<int>(m_current_instruction->source_addressing()));
        return 0;
    };

    return source;
}

void CPU::mov()
{
    uint16_t source, address, address_indirect, reg;

    source = get_source_operand_value();

    if (has_error()) {
        return;
    }

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::Direct:
        address = m_current_instruction->additional_word_destination();
        m_bus->write(address, source);
        break;

    case Instruction::AddressingMode::Indirect:
        address_indirect = m_current_instruction->additional_word_destination();
        address = m_bus->read(address_indirect);

        BUS_ERROR();

        m_bus->write(address, source);
        break;

    case Instruction::AddressingMode::DirectRegister:
        reg = m_current_instruction->destination_register();
        m_registers[reg] = source;
        break;

    case Instruction::AddressingMode::IndirectRegister:
        reg = m_current_instruction->destination_register();
        address = m_registers[reg];
        m_bus->write(address, source);
        break;

    default:
        m_error_string = "Invalid 'mov' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    };
}

void CPU::lea()
{
    uint16_t source, destination, address, address_indirect, reg;

    switch (m_current_instruction->source_addressing()) {
    case Instruction::AddressingMode::Direct:
        source = m_current_instruction->additional_word_source();
        break;
    default:
        m_error_string = "Invalid 'lea' source addressing: " + m_current_instruction->source_addressing_string();
        return;
    };

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::Direct:
        address = m_current_instruction->additional_word_destination();
        m_bus->write(address, source);
        break;

    case Instruction::AddressingMode::Indirect:
        address_indirect = m_current_instruction->additional_word_destination();
        address = m_bus->read(address_indirect);

        BUS_ERROR();

        m_bus->write(address, source);
        break;

    case Instruction::AddressingMode::DirectRegister:
        destination = m_current_instruction->destination_register();
        m_registers[destination] = source;
        break;

    case Instruction::AddressingMode::IndirectRegister:
        reg = m_current_instruction->destination_register();
        address = m_registers[reg];
        m_bus->write(address, source);
        break;

    default:
        m_error_string = "Invalid 'lea' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    };
}

void CPU::hlt()
{
    m_halted = true;
}

void CPU::prn()
{
    uint16_t destination, address, address_indirect, reg;

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::Instant:
        destination = m_current_instruction->additional_word_destination();
        m_bus->write(Display::address, destination);
        break;

    case Instruction::AddressingMode::Direct:
        address = m_current_instruction->additional_word_destination();
        destination = m_bus->read(address);

        BUS_ERROR();

        m_bus->write(Display::address, destination);
        break;

    case Instruction::AddressingMode::Indirect:
        address_indirect = m_current_instruction->additional_word_destination();
        address = m_bus->read(address_indirect);

        BUS_ERROR();

        destination = m_bus->read(address);

        BUS_ERROR();

        m_bus->write(Display::address, destination);
        break;

    case Instruction::AddressingMode::DirectRegister:
        reg = m_current_instruction->destination_register();
        destination = m_registers[reg];
        m_bus->write(Display::address, destination);
        break;

    case Instruction::AddressingMode::IndirectRegister:
        reg = m_current_instruction->destination_register();
        address = m_registers[reg];
        destination = m_bus->read(address);

        BUS_ERROR();

        m_bus->write(Display::address, destination);
        break;

    default:
        m_error_string = "Invalid 'prn' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    }
}

void CPU::cmp()
{
    uint16_t source, destination, address, address_indirect, reg;

    source = get_source_operand_value();

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::Instant:
        destination = m_current_instruction->additional_word_destination();
        break;

    case Instruction::AddressingMode::Direct:
        address = m_current_instruction->additional_word_destination();
        destination = m_bus->read(address);

        BUS_ERROR();
        break;

    case Instruction::AddressingMode::Indirect:
        address_indirect = m_current_instruction->additional_word_destination();
        address = m_bus->read(address_indirect);

        BUS_ERROR();

        destination = m_bus->read(address);

        BUS_ERROR();
        break;

    case Instruction::AddressingMode::DirectRegister:
        reg = m_current_instruction->destination_register();
        destination = m_registers[reg];
        break;

    case Instruction::AddressingMode::IndirectRegister:
        reg = m_current_instruction->destination_register();
        address = m_registers[reg];
        destination = m_bus->read(address);

        BUS_ERROR();
        break;

    default:
        m_error_string = "Invalid 'cmp' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    }

    int32_t result = destination - source;

    m_zero_flag = (result & 0xffff) == 0;
}

void CPU::add()
{
    uint16_t source, destination, address, address_indirect, reg;
    int32_t result;

    source = get_source_operand_value();

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::Direct:
        address = m_current_instruction->additional_word_destination();
        destination = m_bus->read(address);

        BUS_ERROR();

        result = destination + source;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();
        break;

    case Instruction::AddressingMode::Indirect:
        address_indirect = m_current_instruction->additional_word_destination();
        address = m_bus->read(address_indirect);

        BUS_ERROR();

        destination = m_bus->read(address);

        BUS_ERROR();

        result = destination + source;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();

        break;

    case Instruction::AddressingMode::DirectRegister:
        reg = m_current_instruction->destination_register();
        destination = m_registers[reg];

        result = destination + source;

        m_registers[reg] = (uint16_t)result;

        break;

    case Instruction::AddressingMode::IndirectRegister:
        reg = m_current_instruction->destination_register();
        address = m_registers[reg];
        destination = m_bus->read(address);

        BUS_ERROR();

        result = destination + source;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();

        break;

    default:
        m_error_string = "Invalid 'add' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    }

    m_zero_flag = (result & 0xffff) == 0;
    m_carry_flag = result > 0xffff;
}

void CPU::sub()
{
    uint16_t source, destination, address, address_indirect, reg;
    int32_t result;

    source = get_source_operand_value();

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::Direct:
        address = m_current_instruction->additional_word_destination();
        destination = m_bus->read(address);

        BUS_ERROR();

        result = destination - source;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();
        break;

    case Instruction::AddressingMode::Indirect:
        address_indirect = m_current_instruction->additional_word_destination();
        address = m_bus->read(address_indirect);

        BUS_ERROR();

        destination = m_bus->read(address);

        BUS_ERROR();

        result = destination - source;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();

        break;

    case Instruction::AddressingMode::DirectRegister:
        reg = m_current_instruction->destination_register();
        destination = m_registers[reg];

        result = destination - source;

        m_registers[reg] = (uint16_t)result;

        break;

    case Instruction::AddressingMode::IndirectRegister:
        reg = m_current_instruction->destination_register();
        address = m_registers[reg];
        destination = m_bus->read(address);

        BUS_ERROR();

        result = destination - source;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();

        break;

    default:
        m_error_string = "Invalid 'sub' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    }

    m_zero_flag = (result & 0xffff) == 0;
    m_carry_flag = source > destination;
}

void CPU::mul()
{
    uint16_t source, destination, address, address_indirect, reg;
    int32_t result;

    source = get_source_operand_value();

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::Direct:
        address = m_current_instruction->additional_word_destination();
        destination = m_bus->read(address);

        BUS_ERROR();

        result = destination * source;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();
        break;

    case Instruction::AddressingMode::Indirect:
        address_indirect = m_current_instruction->additional_word_destination();
        address = m_bus->read(address_indirect);

        BUS_ERROR();

        destination = m_bus->read(address);

        BUS_ERROR();

        result = destination * source;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();

        break;

    case Instruction::AddressingMode::DirectRegister:
        reg = m_current_instruction->destination_register();
        destination = m_registers[reg];

        result = destination * source;

        m_registers[reg] = (uint16_t)result;

        break;

    case Instruction::AddressingMode::IndirectRegister:
        reg = m_current_instruction->destination_register();
        address = m_registers[reg];
        destination = m_bus->read(address);

        BUS_ERROR();

        result = destination * source;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();

        break;

    default:
        m_error_string = "Invalid 'mul' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    }

    m_zero_flag = (result & 0xffff) == 0;
    m_carry_flag = result > 0xffff;
}

void CPU::div()
{
    uint16_t source, destination, address, address_indirect, reg;
    int32_t result;

    source = get_source_operand_value();

    if (source == 0) {
        m_error_string = "Division by zero.";
        return;
    }

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::Direct:
        address = m_current_instruction->additional_word_destination();
        destination = m_bus->read(address);

        BUS_ERROR();

        result = (uint16_t)destination / (uint16_t)source;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();
        break;

    case Instruction::AddressingMode::Indirect:
        address_indirect = m_current_instruction->additional_word_destination();
        address = m_bus->read(address_indirect);

        BUS_ERROR();

        destination = m_bus->read(address);

        BUS_ERROR();

        result = (uint16_t)destination / (uint16_t)source;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();

        break;

    case Instruction::AddressingMode::DirectRegister:
        reg = m_current_instruction->destination_register();
        destination = m_registers[reg];

        result = (uint16_t)destination / (uint16_t)source;

        m_registers[reg] = (uint16_t)result;

        break;

    case Instruction::AddressingMode::IndirectRegister:
        reg = m_current_instruction->destination_register();
        address = m_registers[reg];
        destination = m_bus->read(address);

        BUS_ERROR();

        result = (uint16_t)destination / (uint16_t)source;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();

        break;

    default:
        m_error_string = "Invalid 'div' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    }

    m_zero_flag = (result & 0xffff) == 0;
}

void CPU::inc()
{
    uint16_t destination, address, address_indirect, reg;
    int32_t result;

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::Direct:
        address = m_current_instruction->additional_word_destination();
        destination = m_bus->read(address);

        BUS_ERROR();

        result = destination + 1;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();
        break;

    case Instruction::AddressingMode::Indirect:
        address_indirect = m_current_instruction->additional_word_destination();
        address = m_bus->read(address_indirect);

        BUS_ERROR();

        destination = m_bus->read(address);

        BUS_ERROR();

        result = destination + 1;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();

        break;

    case Instruction::AddressingMode::DirectRegister:
        reg = m_current_instruction->destination_register();
        destination = m_registers[reg];

        result = destination + 1;

        m_registers[reg] = (uint16_t)result;

        break;

    case Instruction::AddressingMode::IndirectRegister:
        reg = m_current_instruction->destination_register();
        address = m_registers[reg];
        destination = m_bus->read(address);

        BUS_ERROR();

        result = destination + 1;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();

        break;

    default:
        m_error_string = "Invalid 'inc' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    }

    m_zero_flag = (result & 0xffff) == 0;
}

void CPU::dec()
{
    uint16_t destination, address, address_indirect, reg;
    int32_t result;

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::Direct:
        address = m_current_instruction->additional_word_destination();
        destination = m_bus->read(address);

        BUS_ERROR();

        result = destination - 1;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();
        break;

    case Instruction::AddressingMode::Indirect:
        address_indirect = m_current_instruction->additional_word_destination();
        address = m_bus->read(address_indirect);

        BUS_ERROR();

        destination = m_bus->read(address);

        BUS_ERROR();

        result = destination - 1;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();

        break;

    case Instruction::AddressingMode::DirectRegister:
        reg = m_current_instruction->destination_register();
        destination = m_registers[reg];

        result = destination - 1;

        m_registers[reg] = (uint16_t)result;

        break;

    case Instruction::AddressingMode::IndirectRegister:
        reg = m_current_instruction->destination_register();
        address = m_registers[reg];
        destination = m_bus->read(address);

        BUS_ERROR();

        result = destination - 1;

        m_bus->write(address, (uint16_t)result);

        BUS_ERROR();

        break;

    default:
        m_error_string = "Invalid 'dec' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    }

    m_zero_flag = (result & 0xffff) == 0;
}

void CPU::jump()
{
    uint16_t destination, address, reg;

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::Direct:
        destination = m_current_instruction->additional_word_destination();
        m_program_counter = destination;
        break;

    case Instruction::AddressingMode::Indirect:
        address = m_current_instruction->additional_word_destination();
        destination = m_bus->read(address);

        BUS_ERROR();

        m_program_counter = destination;
        break;

    case Instruction::AddressingMode::IndirectRegister:
        reg = m_current_instruction->destination_register();
        address = m_registers[reg];

        destination = m_bus->read(address);

        BUS_ERROR();

        m_program_counter = destination;
        break;

    default:
        m_error_string = "Invalid 'jump' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    };
}

void CPU::jnz()
{
    if (m_zero_flag == false) {
        jump();
    }
}

void CPU::jnc()
{
    if (m_carry_flag == false) {
        jump();
    }
}

void CPU::jsr()
{
    uint16_t destination, address, reg;

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::Direct:
        destination = m_current_instruction->additional_word_destination();
        break;

    case Instruction::AddressingMode::Indirect:
        address = m_current_instruction->additional_word_destination();
        destination = m_bus->read(address);

        BUS_ERROR();

        break;

    case Instruction::AddressingMode::IndirectRegister:
        reg = m_current_instruction->destination_register();
        address = m_registers[reg];
        destination = m_bus->read(address);

        BUS_ERROR();

        break;

    default:
        m_error_string = "Invalid 'jsr' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    }

    m_stack->push(m_stack_pointer, m_program_counter);

    if (m_stack->has_error()) {
        m_error_string = m_stack->error_string();
        return;
    }

    m_program_counter = destination;
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

// I think source and destination is swapped.
void CPU::shl()
{
    uint16_t source, destination, address, address_indirect, reg;
    uint32_t result;

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::Instant:
        destination = m_bus->read(address);
        break;

    case Instruction::AddressingMode::Direct:
        address = m_current_instruction->additional_word_destination();
        destination = m_bus->read(address);

        BUS_ERROR();
        break;

    case Instruction::AddressingMode::Indirect:
        address_indirect = m_current_instruction->additional_word_destination();
        address = m_bus->read(address_indirect);

        BUS_ERROR();

        destination = m_bus->read(address);

        BUS_ERROR();
        break;

    case Instruction::AddressingMode::DirectRegister:
        reg = m_current_instruction->destination_register();
        destination = m_registers[reg];

        break;

    case Instruction::AddressingMode::IndirectRegister:
        reg = m_current_instruction->destination_register();
        address = m_registers[reg];
        destination = m_bus->read(address);

        BUS_ERROR();
        break;

    default:
        m_error_string = "Invalid 'shl' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    }

    switch (m_current_instruction->source_addressing()) {
    case Instruction::AddressingMode::Direct:
        address = m_current_instruction->additional_word_source();
        source = m_bus->read(address);

        BUS_ERROR();

        result = source << destination;

        m_bus->write(address, (uint16_t)result);
        BUS_ERROR();

        break;

    case Instruction::AddressingMode::Indirect:
        address_indirect = m_current_instruction->additional_word_source();
        address = m_bus->read(address_indirect);

        BUS_ERROR();

        source = m_bus->read(address);

        BUS_ERROR();

        result = source << destination;

        m_bus->write(address, (uint16_t)result);
        BUS_ERROR();

        break;

    case Instruction::AddressingMode::DirectRegister:
        reg = m_current_instruction->source_register();
        source = m_registers[reg];

        result = source << destination;

        m_bus->write(address, (uint16_t)result);
        BUS_ERROR();

        break;

    case Instruction::AddressingMode::IndirectRegister:
        reg = m_current_instruction->source_register();
        address = m_registers[reg];

        source = m_bus->read(address);
        BUS_ERROR();

        result = source << destination;

        m_bus->write(address, (uint16_t)result);
        BUS_ERROR();

        break;

    default:
        m_error_string = "Invalid 'shl' source addressing: " + m_current_instruction->source_addressing_string();
        return;
    };

    m_zero_flag = (result & 0xffff) == 0;
    m_carry_flag = result > 0xffff;
}
