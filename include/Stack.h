#pragma once

#include <memory>

#include "Memory.h"
#include "Error.h"

class Stack: public Error {
public:
    static constexpr uint32_t start = Memory::size - 1;
    static constexpr uint32_t size = 16;
    static constexpr uint32_t end = start - size;

    Stack(const std::shared_ptr<Memory> & memory) : m_memory(memory) {}

    void push(uint16_t & stack_pointer, uint16_t value){
        if(stack_pointer - 1 < end){
            m_error_string = "Stack is full.";
            return;
        }

        m_memory->write(stack_pointer, value);
        stack_pointer--;
    }

    uint16_t pop(uint16_t & stack_pointer){
        if(stack_pointer + 1 > start){
            m_error_string = "Stack out of bounds.";
            return 0;
        }

        stack_pointer++;
        return m_memory->read(stack_pointer);
    }

private:
    std::shared_ptr<Memory> m_memory;
};
