#pragma once
#include "esphome.h"
#include "OneWire.h"

namespace esphome {
namespace ibutton {

static const char *const TAG = "ibutton";

enum KeyType {
  KEY_TYPE_UNKNOWN = 0,
  KEY_TYPE_RW1990_1,  // Однократно программируемый
  KEY_TYPE_RW1990_2   // Перезаписываемый
};

class IButton : public Component {
 public:
  void set_pin(uint8_t pin) { pin_ = pin; }
  void set_key_id(uint64_t id) { key_id_ = id; }
  void set_write_button(button::Button *button) { write_button_ = button; }

  void setup() override {
    one_wire_ = new OneWire(pin_);
    pinMode(pin_, INPUT_PULLUP);
    
    if (write_button_) {
      write_button_->add_on_press_callback([this]() { this->write_stored_key(); });
    }
    
    ESP_LOGI(TAG, "iButton initialized on pin %d", pin_);
  }

  void write_stored_key() {
    if (key_id_ == 0) {
      ESP_LOGE(TAG, "No key ID configured");
      return;
    }

    if (write_key(key_id_)) {
      ESP_LOGI(TAG, "Successfully wrote key: %016llX", key_id_);
    } else {
      ESP_LOGE(TAG, "Failed to write key: %s", last_error_.c_str());
    }
  }

  KeyType determine_key_type() {
    if (!one_wire_->reset()) return KEY_TYPE_UNKNOWN;

    uint8_t addr[8];
    one_wire_->write(0x33); // READ_ROM
    for (uint8_t i = 0; i < 8; i++) {
      addr[i] = one_wire_->read();
    }

    if (OneWire::crc8(addr, 7) != addr[7]) return KEY_TYPE_UNKNOWN;

    if ((addr[2] & 0xF0) == 0xF0 && (addr[3] & 0x0F) == 0x00) {
      return KEY_TYPE_RW1990_2;
    }
    return is_blank_key(addr) ? KEY_TYPE_RW1990_1 : KEY_TYPE_RW1990_2;
  }

  optional<uint64_t> read_id() {
    uint8_t addr[8];
    if (!one_wire_->reset()) {
      last_error_ = "Reset failed";
      return {};
    }

    one_wire_->write(0x33);
    for (uint8_t i = 0; i < 8; i++) {
      addr[i] = one_wire_->read();
    }

    if (OneWire::crc8(addr, 7) != addr[7]) {
      last_error_ = "CRC error";
      return {};
    }

    return bytes_to_uint64(addr);
  }

  bool write_key(uint64_t new_id) {
    uint8_t new_bytes[8];
    uint64_to_bytes(new_id, new_bytes);

    if (new_bytes[0] != 0x01) {
      last_error_ = "Invalid family code";
      return false;
    }

    auto key_type = determine_key_type();
    switch(key_type) {
      case KEY_TYPE_RW1990_1:
        return write_with_protocol(0xD1, 0xB5, new_bytes, 1500);
      case KEY_TYPE_RW1990_2:
        return write_with_protocol(0x1D, 0x1E, new_bytes, 1200);
      default:
        last_error_ = "Unsupported key type";
        return false;
    }
  }

  const std::string& get_last_error() const { return last_error_; }

 protected:
  static constexpr const char* TAG = "ibutton";
  uint8_t pin_;
  uint64_t key_id_ = 0;
  OneWire *one_wire_;
  button::Button *write_button_ = nullptr;
  std::string last_error_;

  bool is_blank_key(const uint8_t addr[8]) {
    for (uint8_t i = 0; i < 8; i++) {
      if (addr[i] != 0xFF) return false;
    }
    return true;
  }

  bool write_with_protocol(uint8_t write_cmd, uint8_t flag_cmd, 
                         uint8_t* data, uint16_t delay_us) {
    if (!one_wire_->reset()) {
      last_error_ = "Reset failed";
      return false;
    }

    one_wire_->write(write_cmd);
    one_wire_->write_bit(1);
    for (uint8_t i = 0; i < 8; i++) {
      one_wire_->write(data[i]);
      delayMicroseconds(delay_us);
    }

    if (!check_write_flag(flag_cmd, 0xFE)) {
      last_error_ = "Flag check failed";
      return false;
    }

    return verify_write(data);
  }

  bool check_write_flag(uint8_t cmd, uint8_t expected) {
    if (!one_wire_->reset()) return false;
    one_wire_->write(cmd);
    return (one_wire_->read() == expected);
  }

  bool verify_write(uint8_t written_bytes[8]) {
    auto written_id = read_id();
    if (!written_id.has_value()) return false;
    return (written_id.value() == bytes_to_uint64(written_bytes));
  }

  uint64_t bytes_to_uint64(const uint8_t bytes[8]) {
    uint64_t value = 0;
    for (uint8_t i = 0; i < 8; i++) {
      value |= (uint64_t)bytes[i] << (i * 8);
    }
    return value;
  }

  void uint64_to_bytes(uint64_t value, uint8_t bytes[8]) {
    for (uint8_t i = 0; i < 8; i++) {
      bytes[i] = (value >> (i * 8)) & 0xFF;
    }
  }
};

void IButton::setup() {
  one_wire_ = new OneWire(pin_);
  pinMode(pin_, INPUT_PULLUP);
  ESP_LOGI(TAG, "iButton initialized on pin %d", pin_);
}

// Добавляем схему конфигурации
auto CONFIG_SCHEMA = esphome::config_schema::Schema({
    {"pin", esphome::config_schema::Required<int>()},
    {"key_id", esphome::config_schema::Optional<uint64_t>()}
});

} // namespace ibutton
} // namespace esphome
