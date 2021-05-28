#include "Bus.h"

Bus::Bus(const std::shared_ptr<Memory> & memory)
 : m_memory(memory)
{}

uint16_t Bus::read(uint32_t address) const{
    if(address < Memory::size){
        return m_memory->read(address);
    }

    // FIXME: report error
    return 0;
}

void Bus::write(uint32_t address, uint16_t value){
    if(address < Memory::size){
        m_memory->write(address, value);
    }

    // FIXME: report error
}
