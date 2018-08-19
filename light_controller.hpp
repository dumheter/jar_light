/**
 * Note, this is an altered version of the light controller that does not rely
 * on the c++ stdlib.
 *
 * light_controller
 *
 * Contains information about one led unit. Can contain multiple LEDs
 * all connected to one data pin.
 */

#pragma once

// ============================================================ //
// Headers
// ============================================================ //

// FastLED
//#define FASTLED_ESP8266_RAW_PIN_ORDER //IOX -> X
//#define FASTLED_ALLOW_INTERRUPTS 0
//#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include "FastLED.h"

// stdlib
#include <Arduino.h>
#include <stdint.h>

// ============================================================ //
// Macro
// ============================================================ //

/**
 * Simplifies the process of adding a light.
 * @param controller {std::vector<Light_Controller>} Holds the lights
 * @param led_count {int}
 * @param pin {int}
 */
#define add_neopixel_vector(vector_controller, led_count, pin)          \
  vector_controller.emplace_back(led_count);                            \
  FastLED.addLeds<NEOPIXEL, pin>(vector_controller.back().get_buffer(), led_count);
#define add_neopixel(controller, led_count, pin)                        \
  FastLED.addLeds<WS2812B, pin, GRB>(controller.get_buffer(), led_count);

// #define add_light(controller, light_count, pin)                      \
//   controller.emplace_back(light_count);                              \
//   FastLED.addLeds<WS2812B, pin, GRB>(controller.back().get_buffer(), light_count);

// ============================================================ //
// Class Implementation
// ============================================================ //

class Light_Controller {

public:

  /**
   * Initialize the pin and
   */
  Light_Controller (int num_leds);

  ~Light_Controller ();

  /**
   * @param color 0xRRGGBB00 where 00 are unused
   */
  void set_color (uint32_t color);

  /**
   * @param color CRGB
   */
  void set_color (const CRGB color);

  /**
   * Will set the color by starting with a low light then
   * work its way up to the color.
   *
   * Will block for time.
   */
  void fade_to_color (uint32_t color, uint32_t time);

  /**
   * @return color of the first LED
   */
  CRGB get_color ();

  /**
   * @return member m_last_color
   */
  uint32_t get_last_color () { return m_last_color; };

  /**
   * @return 0 if off
   * @return 1 if on
   */
  bool get_on () { return m_on; };

/**
 * Toggle the lights, on -> off or off -> on.
 */
  void toggle ();

  /**
   * Blink the light. Will block for the duration of time_ms
   */
  void blink (unsigned long time_ms);

  void flash (uint32_t color, unsigned long time_ms=50,
              unsigned long repetitions=3, unsigned long repeat_delay=0);

  /** Array operator overload for m_color **/
  CRGB& operator[] (int i) { return m_color[i]; };
  const CRGB& operator[] (int i) const { return m_color[i]; };

  CRGB* get_buffer () { return m_color; };

private:

  // Color buffer that FastLED will use
  int m_num_leds;
  CRGB* m_color;
  uint32_t m_last_color;
  bool m_on;

  /**
   * Do the acuall action of setting the color
   */
  void set_color_raw(uint32_t color);
  void set_color_raw(const CRGB color);

};
