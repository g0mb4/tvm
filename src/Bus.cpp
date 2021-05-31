#include "Bus.h"

void Bus::add(const std::shared_ptr<BusDevice> & device){
    m_devices.emplace_back(device);
}

uint16_t Bus::read(uint32_t address){
    for(auto & device : m_devices){
        if(address >= device->start_address() && address <= device->end_address()){
            return device->read(address);
        }
    }

    m_error_string = "No device is registered at address";
    return 0;
}

void Bus::write(uint32_t address, uint16_t value){
    for(auto & device : m_devices){
        if(address >= device->start_address() && address <= device->end_address()){
            device->write(address, value);
            return;
        }
    }

     m_error_string = "No device is registered at address";
}
