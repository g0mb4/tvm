#include "Memory.h"

void Memory::load(const uint8_t * data, uint32_t len){
    // NOTE: Memory::size is in words
    if(len >= Memory::size * 2){
        return;
    }

    // data is not valid
    if(len % 2 != 0){
        return;
    }

    memset(m_data, 0, sizeof(uint16_t) * Memory::size);

    bool big_endian = is_host_big_endian();
    uint16_t value = 0;

    for(uint32_t i = 0, j = 0; i < len; i += 2){
        if(big_endian){
            value = data[i] << 8 | data[i + 1];
        } else {
            value = data[i + 1] << 8 | data[i];
        }

        m_data[j++] = value;
    }
}

void Memory::load(const std::vector<uint8_t> & data){
    load(data.data(), data.size());
}

// NOTE: boundaries checked by the Bus
uint16_t Memory::read(uint32_t address) const {
    return m_data[address];
}

// NOTE: boundaries checked by the Bus
void Memory::write(uint32_t address, uint16_t value) {
    m_data[address] = value;
}

bool Memory::is_host_big_endian() const {
    uint16_t num = 1;
    if(*(uint8_t *)&num == 1){
        return false;
    } else {
        return true;
    }
}
