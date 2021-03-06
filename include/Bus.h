#pragma once

#include <memory>
#include <vector>

#include "BusDevice.h"
#include "Display.h"
#include "Error.h"
#include "Helpers.h"
#include "Memory.h"

class Bus : public Error {
public:
    void add(const std::shared_ptr<BusDevice>&);

    uint16_t read(uint32_t address);
    void write(uint32_t address, uint16_t value);

private:
    std::vector<std::shared_ptr<BusDevice>> m_devices;
};
