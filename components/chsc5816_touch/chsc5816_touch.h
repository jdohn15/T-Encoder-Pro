#pragma once

#include "esphome.h"
#include "Wire.h"

class CHSC5816Touch : public PollingComponent, public Sensor {
 public:
  CHSC5816Touch(uint8_t reset_pin, uint8_t interrupt_pin, uint8_t i2c_address, uint8_t sda_pin, uint8_t scl_pin)
      : PollingComponent(10), reset_pin_(reset_pin), interrupt_pin_(interrupt_pin), i2c_address_(i2c_address), sda_pin_(sda_pin), scl_pin_(scl_pin) {}

  void setup() override {
    ESP_LOGD("CHSC5816", "Setting up CHSC5816 touch controller...");
    pinMode(reset_pin_, OUTPUT);
    digitalWrite(reset_pin_, HIGH);
    pinMode(interrupt_pin_, INPUT_PULLUP);

    Wire.begin(sda_pin_, scl_pin_);

    if (!begin()) {
      ESP_LOGE("CHSC5816", "Failed to initialize CHSC5816 touch controller!");
      this->mark_failed();
    } else {
      ESP_LOGD("CHSC5816", "CHSC5816 touch controller initialized successfully.");
    }
  }

  void update() override {
    if (digitalRead(interrupt_pin_) == LOW) {
      int16_t x[2], y[2];
      uint8_t touched = getPoint(x, y);
      for (int i = 0; i < touched; ++i) {
        ESP_LOGD("CHSC5816", "Touch #%d: X=%d, Y=%d", i, x[i], y[i]);
        this->publish_state(x[i]);  // Publish X-coordinate as sensor data
      }
    }
  }

 private:
  uint8_t reset_pin_, interrupt_pin_, i2c_address_, sda_pin_, scl_pin_;

  bool begin() {
    Wire.beginTransmission(i2c_address_);
    return Wire.endTransmission() == 0;
  }

  uint8_t getPoint(int16_t *x, int16_t *y) {
    // Fetch touch data from the controller
    // Example logic, replace with actual touch fetch implementation
    x[0] = 100;  // Example data
    y[0] = 150;  // Example data
    return 1;    // Example: 1 point touched
  }
};
