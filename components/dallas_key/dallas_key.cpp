#include "dallas_key.h"

namespace esphome {
namespace dallas_key {

void DallasKeyComponent::setup() {
  one_wire_ = new OneWire(pin_);
  pinMode(pin_, INPUT_PULLUP);
  ESP_LOGI("dallas_key", "Initialized on pin %d", pin_);
}

optional<uint64_t> DallasKeyComponent::read_id() {
  uint8_t addr[8];
  
  if (!one_wire_->reset()) {
    last_error_ = "No device detected";
    return {};
  }

  one_wire_->write(0x33); // Read ROM command
  for (uint8_t i = 0; i < 8; i++) {
    addr[i] = one_wire_->read();
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    last_error_ = "CRC error";
    return {};
  }

  uint64_t id = 0;
  for (uint8_t i = 0; i < 8; i++) {
    id |= (uint64_t)addr[i] << (i * 8);
  }

  last_error_ = "";
  return id;
}

}  // namespace dallas_key
}  // namespace esphome
