#pragma once

#include<memory>

#include "Memory.h"
#include "Display.h"

class Bus {
public:
    Bus(const std::shared_ptr<Memory> & memory, const std::shared_ptr<Display> & display);

    uint16_t read(uint32_t address) const;
    void write(uint32_t address, uint16_t value);

private:
    std::shared_ptr<Memory> m_memory;
    std::shared_ptr<Display> m_display;
};
