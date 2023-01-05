#include "DS1821.h"

#include <math.h>


DS1821::DS1821(): OneWireItem(0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77)
{
    skip_multidrop = true;
}

void DS1821::duty(OneWireHub* const hub)
{
    uint8_t cmd, data;
    if (hub->recv(&cmd)) return; // error while reading command

    switch(cmd)
    {
    case 0x01: // WRITE TH
        if (hub->recv(&data)) return;
        t_high = data;
        break;

    case 0x02: // WRITE TL
        if (hub->recv(&data)) return;
        t_low = data;
        break;

    case 0xC0: // WRITE STATUS
        if (hub->recv(&data)) return;
        // operation mode and measurement mode change are not supported
        status = (status&~STATUS_SUPPORTED_CHANGE) | (data&STATUS_SUPPORTED_CHANGE);
        break;

    case 0x22: // STOP CONVERT T
        // nop. continous temperature measurement is not supported
        break;

    case 0x41: // LOAD COUNTER
        // NOTE: not available in continous measurement mode
        if (counter < 1)
            counter++;
        break;

    case 0xA0: // READ COUNTER
        uint16_t out;
        if (counter > 1) {
            out = 1;
        } else {
            out = counters[counter];
        }
        // NOTE: not available in continous measurement mode
        if (hub->send((uint8_t) out)) return;
        if (hub->send((uint8_t) out>>8)) return;
        break;

    case 0xA1: // READ TH
        if (hub->send(t_high)) return;
        break;

    case 0xA2: // READ TL
        if (hub->send(t_low)) return;
        break;

    case 0xAA: // READ TEMPERATURE
        if (hub->send(temperature)) return;
        break;

    case 0xAC: // READ STATUS
        if (hub->send(status)) return;
        break;

    case 0xEE: // START CONVERT T
        counter = 0; // reset couner to first one
        requested_new_measurement = true; // expose information, that we were requested to measure
        break;

    default:
        hub->raiseSlaveError(cmd);
        break;
    }
}

void DS1821::setTemperature(float value_degC)
{
    requested_new_measurement = false;
    temperature = (uint8_t) round(value_degC);
    // simulate precision of 2 decimal points

    // equal to 0 - +0.5°C;
    // equal to counter[1] - -0.5°C
    // ex: 32.17: 32 - 0.5 = 31.5, we need 0.67, that is (100 - 33) / 100 -> c[0] = 33, c[1] = 100
    // ex: 23.56: 24 - 0.5 = 23.5, we need 0.06, that is (100 - 94) / 100 -> c[0] = 94, c[1] = 100
    // 32.17: 50 - (3217 - 3200) = 50 - 17 = 33
    // 23:56: 50 - (2356 - 2400) = 50 + 44 = 94
    counters[0] = (uint8_t) (50 - ((int16_t)(value_degC*100) - (((int16_t) temperature)*100)));
    counters[1] = 100;

    if (temperature > t_high)
    {
        status |= STATUS_TH;
    }

    if (temperature < t_low)
    {
        status |= STATUS_TL;
    }
}

void DS1821::setTemperature(int8_t value_degC)
{
    requested_new_measurement = false;
    temperature = value_degC;
    counters[0] = 50; // +/- 0
    counters[1] = 100;
}

int8_t DS1821::getTemperature() const
{
    return temperature;
}

float DS1821::getTemperatureF() const
{
    return (float) temperature - 0.5 + (float) ((int16_t) counters[1] - counters[0]) / counters[1];
}
