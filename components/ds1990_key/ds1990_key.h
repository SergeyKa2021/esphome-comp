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

// Наследник OneWireDevice с публичными методами
class DS1990OneWire : public one_wire::OneWireDevice {
 public:
  using one_wire::OneWireDevice::OneWireDevice;
  
  bool public_reset() { return this->reset_(); }
  void public_reset_search() { this->reset_search(); }
  bool public_search(uint8_t *rom_code) { return this->search(rom_code); }
  void public_write8(uint8_t value) { this->write8(value); }
  uint8_t public_read8() { return this->read8(); }
  void public_skip() { this->write8(0xCC); } // SKIP_ROM
};

class DS1990KeySensor : public text_sensor::TextSensor, public Component {
 public:
  void set_one_wire(DS1990OneWire *one_wire) { one_wire_ = one_wire; }
  
  void setup() override;
  void dump_config() override;
  void loop() override;
  
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  bool read_key_data_();
  
  DS1990OneWire *one_wire_;
  uint64_t address_{0};
  uint32_t last_read_{0};
};

}  // namespace ds1990_key
}  // namespace esphome
