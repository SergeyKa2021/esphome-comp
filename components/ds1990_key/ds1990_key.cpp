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
  // Убрали LOG_UPDATE_INTERVAL так как нет polling
  ESP_LOGCONFIG(TAG, "  Reading mode: On-demand/loop");
}

void DS1990KeySensor::loop() {
  // Читаем каждые 5 секунд (или настройте нужный интервал)
  uint32_t now = millis();
  if (now - this->last_read_ > 5000) {
    this->last_read_ = now;
    
    if (this->read_key_data_()) {
      // Успешное чтение - публикуем адрес в hex формате
      char buffer[17];
      snprintf(buffer, sizeof(buffer), "%016llX", this->address_);
      this->publish_state(buffer);
      ESP_LOGD(TAG, "Key read successfully: %s", buffer);
    } else {
      // Ошибка чтения - публикуем пустую строку
      this->publish_state("");
      ESP_LOGD(TAG, "Failed to read key");
    }
  }
}

bool DS1990KeySensor::read_key_data_() {
  uint8_t rom_code[8];
  
  // Сброс шины и проверка наличия устройств
  if (!this->one_wire_->reset_()) {
    ESP_LOGD(TAG, "No devices found on 1-Wire bus");
    return false;
  }

  // Пропускаем команду выбора устройства (SKIP_ROM = 0xCC)
  this->one_wire_->write8(0xCC);
  // Отправляем команду чтения ROM
  this->one_wire_->write8(READ_ROM);

  // Чтение 8 байт ROM-кода
  for (int i = 0; i < 8; i++) {
    rom_code[i] = this->one_wire_->read8();
  }

  // Проверка CRC (первые 7 байт)
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
