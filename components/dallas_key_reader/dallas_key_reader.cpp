#include "dallas_key_reader.h"
#include "esphome/core/log.h"

namespace esphome {
namespace dallas_key_reader {

static const char *const TAG = "dallas_key_reader";

void DallasKeyReader::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Dallas Key Reader...");
  this->found_keys_.clear();
}

void DallasKeyReader::update() {
  std::vector<uint64_t> new_keys;
  this->one_wire_->reset_search();
  uint64_t address;

  while ((address = this->one_wire_->search()) != 0) {
    new_keys.push_back(address);
  }

  if (new_keys != this->found_keys_) {
    this->found_keys_ = new_keys;
    if (!this->found_keys_.empty()) {
      char buffer[20];
      snprintf(buffer, sizeof(buffer), "%016llX", this->found_keys_[0]);
      this->key_sensor_->publish_state(buffer);
    } else {
      this->key_sensor_->publish_state("None");
    }
  }
}

void DallasKeyReader::dump_config() {
  ESP_LOGCONFIG(TAG, "Dallas Key Reader:");
  if (this->found_keys_.empty()) {
    ESP_LOGCONFIG(TAG, "  No devices found");
  } else {
    for (auto &address : this->found_keys_) {
      ESP_LOGCONFIG(TAG, "  Found device: %016llX", address);
    }
  }
}

}  // namespace dallas_key_reader
}  // namespace esphome
