#pragma once

#include <string>

class Error {
 public:
    bool has_error() const { return !m_error_string.empty(); }
    const std::string & error_string() const { return m_error_string; }

    void clear_error() { m_error_string = ""; }

 protected:
    std::string m_error_string;
};
