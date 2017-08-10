#ifndef NEO_PATTERNS
#define NEO_PATTERNS

#include <Adafruit_NeoPixel.h>

// Pattern types supported:
enum pattern { NONE, RAINBOW_CYCLE, GLOW, FADE };
// Patern directions supported:
enum direction { FORWARD, REVERSE };

class NeoPatterns : public Adafruit_NeoPixel
{
  public:
  // Member Variables:  
  pattern  ActivePattern;  // which pattern is running
  direction Direction;     // direction to run the pattern
  
  unsigned long Interval;   // milliseconds between updates
  unsigned long lastUpdate; // last update of position
  
  uint32_t Color1, Color2;  // What colors are in use
  uint16_t TotalSteps;  // total number of steps in the pattern
  uint16_t Index;  // current step within the pattern
  
  void (*OnComplete)();  // Callback on completion of pattern
  
  // Constructor - calls base-class constructor to initialize strip
  NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)());
  void Update();
  void Increment();
  void Reverse();
  void RainbowCycle(uint8_t interval, direction dir = FORWARD);
  void RainbowCycleUpdate();
  void Glow(uint32_t color);
  void GlowUpdate();
  void Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval,
            direction dir = FORWARD);
  void FadeUpdate();
  void ColorSet(uint32_t color);
  uint8_t Red(uint32_t color);
  uint8_t Green(uint32_t color);
  uint8_t Blue(uint32_t color);
  uint32_t Wheel(byte WheelPos);
};

#endif

