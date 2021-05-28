#include "Bus.h"

Bus::Bus(const std::shared_ptr<Memory> & memory, const std::shared_ptr<Display> & display)
 : m_memory(memory)
 , m_display(display)
{}

uint16_t Bus::read(uint32_t address) const{
    if(address < Memory::size){
        return m_memory->read(address);
    }

    // FIXME: report error
    return 0;
}

void Bus::write(uint32_t address, uint16_t value){
    if(address == Display::address){
        m_display->add_character((char)value);
    }

    if(address < Memory::size){
        m_memory->write(address, value);
    }

    // FIXME: report error
}
