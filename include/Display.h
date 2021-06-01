#pragma once

#include <string>

#include "BusDevice.h"

class Display: public BusDevice {
public:
    static constexpr uint32_t address = 3000;

    Display() : BusDevice(Display::address, Display::address) { reset(); }

    void reset() { m_message = ""; }

    uint16_t read(uint32_t address) const override { return 0; }
    void write(uint32_t, uint16_t value) override {
        m_message += (char)(value);
    }

    const std::string & message() const { return m_message; }
private:
    std::string m_message;
};
