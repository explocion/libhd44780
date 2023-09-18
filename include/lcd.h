#pragma once

#include "hd44780.h"
#include "timer.h"

template <hd44780::DataLength N>
class LCD : public hd44780::HD44780<timer::MicrosSystick, LCD<N>> {
public:
  static constexpr auto data_length() -> hd44780::DataLength { return N; }

  static constexpr auto bus_width() -> core::u8 {
    return data_length() == hd44780::DataLength::FourBits ? 4 : 8;
  }

protected:
  core::u8 register_selection;
  core::u8 enable;
  core::u8 data_bus[bus_width()];

  auto write_bus_now(core::u8 bits) -> void {
    for (core::u8 i = 0; i < bus_width(); i++)
      digitalWrite(this->data_bus[i], bitRead(bits, i));
  }

public:
  LCD(core::u8 RS, core::u8 EN, const core::u8 (&DB)[bus_width()])
      : register_selection(RS), enable(EN) {
    for (core::u8 i = 0; i < bus_width(); i++)
      this->data_bus[i] = DB[i];
  }

  LCD(const LCD<N> &) = delete;
  auto operator=(const LCD<N> &) -> LCD<N> & = delete;

  auto send_now(hd44780::OutputType type, u8 datum) -> void {
    digitalWrite(this->register_selection, type == hd44780::OutputType::Data);
    if (data_length() == hd44780::DataLength::FourBits) {
      digitalWrite(this->enable, HIGH);
      this->write_bus_now(datum >> 4);
      digitalWrite(this->enable, LOW);
      delayMicroseconds(1);
    }
    digitalWrite(this->enable, HIGH);
    this->write_bus_now(datum);
    digitalWrite(this->enable, LOW);
  }

  auto initialize() -> void {
    pinMode(this->register_selection, OUTPUT);
    pinMode(this->enable, OUTPUT);

    digitalWrite(this->register_selection, LOW);
    digitalWrite(this->enable, LOW);

    for (core::u8 i = 0; i < bus_width(); i++)
      pinMode(this->data_bus[i], OUTPUT);

    delay(50);
    digitalWrite(this->enable, HIGH);
    this->write_bus_now(0b00110011);
    digitalWrite(this->enable, LOW);
    delayMicroseconds(4500);
    digitalWrite(this->enable, HIGH);
    delayMicroseconds(1);
    digitalWrite(this->enable, LOW);
    delayMicroseconds(4500);
    digitalWrite(this->enable, HIGH);
    delayMicroseconds(1);
    digitalWrite(this->enable, LOW);
    delayMicroseconds(150);
    digitalWrite(this->enable, HIGH);
    delayMicroseconds(1);
    digitalWrite(this->enable, LOW);
    delayMicroseconds(50);

    if (data_length() == hd44780::DataLength::FourBits) {
      digitalWrite(this->enable, HIGH);
      this->write_bus_now(0b0010);
      digitalWrite(this->enable, LOW);
    }

    this->function_set(true, hd44780::Font::Dots5x8);
    this->set_display_options(true, false, false);
    this->clear();
    this->set_entry_mode(hd44780::Direction::Right, false);
  }
};