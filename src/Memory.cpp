#include "Memory.h"

// FIXME: check boundaries
void Memory::load(const uint8_t * data, uint32_t len){
    memset(m_data, 0, sizeof(uint16_t) * Memory::size);
    for(uint32_t i = 0, j = 0; i < len; i += 2){
        uint16_t value = data[i] << 8 | data[i+1];
        m_data[j++] = value;
    }
}

void Memory::load(const std::vector<uint8_t> & data){
    load(data.data(), data.size());
}

// FIXME: check boundaries
uint16_t Memory::read(uint32_t address) const {
    return m_data[address];
}

// FIXME: check boundaries
void Memory::write(uint32_t address, uint16_t value) {
    m_data[address] = value;
}
