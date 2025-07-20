#include "dallas_key_reader.h"
#include "esphome/core/log.h"

namespace esphome {
namespace dallas_key_reader {

static const char *const TAG = "dallas_key_reader";

void DallasKeyReader::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Dallas Key Reader...");
}

void DallasKeyReader::update() {
  auto *wire = this->bus_->get_one_wire();
  if (wire == nullptr) {
    ESP_LOGE(TAG, "1-Wire bus not available");
    return;
  }

  uint64_t current_key;
  if (!wire->reset()) {
    ESP_LOGW(TAG, "No devices found or bus shorted");
    current_key = 0;
  } else {
    wire->skip();
    wire->write8(0x33);  // READ_ROM command
    current_key = wire->read64();
    
    // Verify CRC
    uint8_t crc = current_key & 0xFF;
    uint64_t data = current_key >> 8;
    uint8_t computed_crc = dallas::crc8((uint8_t *)&data, 7);
    if (crc != computed_crc) {
      ESP_LOGE(TAG, "CRC check failed! Received: 0x%02X, Computed: 0x%02X", crc, computed_crc);
      current_key = 0;
    }
  }

  if (current_key != this->last_key_) {
    this->last_key_ = current_key;
    if (current_key != 0) {
      char buffer[20];
      snprintf(buffer, sizeof(buffer), "%016llX", current_key);
      this->key_sensor_->publish_state(buffer);
      ESP_LOGD(TAG, "Found device with ROM: %016llX", current_key);
    } else {
      this->key_sensor_->publish_state("None");
    }
  }
}

void DallasKeyReader::dump_config() {
  ESP_LOGCONFIG(TAG, "Dallas Key Reader:");
  if (this->last_key_ != 0) {
    ESP_LOGCONFIG(TAG, "  Found device: %016llX", this->last_key_);
  } else {
    ESP_LOGCONFIG(TAG, "  No device found");
  }
}

}  // namespace dallas_key_reader
}  // namespace esphome
