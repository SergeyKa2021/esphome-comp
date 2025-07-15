#include "ibutton.h"

namespace esphome {
namespace ibutton {

void IButtonComponent::setup() {
  one_wire_ = new OneWire(pin_);
  pinMode(pin_, INPUT_PULLUP);
  if (write_button_ != nullptr) {
    write_button_->add_on_press_callback([this]() { 
      ESP_LOGI("ibutton", "Write button pressed");
      // Здесь будет логика записи
    });
  }
  ESP_LOGI("ibutton", "Initialized on pin %d", pin_);
}

}  // namespace ibutton
}  // namespace esphome
