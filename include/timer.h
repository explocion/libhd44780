#pragma once

#include "core.h"

namespace timer {

template <typename _Derived> class DurationOf {
public:
  using Duration = core::u64;
};

template <typename _Derived> class AsyncTimer {
public:
  CRTP_DERIVE(_Derived);
  using Duration = typename DurationOf<Derived>::Duration;

  auto after(Duration duration) -> void { this->derived().after(duration); }

  auto wait() const -> void { this->derived().wait(); }
};

class MicrosSystick;

template <> class DurationOf<MicrosSystick> {
public:
  using Duration = core::u64;
};

class MicrosSystick : public AsyncTimer<MicrosSystick> {
public:
  using Instant = core::u64;
  using Duration = typename DurationOf<MicrosSystick>::Duration;

protected:
  Instant next_instant = 0;

public:
  static inline auto instant() -> Instant { return micros(); }

  auto next() const -> Instant { return this->next_instant; }

  auto after(Duration duration) -> void {
    this->next_instant = instant() + duration;
  }

  auto wait() const -> void {
    auto current = this->instant();
    while (current < this->next_instant) {
      delayMicroseconds(this->next_instant - current);
      current = instant();
    }
  }
};

class MillisSystick;

template <> class DurationOf<MillisSystick> {
public:
  using Duration = core::u64;
};

class MillisSystick : public AsyncTimer<MillisSystick> {
public:
  using Instant = core::u64;
  using Duration = typename DurationOf<MillisSystick>::Duration;

protected:
  Instant next_instant = 0;

public:
  static inline auto instant() -> Instant { return millis(); }

  auto next() const -> Instant { return this->next_instant; }

  auto after(Duration duration) -> void {
    this->next_instant = instant() + duration;
  }

  auto wait() const -> void {
    auto current = instant();
    while (current < this->next_instant) {
      delay(this->next_instant - current);
      current = instant();
    }
  }
};

} // namespace timer