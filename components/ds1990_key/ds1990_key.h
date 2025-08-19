#pragma once

#include "esphome/core/component.h"
#include "esphome/components/one_wire/one_wire.h"
#include "esphome/components/sensor/text_sensor.h"

namespace esphome {
namespace ds1990_key {

class DS1990KeySensor : public Component, public sensor::Sensor {
 public:
  void set_address(uint64_t address);
  void set_one_wire(one_wire::OneWire *one_wire);
  
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  
  void update() override;

 protected:
  one_wire::OneWire *one_wire_;
  uint64_t address_{0};
  bool found_{false};
  
  bool read_key_data_();
};

}  // namespace ds1990_key
}  // namespace esphome
