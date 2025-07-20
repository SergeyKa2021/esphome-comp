#pragma once
#include "esphome.h"
#include "OneWire.h"

namespace esphome {
namespace dallas_key {

class DallasKeyComponent : public Component {
 public:
  void set_pin(uint8_t pin) { pin_ = pin; }
  void setup() override;
  optional<uint64_t> read_id();
  std::string get_last_error() { return last_error_; }

 protected:
  uint8_t pin_;
  OneWire *one_wire_;
  std::string last_error_;
};

}  // namespace dallas_key
}  // namespace esphome
