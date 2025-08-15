#include "dallas_key_reader.h"
#include "esphome/core/log.h"

namespace esphome {
namespace dallas_key_reader {

static const char *const TAG = "dallas_key_reader";

uint8_t DallasKeyReader::crc8(const uint8_t *addr, uint8_t len) {
  uint8_t crc = 0;
  while (len--) {
    uint8_t inbyte = *addr++;
    for (uint8_t i = 8; i; i--) {
      uint8_t mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix) crc ^= 0x8C;
      inbyte >>= 1;
    }
  }
  return crc;
}

void DallasKeyReader::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Dallas Key Reader...");
}

void DallasKeyReader::update() {
  if (this->one_wire_ == nullptr) {
    ESP_LOGE(TAG, "OneWire bus not set!");
    return;
  }

  uint64_t current_key = 0;
  if (this->one_wire_->reset()) {
    this->one_wire_->write8(0x33);  // READ_ROM command
    current_key = this->one_wire_->read64();
    
    // Verify CRC
    uint8_t crc = current_key & 0xFF;
    uint64_t data = current_key >> 8;
    uint8_t computed_crc = this->crc8((uint8_t *)&data, 7);
    
    if (crc != computed_crc) {
      ESP_LOGE(TAG, "CRC check failed! Received: 0x%02X, Computed: 0x%02X", crc, computed_crc);
      current_key = 0;
    }
  } else {
    ESP_LOGW(TAG, "No devices found or bus shorted");
  }

  if (current_key != this->last_key_) {
    this->last_key_ = current_key;
    char buffer[17];
    snprintf(buffer, sizeof(buffer), current_key ? "%016llX" : "None", current_key);
    this->key_sensor_->publish_state(buffer);
    ESP_LOGD(TAG, "Device ROM: %s", buffer);
  }
}

void DallasKeyReader::dump_config() {
  ESP_LOGCONFIG(TAG, "Dallas Key Reader:");
  ESP_LOGCONFIG(TAG, "  Found device: %s", this->last_key_ ? "0x%016llX" : "None", this->last_key_);
}

}  // namespace dallas_key_reader
}  // namespace esphome
