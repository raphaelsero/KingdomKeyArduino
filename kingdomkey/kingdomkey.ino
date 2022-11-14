#include <FastLED.h>
#define DATA_PIN    3
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    8
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

#include <pcmConfig.h>
#include <pcmRF.h>
#include <TMRpcm.h>

#include <Wire.h>
#include <PCM.h>
#include <SD.h>
#define SD_ChipSelectPin 10
#define buffSize 128
#include <SPI.h>
#include <TMRpcm.h>

int CS_PIN = 10;
int blue_pin = 7;
int green_pin = 4;
int red_pin = 2;
int count = 0;

TMRpcm tmrPcm;


void setup()
{
    // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  pinMode(6, OUTPUT);
  Serial.begin(9600);
  tmrPcm.speakerPin = 9;
  if (!SD.begin(SD_ChipSelectPin))
  {
    Serial.println("SD Fail");
    return;
  }

  tmrPcm.setVolume(6);
  


}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns


  
void loop()
{ 
  int buttonState = digitalRead(5);
  int buttonState2 = digitalRead(8);
  
  countManager();
  mainButton(buttonState, buttonState2);
  accentButton(buttonState, buttonState2);
  bothPressed(buttonState, buttonState2); 
  delay(100);  

   // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}


void bothPressed(int buttonState, int buttonState2){
  if(buttonState == 1 && buttonState2 == 1){
    digitalWrite(6, HIGH);
    }
    else {
      digitalWrite(6, LOW);
      }
  
  }




  
void countManager(){
  if(count == 5) {
      count = 0;
      }

  if(count == 4) {
      turnOffLed();
      count = 4;
      Serial.println("Attack");
      }

      if(count == 0 ) {
      setColor(255,0,0);
      count = 0;
      Serial.println("Fire!");
      }

      if(count == 1) {
      setColor(0,0,255);
      count = 1;
      Serial.println("Freeze!");
      }

      if(count == 2) {
      setColor(255,255,0);
      count = 2;
      Serial.println("Thunder!");
      }

      if(count == 3) {
      setColor(0,255,0);
      count = 3;
      Serial.println("Heal!");
      } 
}


void mainButton(int buttonState, int buttonState2){
  if (buttonState == 1 && buttonState2 != 1)
  {  
     tmrPcm.play("khselect.wav");
     count++;     
  }
}

void accentButton(int buttonState, int buttonState2){
    if (buttonState2 == 1 && buttonState != 1){
    if(count == 0){
      delay(500);
    }
    if(count == 1){
      delay(500);
    }
    if(count == 2){
      delay(500);
            tmrPcm.play("thunder.wav");

    }
    if(count == 3){
      delay(500);
    }
    if(count == 4){
      delay(500);
    }
    delay(500);
    count++;
  }
 }
void turnOffLed(){
     digitalWrite(red_pin, LOW);
     digitalWrite(green_pin, LOW);
     digitalWrite(blue_pin, LOW);
  }
void setColor(int red, int green, int blue){
  analogWrite(red_pin, red);
  analogWrite(green_pin, green);
  analogWrite(blue_pin, blue);
  }


  
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  uint8_t dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
