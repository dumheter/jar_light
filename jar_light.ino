// ============================================================ //
// Include
// ============================================================ //

#include <math.h>
#include <stdlib.h>
#include "light_controller.hpp"
#include "battery_level.hpp"
#include "low_power.h"

// ============================================================ //
// Globals
// ============================================================ //

// LED
constexpr int led_count = 3;
constexpr int led_pin = 3;
Light_Controller light(led_count);

// Battery voltage
#define battery_level_pin A1

// Potentiomter
#define potentiometer_pin A0

// ============================================================ //
// Debug
//#define DEBUG

#ifdef DEBUG
#  define dprintln(...) Serial.println(__VA_ARGS__)
#  define dprint(...) Serial.print(__VA_ARGS__)
#else
#  define dprintln(...)
#  define dprint(...)
#endif

// ============================================================ //
//  Functions
// ============================================================ //


void fire()
{
  CRGB rgb{};
  constexpr int lo = 40;
  constexpr int hi = 100;

  int speed = rand() % 10 + 2;
  int green = rand() % 4 + 1;
  for (int i=lo; i<hi; i++) {
    rgb.r = constrain(10+i*3, 0, 255);
    rgb.g = i/green;
    rgb.b = 0;
    light.set_color(rgb);
    delay(i/speed);
  }

  speed = rand() % 7 + 1;
  for (int i=hi; i>lo; i--) {
    rgb.r = constrain(10+i*3, 0, 255);
    rgb.g = i/green;
    rgb.b = 0;
    light.set_color(rgb);
    delay(i/speed);
  }
}

void pulse()
{
  CRGB rgb{};
  for (int i=0; i<100; i++) {
    rgb.r = constrain(10+i*3, 0, 255);
    rgb.g = i/5;
    rgb.b = 0;
    light.set_color(rgb);
    delay(5+i);
  }
  delay(500);
}

void sleep_mode()
{
  if(light.get_on()) light.toggle();
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}

bool get_debounced_pot(int pin)
{
  static int current_val = -100;
  constexpr int debounce_val = 5;
  int diff = (analogRead(pin) - current_val);
  diff = diff < 0 ? diff * -1 : diff;

  if (diff > debounce_val) {
    current_val = analogRead(pin);
    return true;
  }
  return false;
}

CRGB get_color_from_pot(int pin)
{
  constexpr int red_low = 0;
  constexpr int red_high = 1023 / 3;
  constexpr int red_mid = red_high - (red_high - red_low) / 2;
  constexpr int green_low = red_mid + 10;
  constexpr int green_high = 2 * 1023 / 3;
  constexpr int green_mid = green_high - (green_high - green_low) / 2;
  constexpr int blue_low = green_mid + 10;
  constexpr int blue_high = 1023;
  constexpr int blue_mid = blue_high - (blue_high - blue_low) / 2;
  const int pot = analogRead(pin);
  CRGB color{};

  const auto calc_color_value = [](int lo, int hi, int val, bool reverse = false) {
    const int lo_limit = !reverse ? 0 : 255;
    const int hi_limit = !reverse ? 255 : 0;
    return ((val < lo || val > hi) ? 0 :
            map(val, lo, hi, lo_limit, hi_limit));
  };
  const auto calc_color = [calc_color_value](int lo, int hi, int mid, int val) {
    return ((val <= mid) ?
            calc_color_value(lo, mid+5, val) :
            calc_color_value(mid-5, hi, val, true)
            );
  };

  color.r = calc_color(red_low, red_high, red_mid, pot);
  color.g = calc_color(green_low, green_high, green_mid, pot);
  color.b = calc_color(blue_low, blue_high, blue_mid, pot);

  if (pot > blue_mid + 10) {
    color.r = calc_color(blue_mid+10, blue_high, blue_high, pot);
  }
  else if (pot < red_mid - 10) {
    color.b = calc_color_value(red_low, red_mid-10, pot, true);
  }

  dprint(color.r);
  dprint(",\t");
  dprint(color.g);
  dprint(",\t");
  dprint(color.b);
  dprint("\t");
  dprintln(pot);

  return color;
}

// ============================================================ //

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("\nbegin");
#endif

  battery_level_begin(battery_level_pin);
  srand(analogRead(battery_level_pin));

  delay(100);
  add_neopixel(light, led_count, led_pin);
  light.set_color(0xFF5500);
  delay(500);
}

// ============================================================ //

void loop()
{
#ifndef DEBUG
  if (is_battery_low(battery_level_pin)) {
    delay(100);
    while (is_battery_low(battery_level_pin)) {
      sleep_mode();
    }
  }
#endif

  static bool play_fire = false;
  if (get_debounced_pot(potentiometer_pin)) {
    if (analogRead(potentiometer_pin) < 10) {
      play_fire = true;
    }
    else {
      play_fire = false;
      light.set_color(get_color_from_pot(potentiometer_pin));
    }
    dprintln(get_battery_voltage(battery_level_pin));
  }

  if (play_fire) {
    fire();
  }

  LowPower.powerDown(SLEEP_15MS, ADC_OFF, BOD_OFF);
}
