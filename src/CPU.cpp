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

    clear_error();
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

// NOTE: std::map??
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
    default:
        m_error_string = "Unsupported instruction: " + m_current_instruction->name() + " (" + Helpers::value_to_hex_string(m_current_raw_instruction) + ")";
    }
}

void CPU::mov(){
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

void CPU::lea(){
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
        m_error_string = "Unimplemented 'jnz' destination addressing: " + m_current_instruction->destination_addressing_string();
        return;
    };
}

void CPU::hlt(){
    m_halted = true;
}

void CPU::prn(){
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

void CPU::sub() {
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

void CPU::inc() {
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

void CPU::jnc() {
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
