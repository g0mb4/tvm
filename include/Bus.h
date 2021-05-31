#pragma once

#include<memory>
#include<vector>

#include "BusDevice.h"
#include "Memory.h"
#include "Display.h"

class Bus {
public:
    void add(const std::shared_ptr<BusDevice>&);

    uint16_t read(uint32_t address) const;
    void write(uint32_t address, uint16_t value);

private:
    std::vector<std::shared_ptr<BusDevice>> m_devices;
};
