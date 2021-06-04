#pragma once

#include <stdio.h>

#include <string>

class Helpers {
public:
    static std::string value_to_hex_string(uint16_t value, uint16_t digits = 4)
    {
        char* hex = (char*)malloc(digits + 1);
        if (hex == nullptr) {
            return "";
        }

        char fmt[] = "%04X";

        if (digits < 10) {
            fmt[2] = digits + '0';
        } else if (digits < 100) {
            int tens = digits / 10;
            int ones = digits % 10;

            fmt[1] = tens + '0';
            fmt[2] = ones + '0';
        } else {
            return "";
        }

        snprintf(hex, digits + 1, fmt, value);
        return std::string(hex);
    }
};
