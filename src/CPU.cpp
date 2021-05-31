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
    m_carry_flag = false;
    m_zero_flag = false;

    m_halted = false;

    m_current_raw_instruction = 0;
    m_current_instruction = nullptr;
}

void CPU::step(){
    if(has_error())
        return;

    fetch();

    if(has_error())
        return;

    decode();

    if(has_error())
        return;

    execute();
}

void CPU::fetch(){
    m_current_raw_instruction = m_bus->read(m_program_counter);
    if(m_bus->has_error()){
        m_error_string = m_bus->error_string();
    } else {
        m_program_counter++;
    }
}

uint16_t CPU::fetch_data(){
    uint16_t data = m_bus->read(m_program_counter);
    if(m_bus->has_error()){
        m_error_string = m_bus->error_string();
    } else {
        m_program_counter++;
    }

    return data;
}

void CPU::decode(){
    m_current_instruction = std::make_shared<Instruction>(m_current_raw_instruction);

    if(!m_current_instruction->is_valid()){
        m_error_string = "Invalid instruction: " + Helpers::value_to_hex_string(m_current_raw_instruction);
        return;
    }

    uint8_t no_ops = m_current_instruction->number_of_operands();

    if(no_ops == 2 && m_current_instruction->source_addressing() != Instruction::AddressingMode::DirectRegister
      && m_current_instruction->source_addressing() != Instruction::AddressingMode::IndirectRegister){
        m_current_instruction->add_additional_word_source(fetch_data());
    }

    if((no_ops == 2 || no_ops == 1) && m_current_instruction->destination_addressing() != Instruction::AddressingMode::DirectRegister
      && m_current_instruction->destination_addressing() != Instruction::AddressingMode::IndirectRegister){
        m_current_instruction->add_additional_word_destination(fetch_data());
    }
}

void CPU::execute(){
    switch(m_current_instruction->opcode()){
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
    default:
        m_error_string = "Unsupported instruction: " + m_current_instruction->name() + " (" + Helpers::value_to_hex_string(m_current_raw_instruction) + ")";
    }
}

void CPU::mov(){
    uint16_t source = 0;
    uint16_t destination = 0;

    switch (m_current_instruction->source_addressing()) {
    case Instruction::AddressingMode::Instant:
        source = m_current_instruction->additional_word_source();
        break;
    case Instruction::AddressingMode::Direct:{
        uint16_t address = m_current_instruction->additional_word_source();
        source = m_bus->read(address);
        }
        break;
    default:
        m_error_string = "Unsupported 'mov' source addressing:" + std::to_string((int)m_current_instruction->source_addressing());
        return;
    };

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::DirectRegister:
        destination = m_current_instruction->destination_register();
        m_registers[destination] = source;
        break;
    default:
        m_error_string = "Unsupported 'mov' destination addressing:" + std::to_string((int)m_current_instruction->source_addressing());
        return;
    };
}

void CPU::lea(){
    uint16_t source = 0;
    uint16_t destination = 0;

    switch (m_current_instruction->source_addressing()) {
    case Instruction::AddressingMode::Direct:
        source = m_current_instruction->additional_word_source();
        break;
    default:
        m_error_string = "Unsupported 'lea' source addressing:" + std::to_string((int)m_current_instruction->source_addressing());
        return;
    };

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::DirectRegister:
        destination = m_current_instruction->destination_register();
        m_registers[destination] = source;
        break;
    default:
        m_error_string = "Unsupported 'lea' destination addressing:" + std::to_string((int)m_current_instruction->source_addressing());
        return;
    };
}

void CPU::jnz(){
    if(m_zero_flag){
        return;
    }

    uint16_t destination;

    switch (m_current_instruction->destination_addressing()) {
    case Instruction::AddressingMode::Direct:
        destination = m_current_instruction->additional_word_destination();
        m_program_counter = destination;
        break;
    default:
        m_error_string = "Unsupported 'mov' destination addressing:" + std::to_string((int)m_current_instruction->source_addressing());
        return;
    };
}

void CPU::hlt(){
    m_halted = true;
}
