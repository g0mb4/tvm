#pragma once

#include <stdio.h>

#include <string>

class Helpers {
public:
    static std::string value_to_hex_string(uint16_t value)
    {
        char hex[5];
        snprintf(hex, sizeof(hex), "%04X", value);
        return std::string(hex);
    }
};
