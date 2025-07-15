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

  void setup() override {
    one_wire_ = new OneWire(pin_);
    pinMode(pin_, INPUT_PULLUP);
    if (write_button_ != nullptr) {
      write_button_->add_on_press_callback([this]() { this->write_stored_key(); });
    }
    ESP_LOGI("ibutton", "Initialized on pin %d", pin_);
  }

 protected:
  uint8_t pin_;
  uint64_t key_id_{0};
  OneWire *one_wire_{nullptr};
  button::Button *write_button_{nullptr};
};

}  // namespace ibutton
}  // namespace esphome
