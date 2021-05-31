#pragma once

#include <stdint.h>

class BusDevice{
public:
    BusDevice(uint32_t start_address, uint32_t end_address)
        : m_start_address(start_address), m_end_address(end_address) {}

    virtual uint16_t read(uint32_t address) const = 0;
    virtual void write(uint32_t address, uint16_t value) = 0;

    uint32_t start_address() const { return m_start_address; }
    uint32_t end_address() const { return m_end_address; }

protected:
    uint32_t m_start_address;
    uint32_t m_end_address;
};
