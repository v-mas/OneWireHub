#include <OneWireHub.h>
#include <DS1821.h>

auto hub = OneWireHub(D1);
auto ds1821 = DS1821();

float temperature = 37.13f;

void setup() {
  hub.attach(ds1821);
  ds1821.setTemperature(temperature);
}

void loop() {
  hub.poll();
  if (ds1821.requested_new_measurement) {
    temperature += 0.15f;
    ds1821.setTemperature(temperature);
  }
}
