#include "NeoPatterns.h"

// Constructor - calls base-class constructor to initialize strip
NeoPatterns::NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
:Adafruit_NeoPixel(pixels, pin, type)
{
  OnComplete = callback;
}
  
  // Update the pattern
void NeoPatterns::Update()
{
  if((millis() - lastUpdate) > Interval) {
    lastUpdate = millis();
    switch(ActivePattern) {
      case RAINBOW_CYCLE:
        RainbowCycleUpdate();
        break;
      case GLOW:
        GlowUpdate();
        break;
      case FADE:
        FadeUpdate();
        break;
      default:
        break;
    }
  }
}
  
// Increment the Index and reset at the end
void NeoPatterns::Increment()
{
  if (Direction == FORWARD) {
    Index++;
    if (Index >= TotalSteps) {
      Index = 0;
      if (OnComplete)
        OnComplete(); // call the comlpetion callback
    }
  }
  else {// Direction == REVERSE
    --Index;
    if (Index <= 0) {
      Index = TotalSteps - 1;
      if (OnComplete)
        OnComplete(); // call the comlpetion callback
    }
  }
}

// Reverse pattern direction
void NeoPatterns::Reverse()
{
  if (Direction == FORWARD) {
    Direction = REVERSE;
    Index = TotalSteps - 1;
  }
  else {
    Direction = FORWARD;
    Index = 0;
  }
}

// Initialize for a RainbowCycle
void NeoPatterns::RainbowCycle(uint8_t interval, direction dir)
{
  ActivePattern = RAINBOW_CYCLE;
  Interval = interval;
  TotalSteps = 255;
  Index = 0;
  Direction = dir;
}

// Update the Rainbow Cycle Pattern
void NeoPatterns::RainbowCycleUpdate()
{
  ColorSet(Wheel((Index) & 255));
  show();
  Increment();
}

// Initialize for a GLOW
void NeoPatterns::Glow(uint32_t color)
{
  ActivePattern = GLOW;
  Color1 = color;
  ColorSet(color);
}

// Update the Glow Pattern
void NeoPatterns::GlowUpdate()
{}

// Initialize for a Fade
void NeoPatterns::Fade(uint32_t color1, uint32_t color2, uint16_t steps,
                       uint8_t interval, direction dir)
{
  ActivePattern = FADE;
  Interval = interval;
  TotalSteps = steps;
  Color1 = color1;
  Color2 = color2;
  Index = 0;
  Direction = dir;
}

// Update the Fade Pattern
void NeoPatterns::FadeUpdate()
{
  // Calculate linear interpolation between Color1 and Color2
  // Optimise order of operations to minimize truncation error
  uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
  uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
  uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;
  
  ColorSet(Color(red, green, blue));
  show();
  Increment();
}

// Set all pixels to a color (synchronously)
void NeoPatterns::ColorSet(uint32_t color)
{
  for (int i = 0; i < numPixels(); i++)
    setPixelColor(i, color);
  show();
}

// Returns the Red component of a 32-bit color
uint8_t NeoPatterns::Red(uint32_t color)
{
  return (color >> 16) & 0xFF;
}

// Returns the Green component of a 32-bit color
uint8_t NeoPatterns::Green(uint32_t color)
{
  return (color >> 8) & 0xFF;
}

// Returns the Blue component of a 32-bit color
uint8_t NeoPatterns::Blue(uint32_t color)
{
  return color & 0xFF;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t NeoPatterns::Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  else {
    WheelPos -= 170;
    return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

