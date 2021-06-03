#pragma once

#include <stdint.h>
#include <string.h>

#include <vector>

#include "BusDevice.h"
#include "Error.h"

class Memory : public BusDevice, public Error {
public:
    static constexpr uint32_t size = 2000;

    Memory() : BusDevice(0, Memory::size - 1) { reset(); }

    void load(const uint8_t * data, uint32_t len);
    void load(const std::vector<uint8_t> & data);

    void reset() {
         memset(m_data, 0, sizeof(uint16_t) * Memory::size);
         soft_reset();
    }

    void soft_reset(){
        clear_error();
    }

    uint16_t read(uint32_t address) const override;
    void write(uint32_t address, uint16_t value) override;

    const uint16_t * data() const { return m_data; }
private:
    uint16_t m_data[size] {};

    bool is_host_big_endian() const;
};
