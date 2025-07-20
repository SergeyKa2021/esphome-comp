#pragma once

#include "esphome/core/component.h"
#include "esphome/components/dallas/dallas.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace dallas_key_reader {

class DallasKeyReader : public PollingComponent {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_dallas(dallas::DallasComponent *dallas) { dallas_ = dallas; }
  void set_key_sensor(text_sensor::TextSensor *sensor) { key_sensor_ = sensor; }

 protected:
  dallas::DallasComponent *dallas_;
  text_sensor::TextSensor *key_sensor_;
  uint64_t last_key_{0};
};

}  // namespace dallas_key_reader
}  // namespace esphome
