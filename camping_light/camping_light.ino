// This is a demonstration on how to use an input device to trigger changes on your neo pixels.
// You should wire a momentary push button to connect from ground to a digital IO pin.  When you
// press the button it will change to a new pixel animation.  Note that you need to press the
// button once to start the first animation!

#include <Adafruit_NeoPixel.h>

#define BUTTON_1_PIN 11 
#define BUTTON_2_PIN 10
#define POT_PIN A2
#define PIXEL_PIN A0
#define PIXEL_COUNT 60
#define TAU 2.0 * 3.1415926



// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
bool oldState1 = HIGH;
bool oldState2 = HIGH;
int showType = 0;
uint16_t brightnesses[32];
void setup() {
  pinMode(BUTTON_1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_2_PIN, INPUT_PULLUP);
  pinMode(9,OUTPUT);
  digitalWrite(9, LOW);
  pinMode(A1,OUTPUT);
  digitalWrite(A1, LOW);
  Serial.begin(115200);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  static uint16_t count1 = 0, count2 = 0;
  bool newState = digitalRead(BUTTON_1_PIN);
  startShow(showType);
  // Check if state changed from high to low (button press).
  if (newState == LOW && oldState1 == LOW) {
      count1++;
  }
  if (newState == HIGH && oldState1 == LOW) {
    if (count1 > 5 && showType) {
        showType++;
      if (showType > 6)
        showType = 1;
    }
    count1 = 0;
  }
  // Set the last button state to the old state.
  oldState1 = newState;
  
  newState = digitalRead(BUTTON_2_PIN);
    // Check if state changed from high to low (button press).
  if (newState == LOW && oldState2 == LOW) {
    count2++;
    if (count2 == 20) {
      showType = !showType;
    }
  }
  if (newState == HIGH && oldState2 == LOW) {
    count2 = 0;
  }
  // Set the last button state to the old state.
  oldState2 = newState;
}


void startShow(int i) {
  if (i == 0) {
    solidLight(strip.Color(0,0,0),10);
  } else if (i == 1) {
    solidLight(strip.Color(255,255,255),10);
  } else if (i == 2) {
    solidLight(strip.Color(255,0,0),10);
  } else if (i == 3) {
    sosLight(10);
  } else if (i == 4) {
    colorWipe(10);
  } else if (i == 5) {
    rainbowCycle(3);
  } else if (i == 6) {
    sinBreath(strip.Color(0,0,255), 5);
  }
}

void sinBreath(uint32_t c, uint8_t wait) {
  static uint16_t i; 
  double intensity = (1.5 + cos((TAU * (i))/256.0))/2.5;
  intensity *= intensity;
  for (uint8_t j = 0; j < PIXEL_COUNT; j++){
    strip.setPixelColor(j, (c >> 16 & 0xFF) * intensity, (c >> 8 & 0xFF) * intensity, (c & 0xFF) * intensity); 
  }
  UpdateBrightness();
  strip.show();
  i = (i+1) % 256;
  delay(wait);   
}

// Fill the dots one after the other with a color
void colorWipe(uint8_t wait) {
  static uint8_t i, j; 
  static uint32_t c = Wheel(255 * j / 16);
  UpdateBrightness();
  strip.setPixelColor(i, c);
  strip.show();
  i++;
  if (i >= PIXEL_COUNT) {
    i = 0;
    j = (j+1) & 0b1111;
    c = Wheel(255 * j / 16);
  }
  delay(wait);
}

void sosLight(uint8_t wait) {
  static uint16_t count = 0;
  uint32_t c = 0;
  if(count < 360) {
    if(((count % 40 < 20) && (count > 240)) || ((count % 80 < 40) && (count < 240)) ) {
      c = strip.Color(255,255,255);
    }
  }
  for (uint8_t i= 0; i < PIXEL_COUNT; i++){
    strip.setPixelColor(i,c);
  }
  
  UpdateBrightness();
  strip.show();
  count++;
  if (count > 399){
    count = 0;
  } 
  delay(wait); 
}

void solidLight(uint32_t c, uint8_t wait) {
  for (uint8_t i= 0; i < PIXEL_COUNT; i++){
    strip.setPixelColor(i, c);
  }
  UpdateBrightness();
  strip.show();
  delay(wait); 
}



void sinWave(uint8_t node, uint8_t wait) {
  static uint8_t i, k; 
  static uint32_t c = Wheel(255 * k / 15);
  for (uint8_t j = 0; j < PIXEL_COUNT; j++){
    double intensity = (1 + cos((node * TAU * (j+i))/PIXEL_COUNT))/2.0;
    strip.setPixelColor(j, (c >> 16 & 0xFF) * intensity, (c >> 8 & 0xFF) * intensity, (c & 0xFF) * intensity); 
  }
  UpdateBrightness();
  strip.show();
  i++;
  if (i >= PIXEL_COUNT) {
    i = 0;
    k = (k+1) & 0b1111;
  }
  delay(wait);   
}

void UpdateBrightness() {
  static uint8_t counter = 0;
  static uint32_t sum = 0;
  sum -= brightnesses[counter];
  brightnesses[counter] = analogRead(A2);
  sum += brightnesses[counter];
  counter = (counter + 1) % 32;
  uint16_t brightness = min(255, sum / 32 / 4 + 10);
  strip.setBrightness(brightness);
}

void rainbow(uint8_t wait) {
  static uint16_t i;
  for(uint16_t j=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel((i+j) & 255));
  }
  UpdateBrightness();
  strip.show();
  i++;
  if (i >= PIXEL_COUNT) {
    i = 0;
  }
  delay(wait);
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  static uint16_t j;
  
  for(uint16_t i=0; i< strip.numPixels(); i++) {
    strip.setPixelColor(strip.numPixels() - i -1, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
  }
  UpdateBrightness();
  strip.show();
  j++;
  if (j >= 256) {
    j = 0;
  }
  delay(wait);
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      UpdateBrightness();
      strip.show();
      delay(wait);
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      UpdateBrightness();
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(pow(255 - WheelPos*3,2)/255, 0, pow(WheelPos*3,2)/255);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, pow(WheelPos*3,2)/255, pow(255 - WheelPos*3,2)/255);
  }
  WheelPos -= 170;
  return strip.Color(pow(WheelPos*3,2)/255, pow(255 - WheelPos*3,2)/255, 0);
}
