#include "ds1990_key.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ds1990_key {

static const char *const TAG = "ds1990_key";

void DS1990KeySensor::setup() {
  ESP_LOGD(TAG, "DS1990 Key sensor setup complete");
  this->last_read_ = millis();
}

void DS1990KeySensor::dump_config() {
  LOG_TEXT_SENSOR("", "DS1990A/R Key", this);
  ESP_LOGCONFIG(TAG, "  Reading mode: On-demand/loop");
}

void DS1990KeySensor::loop() {
  uint32_t now = millis();
  if (now - this->last_read_ > 5000) {
    this->last_read_ = now;
    
    if (this->read_key_data_()) {
      char buffer[17];
      snprintf(buffer, sizeof(buffer), "%016llX", this->address_);
      this->publish_state(buffer);
      ESP_LOGD(TAG, "Key read successfully: %s", buffer);
    } else {
      this->publish_state("");
      ESP_LOGD(TAG, "Failed to read key");
    }
  }
}

bool DS1990KeySensor::read_key_data_() {
  uint8_t rom_code[8];
  
  // Используем публичные методы
  if (!this->one_wire_->public_reset()) {
    ESP_LOGD(TAG, "No devices found on 1-Wire bus");
    return false;
  }

  // Пропускаем выбор устройства
  this->one_wire_->public_skip();
  // Отправляем команду чтения ROM
  this->one_wire_->public_write8(READ_ROM);

  // Чтение 8 байт ROM-кода
  for (int i = 0; i < 8; i++) {
    rom_code[i] = this->one_wire_->public_read8();
  }

  // Проверка CRC
  uint8_t crc = esphome::crc8(rom_code, 7);
  if (crc != rom_code[7]) {
    ESP_LOGD(TAG, "CRC check failed: calculated %02X, received %02X", crc, rom_code[7]);
    return false;
  }

  // Проверка кода семейства
  if (rom_code[0] != DS1990A_FAMILY_CODE && rom_code[0] != DS1990R_FAMILY_CODE) {
    ESP_LOGD(TAG, "Not a DS1990 device. Family code: %02X", rom_code[0]);
    return false;
  }

  // Формирование 64-битного адреса
  this->address_ = 0;
  for (int i = 0; i < 8; i++) {
    this->address_ |= (static_cast<uint64_t>(rom_code[i]) << (i * 8));
  }

  ESP_LOGD(TAG, "DS1990 key found: %016llX", this->address_);
  return true;
}

}  // namespace ds1990_key
}  // namespace esphome
