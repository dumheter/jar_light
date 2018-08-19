// ============================================================ //
// Headers
// ============================================================ //

#include "light_controller.hpp"
#include <math.h>

// ============================================================ //
// Class Implementation
// ============================================================ //

Light_Controller::Light_Controller(int num_leds)
  : m_num_leds(num_leds), m_color(new CRGB[num_leds])
{
  m_last_color = 0;
}

Light_Controller::~Light_Controller()
{
  delete m_color;
}

void Light_Controller::set_color(uint32_t color)
{
  if (!color) {
    if (!m_on)
      return;

    else
      toggle();
  }

  else {
    m_last_color = color;
    m_on = true;
    set_color_raw(color);
  }
}

void Light_Controller::set_color(const CRGB color)
{
  if (!color) {
    if (!m_on)
      return;

    else
      toggle();
  }

  else {
    m_last_color = color;
    m_on = true;
    set_color_raw(color);
  }
}

void Light_Controller::fade_to_color(uint32_t color, uint32_t time)
{
  CRGB new_color = CRGB(color);
  CRGB old_color = get_color();
  CRGB tmp_color{};

  float steps = time / 150;
  const auto step_col = [](int i, uint8_t oldc, uint8_t newc, float steps) {
    const float diff = oldc - newc;
    const float retc = oldc - diff / steps * i;
    return constrain(lround(retc), 0, 255);
  };

  for (int i=0; i<steps; i++) {
    tmp_color.r = step_col(i, old_color.r, new_color.r, steps);
    tmp_color.g = step_col(i, old_color.g, new_color.g, steps);
    tmp_color.b = step_col(i, old_color.b, new_color.b, steps);

    set_color_raw(tmp_color);
    delay(lround(steps));
  }

  set_color(new_color);
}

CRGB Light_Controller::get_color()
{
  if (m_num_leds < 1 || !m_on)
    return CRGB(static_cast<uint32_t>(0));

  else
      return CRGB(m_color[0]);
  }

void Light_Controller::toggle()
{
  if (m_on) {
    m_on = false;
    set_color_raw(0x000000);
  }

  else {
    m_on = true;
    set_color_raw(m_last_color);
  }
}

void Light_Controller::blink(unsigned long time_ms)
{
  toggle();
  delay(time_ms);
  toggle();
}

void Light_Controller::flash(uint32_t color, unsigned long time_ms,
                             unsigned long repetitions,
                              unsigned long repeat_delay)
{
  while (repetitions-- > 0) {
    set_color_raw(color);
    delay(time_ms);
    set_color_raw(m_last_color);

    if (repeat_delay == 0)
      delay(time_ms);
    else
      delay(repeat_delay);
  }
}

void Light_Controller::set_color_raw(uint32_t color)
{
  CRGB crgb_color = CRGB(color);

  for (uint8_t i=0; i<m_num_leds; i++) {
    m_color[i] = crgb_color;
  }

  FastLED.show();
}

void Light_Controller::set_color_raw(const CRGB crgb_color)
{
  for (uint8_t i=0; i<m_num_leds; i++) {
    m_color[i] = crgb_color;
  }

  FastLED.show();
}
