#pragma once

#include<memory>

#include "Memory.h"


class Bus {
public:
    Bus(const std::shared_ptr<Memory> & memory);

    uint16_t read(uint32_t address) const;
    void write(uint32_t address, uint16_t value);

private:
    std::shared_ptr<Memory> m_memory;
};
