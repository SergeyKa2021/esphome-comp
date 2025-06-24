#include "ibutton.h"
#include "esphome/core/log.h"

namespace esphome {
namespace my_onewire_component {

static const char* TAG = "my.onewire.component";

void MyOneWireSensor::setup() {
  ESP_LOGCONFIG(TAG, "Starting MyOneWireSensor");
  if (bus_ == nullptr) {
    ESP_LOGE(TAG, "Bus pointer is null!");
    return;
  }
}

void MyOneWireSensor::loop() {
  static unsigned long last_scan_time = millis();
  if ((millis() - last_scan_time) > 10000) { // сканируем раз в 10 секунд
    last_scan_time = millis();

    // Выполнение команды Skip ROM (0x0F)
    if (bus_->reset() && bus_->send_command(0x0F)) {
      ESP_LOGI(TAG, "Sent skip ROM command successfully!");

      // Далее идет какая-то полезная операция, например, конвертация температуры для DS18B20
      // bus_->send_command(0x44); // Example of another operation
    }
  }
}

}  // namespace my_onewire_component
}  // namespace esphome
