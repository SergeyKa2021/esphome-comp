#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/one_wire/one_wire.h"

namespace esphome {
namespace my_onewire_component {

class MyOneWireSensor : public Component, public sensor::Sensor {
public:
  explicit MyOneWireSensor(const std::string& name, one_wire::OneWireBus* bus)
      : sensor::Sensor(name), bus_(bus) {}

  void setup() override;
  void loop() override;

private:
  one_wire::OneWireBus* bus_;
};

}  // namespace my_onewire_component
}  // namespace esphome
