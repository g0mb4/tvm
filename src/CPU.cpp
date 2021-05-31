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

void CPU::fetch_data(){
    m_additional_instruction_data = m_bus->read(m_program_counter);
    if(m_bus->has_error()){
        m_error_string = m_bus->error_string();
    } else {
        m_program_counter++;
    }
}

void CPU::decode(){
    m_current_instruction = std::make_shared<Instruction>(m_current_raw_instruction);
    if(m_current_instruction->opcode() == Instruction::OpCode::mov){
        if(m_current_instruction->source_addressing() != Instruction::AddressingMode::DirectRegister
           && m_current_instruction->source_addressing() != Instruction::AddressingMode::IndirectRegister){
            fetch_data();
            m_current_instruction->add_additional_word(m_additional_instruction_data);
        }

        if(m_current_instruction->destination_addressing() != Instruction::AddressingMode::DirectRegister
           && m_current_instruction->destination_addressing() != Instruction::AddressingMode::IndirectRegister){
            fetch_data();
            m_current_instruction->add_additional_word(m_additional_instruction_data);
        }
    }
}

void CPU::execute(){
    switch(m_current_instruction->opcode()){
    case Instruction::OpCode::mov:
        mov();
        break;
    default:
        m_error_string = "Unsupported instruction: " + m_current_instruction->name();
    }
}

void CPU::mov(){
    m_error_string = "MOV is not supported yet";
}
