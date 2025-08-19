#include "ds1990_key.h"
#include "esphome/core/log.h"
#include "esphome/components/one_wire/one_wire.h"

namespace esphome {
namespace ds1990_key {

static const char *const TAG = "ds1990_key";

void DS1990KeySensor::set_address(uint64_t address) { this->address_ = address; }

void DS1990KeySensor::set_one_wire(one_wire::OneWire *one_wire) { this->one_wire_ = one_wire; }

float DS1990KeySensor::get_setup_priority() const { return setup_priority::DATA; }

void DS1990KeySensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up DS1990 Key...");
  
  if (this->one_wire_ == nullptr) {
    ESP_LOGE(TAG, "OneWire bus not configured");
    this->mark_failed();
    return;
  }
  
  // Try to read the key to verify connection
  if (!this->read_key_data_()) {
    ESP_LOGW(TAG, "DS1990 key not found at address 0x%08X%08X", 
             (uint32_t)(this->address_ >> 32), (uint32_t)this->address_);
    this->found_ = false;
  } else {
    this->found_ = true;
    ESP_LOGI(TAG, "DS1990 key found at address 0x%08X%08X", 
             (uint32_t)(this->address_ >> 32), (uint32_t)this->address_);
  }
}

void DS1990KeySensor::update() {
  if (!this->found_) {
    // Try to rediscover the key
    if (this->read_key_data_()) {
      this->found_ = true;
      ESP_LOGI(TAG, "DS1990 key reconnected");
    } else {
      this->publish_state(NAN);
      return;
    }
  }
  
  if (!this->read_key_data_()) {
    ESP_LOGW(TAG, "Error reading DS1990 key data");
    this->publish_state(NAN);
    this->found_ = false;
    return;
  }
  
  // For DS1990, we publish the key as a numeric value (the address)
  // You might want to convert this to a different representation
  this->publish_state(static_cast<float>(this->address_));
}

bool DS1990KeySensor::read_key_data_() {
  if (this->one_wire_ == nullptr) {
    return false;
  }
  
  // Reset the bus
  if (!this->one_wire_->reset()) {
    return false;
  }
  
  // Select the specific device
  this->one_wire_->select(this->address_);
  
  // DS1990 doesn't have complex data reading like temperature sensors
  // We just need to verify the device responds and read its ROM code
  // The actual key data is the ROM code itself (64-bit lasered ROM)
  
  // For DS1990, we can simply verify the device responds to selection
  // and read back the ROM to confirm it matches
  
  uint8_t cmd = 0x33;  // Read ROM command
  this->one_wire_->write8(cmd);
  
  // Read back 8 bytes of ROM data
  uint64_t rom_code = 0;
  for (int i = 0; i < 8; i++) {
    uint8_t byte = this->one_wire_->read8();
    rom_code |= (static_cast<uint64_t>(byte) << (i * 8));
  }
  
  // Verify CRC (last byte is CRC of first 7 bytes)
  uint8_t crc = one_wire::crc8((uint8_t *)&rom_code, 7);
  if (crc != ((rom_code >> 56) & 0xFF)) {
    ESP_LOGW(TAG, "CRC check failed for DS1990 key");
    return false;
  }
  
  // Verify the read ROM matches our expected address
  if (rom_code != this->address_) {
    ESP_LOGW(TAG, "ROM code mismatch: expected 0x%08X%08X, got 0x%08X%08X",
             (uint32_t)(this->address_ >> 32), (uint32_t)this->address_,
             (uint32_t)(rom_code >> 32), (uint32_t)rom_code);
    return false;
  }
  
  return true;
}

void DS1990KeySensor::dump_config() {
  ESP_LOGCONFIG(TAG, "DS1990 Key:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%08X%08X", (uint32_t)(this->address_ >> 32), (uint32_t)this->address_);
  ESP_LOGCONFIG(TAG, "  Found: %s", YESNO(this->found_));
}

}  // namespace ds1990_key
}  // namespace esphome
