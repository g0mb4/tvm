#pragma once

#include <stdint.h>
#include <string.h>

#include <vector>

#include "BusDevice.h"

class Memory : public BusDevice {
public:
    static constexpr uint32_t size = 2000;

    Memory() : BusDevice(0, Memory::size - 1) {}

    void load(const uint8_t * data, uint32_t len);
    void load(const std::vector<uint8_t> & data);

    uint16_t read(uint32_t address) const override;
    void write(uint32_t address, uint16_t value) override;

    const uint16_t * data() const { return m_data; }
private:
    uint16_t m_data[size] {};

    bool is_big_endian() const;
};
