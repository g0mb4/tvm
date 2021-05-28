#pragma once

#include <string>

class Display {
public:
    static constexpr uint32_t address = 3000;

    void reset() { m_message = ""; }
    void add_character(char c) { m_message += c; }

    const std::string & message() const { return m_message; }
private:
    std::string m_message;
};
