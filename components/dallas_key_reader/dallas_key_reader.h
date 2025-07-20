#pragma once

#include "esphome/core/component.h"
#include "esphome/components/one_wire/one_wire.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace dallas_key_reader {

class DallasKeyReader : public PollingComponent {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_one_wire(one_wire::OneWire *one_wire) { one_wire_ = one_wire; }
  void set_key_sensor(text_sensor::TextSensor *sensor) { key_sensor_ = sensor; }

 protected:
  one_wire::OneWire *one_wire_;
  text_sensor::TextSensor *key_sensor_;
  std::vector<uint64_t> found_keys_;
};

}  // namespace dallas_key_reader
}  // namespace esphome
