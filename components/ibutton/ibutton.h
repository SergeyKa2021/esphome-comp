#pragma once
#include "esphome.h"
#include "OneWire.h"

namespace esphome {
namespace ibutton {

class IButtonComponent : public Component {
 public:
  void set_pin(uint8_t pin) { pin_ = pin; }
  void set_key_id(uint64_t id) { key_id_ = id; }
  void set_write_button(button::Button *button) { write_button_ = button; }

  void setup() override;
  
 protected:
  uint8_t pin_;
  uint64_t key_id_{0};
  OneWire *one_wire_{nullptr};
  button::Button *write_button_{nullptr};
};

}  // namespace ibutton
}  // namespace esphome
