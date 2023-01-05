#ifndef ONEWIREHUB_DS1821_H
#define ONEWIREHUB_DS1821_H

#include "OneWireItem.h"

/**
 * Digital Thermometer
 * 
 * Does not have ROM (address) - does not support multidrop, can be only one device on the bus.
 * Can operate in thermostate mode, but is not supported by this simulated device
 * (in thermostate mode it does not communicate by 1-wire untill special sequence is received).
 * 
 * Tested on master (NodeMCU 1.0 <-> NodeMCU 1.0 ) written with OneWire library, functions: convert_t, read_temp, read_counter, load_counter.
 * The same master had properly read actual DS1821 device (NodeMCU 1.0 <-> DS1821).
 */
class DS1821 : public OneWireItem
{
// TODO: t_high, t_low, and 0x1f bits from status are stored in EEPROM in real device
private:
    uint8_t temperature;
    uint16_t counters[2];
    uint8_t counter = 0;

    static constexpr uint8_t  STATUS_TH         { 1<<4 };
    static constexpr uint8_t  STATUS_TL         { 1<<3 };
    static constexpr uint8_t  STATUS_POL        { 1<<1 };
    static constexpr uint8_t  STATUS_SUPPORTED_CHANGE = STATUS_TL | STATUS_TH | STATUS_POL;

    /**
     * 0b 1000 0000 - busy flag
     * 0b 0100 0000 - always 1
     * 0b 0010 0000 - non-volatile memory busy
     * 0b 0001 0000 - high temperature reached (at least once temperature was higher than t_high)
     * 0b 0000 1000 - low temperature reached (at least once temperature was lower than t_low)
     * 0b 0000 0100 - power-up operation mode (0 - one wire, 1 - thermostat)
     * 0b 0000 0010 - polarity of active state while in thermostat mode
     * 0b 0000 0001 - measurement type (0 - continous, 1 - one-shot)
     */
    uint8_t status = 0b00000001;

public:

    uint8_t t_high;
    uint8_t t_low;

    bool requested_new_measurement = true;

    DS1821(void);

    void duty(OneWireHub * hub) final;

    /** Supports -55°C to +125°C */
    void setTemperature(float value_degC); 
    /** Supports -55°C to +125°C */
    void setTemperature(int8_t value_degC);
    /** Supports -55°C to +125°C */
    int8_t getTemperature() const;
    /** Supports -55°C to +125°C */
    float getTemperatureF() const;

};

#endif
