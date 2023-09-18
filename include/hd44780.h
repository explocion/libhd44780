#pragma once

#include "core.h"
#include "timer.h"

namespace hd44780 {

enum class OutputType { Instruction, Data };

enum class DataLength { FourBits = 4, EightBits = 8 };

enum class Direction { Left, Right };

enum class Font { Dots5x8, Dots5x10 };

enum class ShiftOption { Cursor, Display };

static constexpr core::u8 CURSOR_ADDR_LINE_2 = 0x40;

// CRTP Trait for a HD44780 LCD
template <typename _Timer, typename _Derived> class HD44780 : public Print {
public:
  using Timer = _Timer;
  CRTP_DERIVE(_Derived);

protected:
  Timer async_timer;

  auto timer() -> timer::AsyncTimer<Timer> & {
    return static_cast<timer::AsyncTimer<Timer> &>(this->async_timer);
  }

  auto timer() const -> const timer::AsyncTimer<Timer> & {
    return static_cast<const timer::AsyncTimer<Timer> &>(this->async_timer);
  }

public:
  static constexpr auto data_length() -> DataLength {
    return Derived::data_length();
  }

  auto send(OutputType type, core::u8 datum, core::u16 duration) -> void {
    this->timer().wait();
    this->derived().send_now(type, datum);
    this->timer().after(duration);
  }

  inline auto clear() -> void {
    this->send(OutputType::Instruction, 0x01, 1520);
  }

  inline auto return_home() -> void {
    this->send(OutputType::Instruction, 0x02, 1520);
  }

  auto set_entry_mode(Direction direction, bool shift) -> void {
    u8 datum = 0x04 | (direction == Direction::Right ? 0x02 : 0x00) |
               (shift ? 0x01 : 0x00);
    this->send(OutputType::Instruction, datum, 37);
  }

  auto set_display_options(bool display_on, bool cursor_on, bool cursor_blink)
      -> void {
    u8 datum = 0x08 | (display_on ? 0x04 : 0x00) | (cursor_on ? 0x02 : 0x00) |
               (cursor_blink ? 0x01 : 0x00);
    this->send(OutputType::Instruction, datum, 37);
  }

  auto shift(ShiftOption option, Direction direction) -> void {
    u8 datum = 0x10 | (option == ShiftOption::Display ? 0x08 : 0x00) |
               (direction == Direction::Right ? 0x04 : 0x00);
    this->send(OutputType::Instruction, datum, 37);
  }

  auto function_set(bool snd_on, Font font) -> void {
    u8 datum = 0x20 | (data_length() == DataLength::EightBits ? 0x10 : 0x00) |
               (snd_on ? 0x08 : 0x00) | (font == Font::Dots5x10 ? 0x04 : 0x00);
    this->send(OutputType::Instruction, datum, 37);
  }

  inline auto set_character_generation_addr(u8 addr) -> void {
    this->send(OutputType::Instruction, 0x40 | (addr & 0x3F), 41);
  }

  inline auto set_cursor_addr(u8 addr) -> void {
    this->send(OutputType::Instruction, 0x80 | addr, 41);
  }

  inline auto write(u8 datum) -> core::usize {
    this->send(OutputType::Data, datum, 41);
    return 1;
  }
};

} // namespace hd44780