#include "Bus.h"

void Bus::add(const std::shared_ptr<BusDevice> & device){
    m_devices.emplace_back(device);
}

uint16_t Bus::read(uint32_t address) const{
    for(auto & device : m_devices){
        if(address >= device->start_address() && address <= device->end_address()){
            return device->read(address);
        }
    }

    // FIXME: report error
    return 0;
}

void Bus::write(uint32_t address, uint16_t value){
    for(auto & device : m_devices){
        if(address >= device->start_address() && address <= device->end_address()){
            device->write(address, value);
            return;
        }
    }

    // FIXME: report error
}
