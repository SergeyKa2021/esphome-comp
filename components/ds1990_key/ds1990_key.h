#pragma once

#include "esphome.h"
#include "esphome/components/one_wire/one_wire.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace ds1990_key {

// Коды семейств DS1990
static const uint8_t DS1990A_FAMILY_CODE = 0x01;
static const uint8_t DS1990R_FAMILY_CODE = 0x81;
static const uint8_t READ_ROM = 0x33;

class DS1990KeySensor : public PollingComponent, public text_sensor::TextSensor, public one_wire::OneWireDevice {
 public:
  void setup() override;
  void dump_config() override;
  void update() override;
  void set_bus(one_wire::OneWireBus *bus) { bus_ = bus; }
  
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  bool read_key_data_();
  one_wire::OneWireBus *bus_;
  uint64_t address_{0};
};

}  // namespace ds1990_key
}  // namespace esphome
