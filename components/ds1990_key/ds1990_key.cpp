#include "ds1990_key.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ds1990_key {

static const char *const TAG = "ds1990_key";

void DS1990KeySensor::dump_config() {
  LOG_TEXT_SENSOR("", "DS1990A/R Key", this);
  LOG_UPDATE_INTERVAL(this);
}

void DS1990KeySensor::update() {
  if (this->read_key_data_()) {
    // Успешное чтение - публикуем адрес как строку
    this->publish_state(std::to_string(this->address_));
  } else {
    // Ошибка чтения - публикуем пустую строку
    this->publish_state("");
  }
}

bool DS1990KeySensor::read_key_data_() {
  uint8_t rom_code[8];
  
  // Исправлен вызов reset_
  if (!this->one_wire_->reset_()) {
    ESP_LOGD(TAG, "No devices found");
    return false;
  }

  this->one_wire_->skip();
  this->one_wire_->write_bytes(READ_ROM);

  for (int i = 0; i < 8; i++) {
    rom_code[i] = this->one_wire_->read_bytes();
  }

  // Исправлен вызов crc8
  uint8_t crc = crc8((uint8_t *)&rom_code, 7);
  if (crc != rom_code[7]) {
    ESP_LOGD(TAG, "CRC check failed: %02X != %02X", crc, rom_code[7]);
    return false;
  }

  if (rom_code[0] != DS1990A_FAMILY_CODE && rom_code[0] != DS1990R_FAMILY_CODE) {
    ESP_LOGD(TAG, "Not a DS1990 device: %02X", rom_code[0]);
    return false;
  }

  this->address_ = 0;
  for (int i = 7; i >= 0; i--) {
    this->address_ = (this->address_ << 8) | rom_code[i];
  }

  ESP_LOGD(TAG, "Key found: %llX", this->address_);
  return true;
}

}  // namespace ds1990_key
}  // namespace esphome
}  // namespace ds1990_key
}  // namespace esphome
