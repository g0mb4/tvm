#include "CPU.h"
#include "Stack.h"

CPU::CPU(const std::shared_ptr<Bus>& bus, const std::shared_ptr<Stack>& stack)
    : m_bus(bus)
    , m_stack(stack)
{
    reset();
}

void CPU::reset()
{
    memset(m_registers, 0, sizeof(uint16_t) * 8);
    m_program_counter = 0;
    m_stack_pointer = Stack::start;
    m_carry_flag = false;
    m_zero_flag = false;

    m_halted = false;

    m_current_raw_instruction = 0;
    m_current_instruction = nullptr;

    clear_error();
}

void CPU::step()
{
    if (has_error())
        return;

    fetch();

    if (has_error())
        return;

    decode();

    if (has_error())
        return;

    execute();
}

void CPU::fetch()
{
    m_current_raw_instruction = m_bus->read(m_program_counter);
    if (m_bus->has_error()) {
        m_error_string = m_bus->error_string();
    } else {
        m_program_counter++;
    }
}

uint16_t CPU::fetch_data()
{
    uint16_t data = m_bus->read(m_program_counter);
    if (m_bus->has_error()) {
        m_error_string = m_bus->error_string();
    } else {
        m_program_counter++;
    }

    return data;
}

void CPU::decode()
{
    m_current_instruction = std::make_shared<Instruction>(m_current_raw_instruction);

    if (m_current_instruction->has_error()) {
        m_error_string = m_current_instruction->error_string();
        return;
    }

    if (m_current_instruction->needs_additional_source_word()) {
        m_current_instruction->add_additional_word_source(fetch_data());
    }

    if (has_error()) {
        return;
    }

    if (m_current_instruction->needs_additional_destination_word()) {
        m_current_instruction->add_additional_word_destination(fetch_data());
    }
}

// NOTE: std::map??
void CPU::execute()
{
    switch (m_current_instruction->opcode()) {
    case Instruction::OpCode::mov:
        mov();
        break;
    case Instruction::OpCode::lea:
        lea();
        break;
    case Instruction::OpCode::jnz:
        jnz();
        break;
    case Instruction::OpCode::hlt:
        hlt();
        break;
    case Instruction::OpCode::prn:
        prn();
        break;
    case Instruction::OpCode::sub:
        sub();
        break;
    case Instruction::OpCode::inc:
        inc();
        break;
    case Instruction::OpCode::jnc:
        jnc();
        break;
    case Instruction::OpCode::jsr:
        jsr();
        break;
    case Instruction::OpCode::rts:
        rts();
        break;
    case Instruction::OpCode::cmp:
        cmp();
        break;
    case Instruction::OpCode::add:
        add();
        break;
    case Instruction::OpCode::mul:
        mul();
        break;
    case Instruction::OpCode::div:
        div();
        break;
    case Instruction::OpCode::dec:
        dec();
        break;
    case Instruction::OpCode::shl:
        shl();
        break;
    default:
        m_error_string = "Invalid instruction:" + Helpers::value_to_hex_string(m_current_raw_instruction);
    }
}
