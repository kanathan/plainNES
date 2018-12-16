#include "io.h"

namespace IO
{

uint8_t controller_state[2];
uint8_t controller_shiftR[2];
bool controllerStrobe;

void init()
{
    controller_state[0] = 0;
    controller_state[1] = 0;
    controller_shiftR[0] = 0;
    controller_shiftR[1] = 0;
    controllerStrobe = false;
}

uint8_t regGet(uint16_t addr)
{
    if(addr == 0x4016) {
        if(controllerStrobe) {
            return controller_state[0] & 1;
        }
        else {
            uint8_t val = controller_shiftR[0] & 1;
            controller_shiftR[0] >>= 1;
            return val;
        }
    }
    if(addr == 0x4017) {
        if(controllerStrobe) {
            return controller_state[1] & 1;
        }
        else {
            uint8_t val = controller_shiftR[1] & 1;
            controller_shiftR[1] >>= 1;
            return val;
        }
    }
    return 0;
}

void regSet(uint16_t addr, uint8_t val)
{
    if(addr == 0x4016) {
        if((val & 1) > 0) {
            controllerStrobe = true;
        }
        else {
            controller_shiftR[0] = controller_state[0];
            controller_shiftR[1] = controller_state[1];
            controllerStrobe = false;
        }
    }
}

uint8_t* getControllerStatePtr()
{
    return controller_state;
}

} // IO