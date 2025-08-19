#include "ds1990_key.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ds1990_key {

static const char *const TAG = "ds1990_key";

void DS1990KeySensor::setup() {
  // Инициализация не требуется, OneWireBus уже инициализирован
  ESP_LOGD(TAG, "DS1990 Key sensor setup complete");
}

void DS1990KeySensor::dump_config() {
  LOG_TEXT_SENSOR("", "DS1990A/R Key", this);
  if (this->update_interval() == SCHEDULER_DONT_RUN) {
    ESP_LOGCONFIG(TAG, "  Update Interval: DISABLED");
  } else if (this->update_interval() < 100) {
    ESP_LOGCONFIG(TAG, "  Update Interval: %.3fs", this->update_interval() / 1000.0f);
  } else {
    ESP_LOGCONFIG(TAG, "  Update Interval: %.1fs", this->update_interval() / 1000.0f);
  }
}

void DS1990KeySensor::update() {
  if (this->read_key_data_()) {
    // Успешное чтение - публикуем адрес в hex формате
    char buffer[17];
    snprintf(buffer, sizeof(buffer), "%016llX", this->address_);
    this->publish_state(buffer);
  } else {
    // Ошибка чтения - публикуем пустую строку
    this->publish_state("");
    ESP_LOGD(TAG, "Failed to read key");
  }
}

bool DS1990KeySensor::read_key_data_() {
  uint8_t rom_code[8];
  
  // Используем публичный метод reset вместо защищенного reset_
  if (!this->one_wire_->reset_()) {
    ESP_LOGD(TAG, "No devices found on 1-Wire bus");
    return false;
  }

  // Используем правильные методы OneWireBus
  this->one_wire_->write8(0xCC);  // SKIP_ROM
  this->one_wire_->write8(READ_ROM);

  for (int i = 0; i < 8; i++) {
    rom_code[i] = this->one_wire_->read8();
  }

  // Проверка CRC
  uint8_t crc = esphome::crc8(rom_code, 7);
  if (crc != rom_code[7]) {
    ESP_LOGD(TAG, "CRC check failed: calculated %02X, received %02X", crc, rom_code[7]);
    return false;
  }

  // Проверка кода семейства
  if (rom_code[0] != DS1990A_FAMILY_CODE && rom_code[0] != DS1990R_FAMILY_CODE) {
    ESP_LOGD(TAG, "Not a DS1990 device: %02X", rom_code[0]);
    return false;
  }

  // Формирование 64-битного адреса (little-endian to big-endian)
  this->address_ = 0;
  for (int i = 0; i < 8; i++) {
    this->address_ |= (static_cast<uint64_t>(rom_code[i]) << (i * 8));
  }

  ESP_LOGD(TAG, "DS1990 key found: %016llX", this->address_);
  return true;
}

}  // namespace ds1990_key
}  // namespace esphome
