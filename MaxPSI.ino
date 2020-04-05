/***************************
 *  Maxstang's MaxPSI Sketch for the PSI PRO Connected
 *  Written by Neil Hutchison
 *  Main sequence transitions by Krijn Schaap, based on his PSI sketch.  Many thanks Krijn.
 *  Pattern Timing Tuning by Malcolm MacKenzie
 *  
 *
 *  Thanks to Malcolm (Maxstang) for the boards, support, testing and encouragement.
 *
 *  Version 0.96
 *
 *  Version History :
 *  
 *  Version 0.96 - 5th April 2020
 *  Added address checking for T commands
 *  0 is all
 *  4 is Front PSI
 *  5 is rear PSI as taken from Marc's Teeces command guide.
 *  
 *      Address field is interpreted as follows:
 *      0 - global address, all displays that support the command are set
 *      1 - TFLD (Top Front Logic Dislay)
 *      2 - BFLD (Bottom Front Logic Display)
 *      3 - RLD  (Rear Logic Display)
 *      4 - Front PSI
 *      5 - Rear PSI
 *      6 - Front Holo (not implemented here)
 *      7 - Rear Holo  (not implemented here)
 *      8 - Top Holo   (not implemented here)
 *  
 *  Version 0.9.5 - 5th April 2020
 *  Star Wars scrolling text sequence added
 *  Minor bug fixes
 *  
 *  Version 0.94 - 4th April 2020
 *  Comments cleanup and clarification
 *  More timing tweaks
 *  Work around added for serial difficulties with non Sparkfun Pro Micro
 *  
 *  
 *  Version 0.93 - 1st April 2020 (Happy April Fools Day!)
 *  Code cleanup, and code size reduction
 *  Timing tweaks from Malcolm for various sequences.
 *  Updated JawaLite To support A, D and P (P used to change always on mode)
 *  T1 (Swipe) is now the default sequence, as MarcDuino sends 0T1 on startup.
 *  Added the ability to set the default pattern in the config.h  
 *      Note that MarcDuino will send 0T1, so whatever is in Mode 1 will be the starting pattern.
 *      After that point when a sequence completes, it will restore the "defaultPattern" 
 *      as defined in config.h
 *  EEPROM Support added to store various global settings:
 *      alwaysOn config
 *      Internal or External POT use
 *      Internal brightness setting if using Internal Brightness value (1P1 was sent)
 *  Fixed a bug in the VU Meter Sequence.
 *
 *  Version 0.8 - 31st March 2020
 *  Added Lightsaber Battle animation
 *  Added Pulse for rear logic dsiplay on T9
 *  Updated JawaLite Commanding on Serial to be 0Txx format
 *  Added the ability to change the serial port by defining USB_DEBUG.  
 *    Uncomment #define USB_DEBUG for serial comunications using Tx and Rx (removed again)
 *  Set the default behavior for unrecognised commands to just keep running the swipe pattern.
 *  Configuration data moved to config.h rather than being scattered.
 *
 *  Version 0.7 - 30th March 2020
 *  Non-Delay version of code.
 *  Allows sequences to be interrupted at ay time.
 *  Waiting for sequence completion is no longer required
 *  Set the defauly brightness in setup from the Brightness POT
 *
 *  Version 0.6 - 29th March 2020
 *  Base versions of most sequences implented
 *  Support for Front/Rear color selection using Jumper implemented
 *  Brightness Pot implemented
 * 
 *  Supported JAWALite Commands via Serial or i2c:
 *
 *  Serial:
 *
 *  Command T - Trigger a numbered Mode.  Txx where xx is the pattern number below. When using the R2 Touch app, commands
 *              should be in the form @0Tx\r or @0Txx\r
 *  
 *  Command A - Go to Main mode of operation which is Standard Swipe Pattern.
 *              @0A from R2 Touch
 *  
 *  Command D - Go to Default mode which is the Standard Swipe Pattern.
 *              @0D from R2 Touch
 *  
 *  Command xPy - Sets various board parameters.
 *                If x is 0, Set the alwaysOn behavior of the panel
 *                  The default mode for the panel is to display command sequences for 
 *                  a given time, then revert to the standard swipe behavior.  
 *                  By sending the xPy command, this can be changed.
 *                  Y is either 0 or 1 (default or always on mode)
 *                  0P0 - Default mode, where swipe is restored after the sequence plays
 *                  0P1 - The sequence continues to play until a new comand is received.
 *                If x is 1, Set the POT mode
 *                  The default is to read the external POT value for setting brightness
 *                  Y is either 0 or 1 (Pot or internal setting)
 *                  1P0 - Default mode, uses the external POT to set the LED brightness
 *                  1P1 - Use the internal brightness, which is set using command 2Py below
 *                  
 *                If x is 2, Set the internal brightness value, overriding the POT.
 *                  The default setting is that brightness is 20.
 *                  y is a value between 0 (off) and 255 (max brightness) This value
 *                  is saved to the EPROM and will persist after power down. 
 *                  for example:  2Py or 2Pyy or 2Pyyy
 *               
 *               @xPy from R2 Touch (You don't need the '0' before the x when using the P command. 
 *                                          
 *                                       ********   WARNING ********
 *                                       
 *              This PSI CAN DRAW MORE POWER THAN YOUR computer's USB PORT CAN SUPPLY!! When using the USB connection 
 *              on the Pro Micro to power the PSI (during programming for instance) be sure to have the brightness 
 *              POT turned nearly all the way COUNTERCLOCKWISE.  Having the POT turned up too far when plugged into 
 *              USB can damage the Pro Micro and/or your computer's USB port!!!! If you are using the internal brightness
 *              control and are connected to USB, KEEP THIS VALUE LOW, not higher than 20. 
 * 
 *  i2c:
 *
 *  When sending i2c command the Panel Address is defined below to be 22.  The command type and value are needed.  To trigger a
 *  pattern, send character 'T' and the value corresponding to the pattern list below to trigger the corresponding sequence.
 *  Sequences must be terminated with a carriage return (\r).  Using i2c with the R2 Touch app, commands must be sent in hex.
 *  form &220T6\r would be spelled &22,x33,x54,x36,x0D\r
 *
 *  Commands
 *
 *    Mode 0  - Turn Panel off
 *    Mode 1  - Default (Swipe) The default mode can be changed on the config.h tab
 *    Mode 2  - Flash (fast flash)
 *    Mode 3  - Alarm (slow flash)
 *    Mode 4  - Short Circuit
 *    Mode 5  - Scream
 *    Mode 6  - Leia Message
 *    Mode 7  - I heart U
 *    Mode 8  - Quarter Panel sweep
 *    Mode 9  - Flashing Red Heart (Front PSI), Pulse Monitor (Rear PSI)
 *    Mode 10 - Star Wars - Title Scroll
 *    Mode 11 - Imperial March
 *    Mode 12 - VU Meter
 *    Mode 13 - Disco Ball
 *    Mode 14 - Rebel Symbol
 *    Mode 15 - Knight Rider
 *    Mode 16 - Test Sequence (white on Indefinitely)
 *    Mode 17 - Red on Indefinitely  
 *    Mode 18 - Green on Indefinitely
 *    Mode 19 - LightSaber Battle
 *    
 * Most users shouldn't need to change anything below this line. Please see the config.h tab    
 * for user adjustable settings.  
*/

// Include the various libraries that we need.
#include <FastLED.h>
#include <EEPROM.h>
#include "matrices.h"
#include "config.h"
#include "Wire.h"

// Setup the LED Matrix
CRGB leds[NUM_LEDS];

// Brightness control
bool internalBrightness = false;
uint8_t globalBrightnessValue = 20; // Set to a default of 20.  This is overridden in the P command or read from EEPROM.

// Command loop processing times
unsigned long previousMillis = 0;
unsigned long interval = 25;

//counters and state stuff
unsigned long doNext;
unsigned long globalTimeout;

// Global animation stuff is defined here.
int updateLed = 0;
int ledPatternState;
bool firstTime;
bool patternRunning = false;
uint8_t globalPatternLoops;

// Used for the VU display to store global state ...
int level[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};;

//Serial Stuff
int lastPSIeventCode     = defaultPattern;
bool firstTimeCode      = true;

// Swipe Default pattern stuff
enum state {
  Primary,
  PrimaryToSecondary,
  Secondary,
  SecondaryToPrimary
};

// Global animation state for the Swipe Default sequence is defined here
state ledState = Primary;
uint8_t visibleSecondaryColumns = 0;
unsigned long nextEvent = 0;
unsigned long swipeDelay = 0;
unsigned long lastLedUpdate = 0;
CRGB overlayColors[COLUMNS];

// function prototype for the Matrix Display.  This is doing cleverness.  Don't change it!
void displayMatrixColor(byte PROGMEM * matrix, CRGB fgcolor, CRGB bgcolor, bool displayMe, unsigned long timeout, 
                        CRGB color2=0x000000, CRGB color3=0x000000, CRGB color4=0x000000, CRGB color5=0x000000,
                        CRGB color6=0x000000, CRGB color7=0x000000, CRGB color8=0x000000);

// Function prototype for helper functions
void fill_row(uint8_t row, CRGB color, uint8_t scale_brightness=0);
void fill_column(uint8_t column, CRGB color, uint8_t scale_brightness=0);                       

// Setup
void setup() {

  // Setup LED defaults
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  pinMode(JUMP_FRONT_REAR, INPUT_PULLUP);

  //Sets the default brightness, this reads from the POT and sets the value
  FastLED.setBrightness(brightness());

  //  Setup I2C
  Wire.begin(I2CAdress);                   // Start I2C Bus as Master I2C Address
  Wire.onReceive(receiveEvent);            // register event so when we receive something we jump to receiveEvent();

  // Setup the Serial for debug and command input
  // initialize suart used to communicate with the JEDI Display at 2400 or 9600 bauds
  // This is set in config.h
  uint16_t baudrate;
#ifdef _9600BAUDSJEDI_
  baudrate=9600;
#else
  baudrate=2400;
#endif
  //Serial.begin(baudrate);
  Serial1.begin(baudrate);

  // READ the default settings from the EEPROM
  byte value;

  // Set the Always on Behavior.
  value = EEPROM.read(alwaysOnAddress);
  if (value == 0) {
    alwaysOn = false;
    DEBUG_PRINT_LN("Panel Behaviour set to default, run sequence then default");
  }
  if (value == 1) {
    alwaysOn = true;
    DEBUG_PRINT_LN("Panel Behaviour set to always on.  Send new command to change sequence.");
  }

  value = EEPROM.read(externalPOTAddress);
  if (value == 0) {
    internalBrightness = false;
    DEBUG_PRINT_LN("Using External POT for brightness control");
  }
  if (value == 1) {
    internalBrightness = true;
    DEBUG_PRINT_LN("Using Internal value for brightness control");
  }

  value = EEPROM.read(internalBrightnessAddress);
  globalBrightnessValue = value;
  DEBUG_PRINT("Global LED Brightness set to :"); DEBUG_PRINT_LN(globalBrightnessValue);
  
  DEBUG_PRINT_LN("Ready");
}


// Main loop
void loop()
{
  // Get current time.
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > interval)
  {
    //DEBUG_PRINT_LN("Main Loop Tick");
    previousMillis = currentMillis;

    if (patternRunning)
    {
      runPattern(lastPSIeventCode);
    }
    else
    {
      lastPSIeventCode == defaultPattern;
      runPattern(lastPSIeventCode);
    }
  }
}

//////////////////////////
// LED Helper Functions //
//////////////////////////

void allON(CRGB color, bool showLED)
{
  //DEBUG_PRINT_LN("Turn on all LEDs");
  fill_solid(leds, NUM_LEDS, color);
  if (showLED) FastLED.show(brightness());
}

void allOFF(bool showLED)
{
  //DEBUG_PRINT_LN("LED All Off");
  FastLED.clear();
  if (showLED) FastLED.show();
}

void fill_column(uint8_t column, CRGB color, uint8_t scale_brightness=0) {
  for (int i = 0; i < LEDS_PER_COLUMN; i++) {
    int8_t ledIndex = ledMatrix[column][i];
    if (ledIndex != -1) {
      leds[ledIndex] = color;
      if (scale_brightness != 0) leds[ledIndex] %= scale_brightness;
    }
  }
}

// Fills half a column.  0 for top half, 1 for bottom half.
void fill_half_column(uint8_t column, uint8_t half, CRGB color) {
  uint8_t start;
  uint8_t end_col;
  if (!half) {
    start = 0;
    end_col = LEDS_PER_COLUMN / 2;
  }
  else
  {
    start = LEDS_PER_COLUMN / 2;
    end_col = LEDS_PER_COLUMN;
  }

  for (int i = start; i < end_col; i++) {
    int8_t ledIndex = ledMatrix[column][i];
    if (ledIndex != -1) leds[ledIndex] = color;
  }
}

void fill_row(uint8_t row, CRGB color, uint8_t scale_brightness=0) {
  for (int i = 0; i < COLUMNS; i++) {
    int8_t ledIndex = ledMatrix[i][row];
    if (ledIndex != -1) {
      leds[ledIndex] = color;
      if (scale_brightness != 0) leds[ledIndex] %= scale_brightness;
    }
  }
}

// Display a solid line across each row in ascending or decending order, based on the scanDirection
// Scan Direction of 0 will scan Down from the top row, 1 swill scan up from the bottom row.
void scanRow(unsigned long time_delay, int start_row, CRGB color, bool scanDirection)
{
  if (firstTime) {
    if (scanDirection == 0) ledPatternState = start_row;
    // Down
    if (scanDirection == 1) ledPatternState = (LEDS_PER_COLUMN - 1) - start_row;
    firstTime = false;
    patternRunning = true;
  }

  updateLed = 0;

  if (checkDelay()) {
    switch (ledPatternState) {
      case 0: {
          allOFF(true);
          fill_row(0, color);
          updateLed = 1;
          break;
        }
      case 1: {
          allOFF(true);
          fill_row(1, color);
          updateLed = 1;
          break;
        }
      case 2: {
          allOFF(true);
          fill_row(2, color);
          updateLed = 1;
          break;
        }
      case 3: {
          allOFF(true);
          fill_row(3, color);
          updateLed = 1;
          break;
        }
      case 4: {
          allOFF(true);
          fill_row(4, color);
          updateLed = 1;
          break;
        }
      case 5: {
          allOFF(true);
          fill_row(5, color);
          updateLed = 1;
          break;
        }
      default: {
          // Do nothing.
          break;
        }
    }

    // Increment the state.
    if (scanDirection == 0) ledPatternState++;
    if (scanDirection == 1) ledPatternState --;
    if (ledPatternState < 0)
    {
      ledPatternState = (LEDS_PER_COLUMN - 1);
      globalPatternLoops--;
    }
    else if (ledPatternState > (LEDS_PER_COLUMN - 1))
    {
      ledPatternState = 0;
      globalPatternLoops--;
    }
  }

  if (updateLed) {
    FastLED.show(brightness());
    set_delay(time_delay);
  }
}

// Scans down the rows starting at the first specified row from the bottom
void scanRowDownUp(unsigned long time_delay, int start_row, CRGB color, bool scanDirection)
{
  if (firstTime) {
    if (scanDirection == 0) ledPatternState = start_row;
    if (scanDirection == 1) ledPatternState = (LEDS_PER_COLUMN - 1) - start_row;
    firstTime = false;
    patternRunning = true;
  }

  updateLed = 0;

  if (checkDelay()) {
    switch (ledPatternState) {
      case 0: {
          allOFF(true);
          fill_row(0, color);
          updateLed = 1;
          break;
        }
      case 1: {
          allOFF(true);
          fill_row(1, color);
          updateLed = 1;
          break;
        }
      case 2: {
          allOFF(true);
          fill_row(2, color);
          updateLed = 1;
          break;
        }
      case 3: {
          allOFF(true);
          fill_row(3, color);
          updateLed = 1;
          break;
        }
      case 4: {
          allOFF(true);
          fill_row(4, color);
          updateLed = 1;
          break;
        }
      case 5: {
          allOFF(true);
          fill_row(5, color);
          updateLed = 1;
          break;
        }
      case 6: {
          allOFF(true);
          fill_row(4, color);
          updateLed = 1;
          break;
        }
      case 7: {
          allOFF(true);
          fill_row(3, color);
          updateLed = 1;
          break;
        }
      case 8: {
          allOFF(true);
          fill_row(2, color);
          updateLed = 1;
          break;
        }
      case 9: {
          allOFF(true);
          fill_row(1, color);
          updateLed = 1;
          break;
        }
      default: {
          // Do nothing.
          break;
        }
    }

    // Increment the state.
    if (scanDirection == 0) ledPatternState++;
    if (scanDirection == 1) ledPatternState--;

    if (ledPatternState < 0)
    {
      ledPatternState = 9;
      globalPatternLoops--;
    }
    else if (ledPatternState > 9)
    {
      ledPatternState = 0;
      globalPatternLoops--;
    }
  }

  if (updateLed) {
    FastLED.show(brightness());
    set_delay(time_delay);
  }
}

// Display a solid line across each row in ascending or decending order, based on the scanDirection
// Scan Direction of 0 will scan right to left across the columns starting at the first specified row from the right,
// Scan Direction of 1 will scan left to right across the columns starting at the first specified row from the left.
void scanCol(unsigned long time_delay, int start_col, CRGB color, bool scanDirection)
{
  if (firstTime) {
    if (scanDirection == 0) ledPatternState = start_col;
    // Down
    if (scanDirection == 1) ledPatternState = (COLUMNS - 1) - start_col;
    firstTime = false;
    patternRunning = true;
  }

  updateLed = 0;

  if (checkDelay()) {
    switch (ledPatternState) {
      case 0: {
          allOFF(true);
          fill_column(0, color);
          updateLed = 1;
          break;
        }
      case 1: {
          allOFF(true);
          fill_column(1, color);
          updateLed = 1;
          break;
        }
      case 2: {
          allOFF(true);
          fill_column(2, color);
          updateLed = 1;
          break;
        }
      case 3: {
          allOFF(true);
          fill_column(3, color);
          updateLed = 1;
          break;
        }
      case 4: {
          allOFF(true);
          fill_column(4, color);
          updateLed = 1;
          break;
        }
      case 5: {
          allOFF(true);
          fill_column(5, color);
          updateLed = 1;
          break;
        }
      case 6: {
          allOFF(true);
          fill_column(6, color);
          updateLed = 1;
          break;
        }
      case 7: {
          allOFF(true);
          fill_column(7, color);
          updateLed = 1;
          break;
        }
      case 8: {
          allOFF(true);
          fill_column(8, color);
          updateLed = 1;
          break;
        }
      case 9: {
          allOFF(true);
          fill_column(9, color);
          updateLed = 1;
          break;
        }
      default: {
          // Do nothing.
          break;
        }
    }

    // Increment the state.
    if (scanDirection == 0) ledPatternState++;
    if (scanDirection == 1) ledPatternState --;
    if (ledPatternState < 0)
    {
      ledPatternState = (COLUMNS - 1);
      globalPatternLoops--;
    }
    else if (ledPatternState > (COLUMNS - 1))
    {
      ledPatternState = 0;
      globalPatternLoops--;
    }
  }

  if (updateLed) {
    FastLED.show(brightness());
    set_delay(time_delay);
  }
}

// Sweeps Left/Right or Right/Left across the columns.
// Scan Direction of 0 will scan right to left across the columns starting at the first specified row from the right,
// Scan Direction of 1 will scan left to right across the columns starting at the first specified row from the left.
void scanColLeftRight(unsigned long time_delay, int start_row, CRGB color, bool scanDirection)
{
  if (firstTime) {
    if (scanDirection == 0) ledPatternState = start_row;
    if (scanDirection == 1) ledPatternState = 9 - start_row;
    firstTime = false;
    patternRunning = true;
  }

  updateLed = 0;

  if (checkDelay()) {
    switch (ledPatternState) {
      case 0: {
          allOFF(true);
          fill_column(0, color);
          updateLed = 1;
          break;
        }
      case 1: {
          allOFF(true);
          fill_column(1, color);
          updateLed = 1;
          break;
        }
      case 2: {
          allOFF(true);
          fill_column(2, color);
          updateLed = 1;
          break;
        }
      case 3: {
          allOFF(true);
          fill_column(3, color);
          updateLed = 1;
          break;
        }
      case 4: {
          allOFF(true);
          fill_column(4, color);
          updateLed = 1;
          break;
        }
      case 5: {
          allOFF(true);
          fill_column(5, color);
          updateLed = 1;
          break;
        }
      case 6: {
          allOFF(true);
          fill_column(6, color);
          updateLed = 1;
          break;
        }
      case 7: {
          allOFF(true);
          fill_column(7, color);
          updateLed = 1;
          break;
        }
      case 8: {
          allOFF(true);
          fill_column(8, color);
          updateLed = 1;
          break;
        }
      case 9: {
          allOFF(true);
          fill_column(9, color);
          updateLed = 1;
          break;
        }
      case 10: {
          allOFF(true);
          fill_column(8, color);
          updateLed = 1;
          break;
        }
      case 11: {
          allOFF(true);
          fill_column(7, color);
          updateLed = 1;
          break;
        }
      case 12: {
          allOFF(true);
          fill_column(6, color);
          updateLed = 1;
          break;
        }
      case 13: {
          allOFF(true);
          fill_column(5, color);
          updateLed = 1;
          break;
        }
      case 14: {
          allOFF(true);
          fill_column(4, color);
          updateLed = 1;
          break;
        }
      case 15: {
          allOFF(true);
          fill_column(3, color);
          updateLed = 1;
          break;
        }
      case 16: {
          allOFF(true);
          fill_column(2, color);
          updateLed = 1;
          break;
        }
      case 17: {
          allOFF(true);
          fill_column(1, color);
          updateLed = 1;
          break;
        }
      default: {
          // Do nothing.
          break;
        }
    }

    // Increment the state.
    if (scanDirection == 0) ledPatternState++;
    if (scanDirection == 1) ledPatternState--;

    if (ledPatternState < 0)
    {
      ledPatternState = 17;
      globalPatternLoops--;
    }
    else if (ledPatternState > 17)
    {
      ledPatternState = 0;
      globalPatternLoops--;
    }
  }

  if (updateLed) {
    FastLED.show(brightness());
    set_delay(time_delay);
  }
}

// Display a matrix using the byte array.  Colours are defined so that
// if the matrix has a 1, we use fgcolor, and 0 is bgcolor.
// Optionally colors 2,3,4,5,6,7,8 (allowing a total of nine colours to be used in a pattern.
void displayMatrixColor(byte PROGMEM * matrix, CRGB fgcolor, CRGB bgcolor, bool displayMe, unsigned long timeout, 
                        CRGB color2=0x000000, CRGB color3=0x000000, CRGB color4=0x000000, CRGB color5=0x000000,
                        CRGB color6=0x000000, CRGB color7=0x000000, CRGB color8=0x000000)
{
  // global LED ID counter ...
  int ledNum = 0;
  byte ledOn;

  if (firstTime) {
    firstTime = false;
    patternRunning = true;
    allOFF(true);
    // Special case where we don't want to use the global timeout!
    if (timeout != 0) set_global_timeout(timeout);
  }

  // First row is easy
  for (int i = 0; i < 6; i++) {
    ledOn = pgm_read_byte(&(matrix[i])); // Read pixel
    switch (ledOn){
      case 0: leds[ledNum] = bgcolor; break;
      case 1: leds[ledNum] = fgcolor; break;
      case 2: leds[ledNum] = color2; break;
      case 3: leds[ledNum] = color3; break;
      case 4: leds[ledNum] = color4; break;
      case 5: leds[ledNum] = color5; break;
      case 6: leds[ledNum] = color6; break;
      case 7: leds[ledNum] = color7; break;
      case 8: leds[ledNum] = color8; break;
    }  
    ledNum++;
  }
  // Second row, we take 7-14 and make them 14-17
  for (int i = 0; i < 8; i++) {
    ledOn = pgm_read_dword(&(matrix[13 - i]));
    switch (ledOn){
      case 0: leds[ledNum] = bgcolor; break;
      case 1: leds[ledNum] = fgcolor; break;
      case 2: leds[ledNum] = color2; break;
      case 3: leds[ledNum] = color3; break;
      case 4: leds[ledNum] = color4; break;
      case 5: leds[ledNum] = color5; break;
      case 6: leds[ledNum] = color6; break;
      case 7: leds[ledNum] = color7; break;
      case 8: leds[ledNum] = color8; break;
    }  
    ledNum++;
  }
  // Third row, we take 15-24 as is
  for (int i = 0; i < 10; i++) {
    ledOn = pgm_read_dword(&(matrix[14 + i]));
    switch (ledOn){
      case 0: leds[ledNum] = bgcolor; break;
      case 1: leds[ledNum] = fgcolor; break;
      case 2: leds[ledNum] = color2; break;
      case 3: leds[ledNum] = color3; break;
      case 4: leds[ledNum] = color4; break;
      case 5: leds[ledNum] = color5; break;
      case 6: leds[ledNum] = color6; break;
      case 7: leds[ledNum] = color7; break;
      case 8: leds[ledNum] = color8; break;
    }  
    ledNum++;
  }
  // Fourth row, we take 25-34 and make them 34-25
  for (int i = 0; i < 10; i++) {
    ledOn = pgm_read_dword(&(matrix[33 - i]));
    switch (ledOn){
      case 0: leds[ledNum] = bgcolor; break;
      case 1: leds[ledNum] = fgcolor; break;
      case 2: leds[ledNum] = color2; break;
      case 3: leds[ledNum] = color3; break;
      case 4: leds[ledNum] = color4; break;
      case 5: leds[ledNum] = color5; break;
      case 6: leds[ledNum] = color6; break;
      case 7: leds[ledNum] = color7; break;
      case 8: leds[ledNum] = color8; break;
    }  
    ledNum++;
  }
  // Fifth row, we take 35-42 as is
  for (int i = 0; i < 8; i++) {
    ledOn = pgm_read_dword(&(matrix[34 + i]));
    switch (ledOn){
      case 0: leds[ledNum] = bgcolor; break;
      case 1: leds[ledNum] = fgcolor; break;
      case 2: leds[ledNum] = color2; break;
      case 3: leds[ledNum] = color3; break;
      case 4: leds[ledNum] = color4; break;
      case 5: leds[ledNum] = color5; break;
      case 6: leds[ledNum] = color6; break;
      case 7: leds[ledNum] = color7; break;
      case 8: leds[ledNum] = color8; break;
    }  
    ledNum++;
  }
  // Sixth and final row, we take 43-48 and make them 48-43
  for (int i = 0; i < 6; i++) {
    ledOn = pgm_read_dword(&(matrix[47 - i]));
    switch (ledOn){
      case 0: leds[ledNum] = bgcolor; break;
      case 1: leds[ledNum] = fgcolor; break;
      case 2: leds[ledNum] = color2; break;
      case 3: leds[ledNum] = color3; break;
      case 4: leds[ledNum] = color4; break;
      case 5: leds[ledNum] = color5; break;
      case 6: leds[ledNum] = color6; break;
      case 7: leds[ledNum] = color7; break;
      case 8: leds[ledNum] = color8; break;
    }  
    ledNum++;
  }

  if (displayMe) FastLED.show(brightness());

  if (timeout != 0) {
    globalTimerDonedoRestoreDefault();
  }
}

//////////////////////////////
// END LED Helper Functions //
//////////////////////////////

// Flashes all LED's to the given color.  The time_delay is the delay that the LED is on then Off
void flash(CRGB color, unsigned long time_delay, int loops) //4 seconds same as alarm Command 0T2
{
  if (firstTime) {
    DEBUG_PRINT_LN("Flash");
    firstTime = false;
    patternRunning = true;
    ledPatternState = 0;
    // We set loops to double here, because we decrement for each on and each off cycle
    globalPatternLoops = loops * 2;
  }

  updateLed = 0;

  if (checkDelay()) {
    switch (ledPatternState) {
      case 0: {
          allON(color, false);
          updateLed = 1;
          break;
        }
      case 1: {
          allOFF(false);
          updateLed = 1;
          break;
        }
      default: {
          // Do nothing.
          break;
        }
    }

    // Toggle the state.
    ledPatternState = ledPatternState ^ 1;
    globalPatternLoops--;
  }

  if (updateLed) {
    FastLED.show(brightness());
    set_delay(time_delay);
  }

  // Check to see if we have run the loops needed for this pattern
  loopsDonedoRestoreDefault();
}

void Cylon_Row(CRGB color, unsigned long time_delay, int type, int loops)
{

  if (firstTime) {
    DEBUG_PRINT("Cylon Row");
    globalPatternLoops = loops;
  }

  switch (type) {
    case 1:
      // Scan Down then Up ...
      scanRowDownUp(time_delay, 0, color, 0);
      break;
    case 2 :
      // Scan Up then down
      scanRowDownUp(time_delay, 0, color, 1);
      break;
    case 3:
      // Scan Down ...
      //scanDown(time_delay, 0, color);
      scanRow(time_delay, 0, color, 0);
      break;
    case 4:
      // Scan Up ...
      //scanUp(time_delay, 0, color);
      scanRow(time_delay, 0, color, 1);
      break;
    default:
      // do nothing
      break;
  }

  // Check to see if we have run the loops needed for this pattern
  loopsDonedoRestoreDefault();
}

void Cylon_Col(CRGB color, unsigned long time_delay, int type, int loops)
{
  if (firstTime) {
    DEBUG_PRINT("Cylon Col");
    globalPatternLoops = loops;
  }


  switch (type) {
    case 1:
      // Scan Right then left
      scanColLeftRight(time_delay, 0, color, 0);
      break;
    case 2:
      // Scan Left then Right
      scanColLeftRight(time_delay, 0, color, 1);
      break;
    case 3:
      // Scan Right to left ...
      scanCol(time_delay, 0, color, 0);
      break;
    case 4:
      // Scan Left to right ...
      scanCol(time_delay, 0, color, 1);
      break;
    default:
      // do nothing
      break;
  }

  // Check to see if we have run the loops needed for this pattern
  loopsDonedoRestoreDefault();
}

// Displays the message I <heart> U.
void i_heart_u(unsigned long time_delay, int loops) // 5 seconds command 0T7
{
  if (firstTime) {
    DEBUG_PRINT_LN("I Heart U");
    firstTime = false;
    patternRunning = true;
    globalPatternLoops = loops;
    ledPatternState = 0;
    // Clear the display the first time through
    allOFF(true);
  }

  updateLed = 0;

  if (checkDelay()) {
    switch (ledPatternState) {
      // Note we set the display timeout large here so that the image stays displayed.
      case 0: displayMatrixColor(LetterI, 0xff0000, 0x909497, false, 0); updateLed = 1; break;
      case 1: allOFF(false); updateLed = 1; break;
      case 2: displayMatrixColor(Heart, 0xff0000, 0x909497, false, 0); updateLed = 1; break;
      case 3: allOFF(false); updateLed = 1; break;
      case 4: displayMatrixColor(LetterU, 0xff0000, 0x909497, false, 0); updateLed = 1; break;
      case 5: allOFF(false); updateLed = 1; break;
      default: {
          // Do nothing.
          break;
        }
    }

    // Increment the state.
    ledPatternState++;
    if (ledPatternState > 5)
    {
      ledPatternState = 0;
      globalPatternLoops--;
    }
  }

  if (updateLed) {
    FastLED.show(brightness());
    set_delay(time_delay);
  }

  // Check to see if we have run the loops needed for this pattern
  loopsDonedoRestoreDefault();
}

// Displays a flashing heart.
void red_heart(unsigned long time_delay, int loops) //5 seconds command 0T9
{
  if (firstTime) {
    DEBUG_PRINT("Flashing Red Heart");
    firstTime = false;
    patternRunning = true;
    globalPatternLoops = loops;
    ledPatternState = 0;
    // Clear the display the first time through
    allOFF(true);
  }

  updateLed = 0;

  if (checkDelay()) {
    switch (ledPatternState) {
      // Note we set the display timeout large here so that the image stays displayed.
      case 0: displayMatrixColor(Heart, 0xff0000, 0x909497, true, 0); updateLed = 1; break;//ffffff
      case 1: allOFF(false); updateLed = 1; break;
      case 2: displayMatrixColor(Heart, 0xff0000, 0x909497, true, 0); updateLed = 1; break;
      case 3: allOFF(false); updateLed = 1; break;
      case 4: displayMatrixColor(Heart, 0xff0000, 0x909497, true, 0); updateLed = 1; break;
      case 5: allOFF(false); updateLed = 1; break;
      default: {
          // Do nothing.
          break;
        }
    }

    // Increment the state.
    ledPatternState++;
    if (ledPatternState > 5)
    {
      ledPatternState = 0;
      globalPatternLoops--;
    }
  }

  if (updateLed) {
    FastLED.show(brightness());
    set_delay(time_delay);
  }

  // Check to see if we have run the loops needed for this pattern
  loopsDonedoRestoreDefault();
}

void march(CRGB color, unsigned long time_delay, int loops) //47 seconds Command 0T11
{
  if (firstTime) {
    DEBUG_PRINT_LN("Imperial March");
    firstTime = false;
    patternRunning = true;
    globalPatternLoops = loops * 2;
    ledPatternState = 0;
    // Clear the display the first time through
    allOFF(true);
  }

  updateLed = 0;

  if (checkDelay()) {
    switch (ledPatternState) {
      // Note we set the display timeout large here so that the image stays displayed.
      case 0:
        for (int i = 0; i < COLUMNS; i++)
        {
          if (i < COLUMNS / 2)
          {
            fill_column(i, color);
          }
          else fill_column(i, 0x000000);
        }
        updateLed = 1;
        break;
      case 1:
        for (int i = 0; i < COLUMNS; i++)
        {
          if (i < COLUMNS / 2)
          {
            fill_column(i, 0x000000);
          }
          else fill_column(i, color);
        }
        updateLed = 1;
        break;
      default: {
          // Do nothing.
          break;
        }
    }

    // Toggle the state.
    ledPatternState = ledPatternState ^ 1;
    globalPatternLoops--;
  }

  if (updateLed) {
    FastLED.show(brightness());
    set_delay(time_delay);
  }

  // Check to see if we have run the loops needed for this pattern
  loopsDonedoRestoreDefault();
}

// Sweeps the panel in a clockwise direction
void radar(CRGB color, unsigned long time_delay, int loops)
{
  if (firstTime) {
    DEBUG_PRINT_LN("Radar");
    firstTime = false;
    patternRunning = true;
    globalPatternLoops = loops;
    ledPatternState = 0;
    // Clear the display the first time through
    allOFF(true);
  }

  updateLed = 0;

  if (checkDelay()) {
    switch (ledPatternState) {
      // Note we set the display timeout large here so that the image stays displayed.
      case 0:
        fill_half_column(0, 0, color);
        fill_half_column(1, 0, color);
        fill_half_column(2, 0, color);
        fill_half_column(3, 0, color);
        fill_half_column(4, 0, color);
        updateLed = 1;
        break;
      case 1:
        allOFF(false);
        break;
      case 2:
        fill_half_column(5, 0, color);
        fill_half_column(6, 0, color);
        fill_half_column(7, 0, color);
        fill_half_column(8, 0, color);
        fill_half_column(9, 0, color);
        updateLed = 1;
        break;
      case 3:
        allOFF(false);
        break;
      case 4:
        fill_half_column(5, 1, color);
        fill_half_column(6, 1, color);
        fill_half_column(7, 1, color);
        fill_half_column(8, 1, color);
        fill_half_column(9, 1, color);
        updateLed = 1;
        break;
      case 5:
        allOFF(false);
        break;
      case 6:
        fill_half_column(0, 1, color);
        fill_half_column(1, 1, color);
        fill_half_column(2, 1, color);
        fill_half_column(3, 1, color);
        fill_half_column(4, 1, color);
        updateLed = 1;
        break;
      case 7:
        allOFF(false);
        break;
      default: {
          // Do nothing.
          break;
        }
    }

    // Increment the state.
    ledPatternState++;
    if (ledPatternState > 7)
    {
      ledPatternState = 0;
      globalPatternLoops--;
    }
  }

  if (updateLed) {
    FastLED.show(brightness());
    set_delay(time_delay);
  }

  // Check to see if we have run the loops needed for this pattern
  loopsDonedoRestoreDefault();
}

void swipe() {

  // We set this to false as we're not running a pattern
  // Yes, I know the swipe is a pattern, but it's the default pattern
  // Just work with me here people!
  patternRunning = false;

  if (millis() >= nextEvent) {
    switch (ledState) {
      case Primary: {
          ledState = PrimaryToSecondary;
          swipeDelay = random(SWIPE_DELAY_MINIMUM, SWIPE_DELAY_MAXIMUM);
          DEBUG_PRINT_LN("Switching to secondary color");


          int totalChance = CHANCE_SECONDARY_FULL + CHANCE_SECONDARY_PARTIAL + CHANCE_SECONDARY_PARTIAL_OFF;
          int selection = random(totalChance);
          if (selection < CHANCE_SECONDARY_FULL) {
            DEBUG_PRINT_LN("Selected full color");
            for (int i = 0; i < COLUMNS; i++) {
              overlayColors[i] = secondary_color();
            }
          } else if (selection < CHANCE_SECONDARY_FULL + CHANCE_SECONDARY_PARTIAL) {
            DEBUG_PRINT_LN("Selected partial secondary, with rest primary");
            int totalColumnsLit = random(SECONDARY_PARTIAL_LINES_MIN, SECONDARY_PARTIAL_LINES_MAX);
            for (int i = 0; i < COLUMNS; i++) {
              overlayColors[i] = i > COLUMNS - totalColumnsLit - 1 ? secondary_color() : primary_color();
            }
          } else {
            DEBUG_PRINT_LN("Selected partial secondary, rest off");
            for (int i = 0; i < COLUMNS; i++) {
              overlayColors[i] = i > COLUMNS - SECONDARY_PARTIAL_OFF_LINES - 1 ? secondary_color() : secondary_off_color();
            }
          }

          // Intentional fall through
        }
      case PrimaryToSecondary:
        visibleSecondaryColumns++;
        if (visibleSecondaryColumns >= COLUMNS) {
          DEBUG_PRINT_LN("On secondary color");
          ledState = Secondary;
          nextEvent = millis() + random(SECONDARY_COLOR_DURATION_MINIMUM, SECONDARY_COLOR_DURATION_MAXIMUM);
        } else {
          nextEvent = millis() + swipeDelay;
        }
        break;
      case Secondary: {
          ledState = SecondaryToPrimary;
          swipeDelay = random(SWIPE_DELAY_MINIMUM, SWIPE_DELAY_MAXIMUM);
          DEBUG_PRINT_LN("Switching to primary color");
          // Intentional fall through
        }
      case SecondaryToPrimary:
        visibleSecondaryColumns--;
        if (visibleSecondaryColumns == 0) {
          DEBUG_PRINT_LN("On primary color");
          ledState = Primary;
          nextEvent = millis() + random(PRIMARY_COLOR_DURATION_MINIMUM, PRIMARY_COLOR_DURATION_MAXIMUM);
        } else {
          nextEvent = millis() + swipeDelay;
        }
        break;
    }

    updateLed = 1;
  }

  if (updateLed || ((millis() - lastLedUpdate) > 100)) {
    lastLedUpdate = millis();

    CRGB primaryColor = primary_color();
    uint8_t switchPoint = COLUMNS - visibleSecondaryColumns;

    for (int i = 0; i < COLUMNS; i++) {
      CRGB columnColor = primaryColor;
      if (i >= switchPoint) {
        columnColor = overlayColors[i - switchPoint];
      }
      fill_column(i, columnColor);
    }

    FastLED.show(brightness());
  }
}

// Randomly fades and brightens pixels
// Generally used for a Short Circuit sequence.
void FadeOut(unsigned long time_delay, uint8_t loops) {

  // Variables to control the dim/brightness levels
  uint8_t dim_by;
  uint8_t multiply_by;

  int case0count = 4;
  int case1count = 8;

  int totalLoopCount  = loops * (case0count + case1count);

  // Note that we don't set the panel off here first
  // We use the brightness, and fade API's to play with
  // the panel appearance.

  if (firstTime) {
    DEBUG_PRINT_LN("Fade Out");
    firstTime = false;
    patternRunning = true;
    globalPatternLoops = totalLoopCount;
    ledPatternState = 0;
  }

  updateLed = 0;

  if (checkDelay()) {
    switch (ledPatternState) {
      // Note we set the display timeout large here so that the image stays displayed.
      // Do this 4 times ....
      case 0:
        for (int x = 0; x < NUM_LEDS; x++) {
          dim_by = random(220, 250);
          leds[x].nscale8_video(dim_by);
        }
        updateLed = 1;
        break;
      // Do this 8 times....
      case 1:
        for (int x = 0; x < NUM_LEDS; x++) {
          multiply_by = random(0, 6);
          leds[x] *= multiply_by;
        }
        updateLed = 1;
        break;
      default: {
          // Do nothing.
          break;
        }
    }

    // Toggle the state.
    // Really ugly way of counting loops :(
    if (globalPatternLoops == totalLoopCount - case0count) ledPatternState = ledPatternState ^ 1;
    if (globalPatternLoops == totalLoopCount - (case0count + case1count)) ledPatternState = ledPatternState ^ 1;
    if (globalPatternLoops == totalLoopCount - (case0count + case1count + case0count)) ledPatternState = ledPatternState ^ 1;
    if (globalPatternLoops == totalLoopCount - (case0count + case1count + case0count + case1count)) ledPatternState = ledPatternState ^ 1;
    globalPatternLoops--;
  }

  if (updateLed) {
    FastLED.show(brightness());
    set_delay(time_delay);
  }

  // Check to see if we have run the loops needed for this pattern
  loopsDonedoRestoreDefault();
}

// Delay, loops, color
void VUMeter(unsigned long time_delay, uint8_t loops)
{
  // We use the VU_chart matrix to define the colors used
  // Then we'll simply display as many or as few of the pixels as we want
  // and use a random number to determine rise and fall.

  if (firstTime) {
    DEBUG_PRINT_LN("VU Meter");
    firstTime = false;
    patternRunning = true;
    globalPatternLoops = loops;
    // Clear the display the first time through
    allOFF(true);

    // Set a default start level for each column.
    for (int i=0; i< COLUMNS; i++)
    {
      level[i] = random(0, 6);
    }
  }

  updateLed = 0;

  if (checkDelay()) {
    // read the display, but don't show it.
    // we'll blank out some pixels in a sec
    displayMatrixColor(VUChart, 0x008000, 0x000000, false, 0, 0xffd700, 0xff8c00, 0xff0000);

    // Now go through each column, and turn off the unused pixels
    for (int c = 0; c < COLUMNS; c++)
    {
      for (int i = 0; i < LEDS_PER_COLUMN; i++) {
        int8_t ledIndex = ledMatrix[c][i];
        if (ledIndex != -1) {
          if (level[c] > i) leds[ledIndex] = CRGB::Black;
        }
      }
    }
    
    updateLed = 1;

    // calc the next position of the bars
    for (int y = 0; y < COLUMNS; y++)
    {
      byte upDown = random(0, 2);
      byte changeSize = random(1, 3);
      // go up
      if (upDown == 1)
      {
        ((level[y] + changeSize) <= 6) ? level[y] += changeSize : level[y] = 6;
      }
      // go down
      else
      {
        ((level[y] - changeSize) >= 0) ? level[y] -= changeSize : level[y] = 0;
      }
    }

    globalPatternLoops--;
  }


  if (updateLed) {
    FastLED.show(brightness());
    set_delay(time_delay);
  }
  
  // Check to see if we have run the loops needed for this pattern
  loopsDonedoRestoreDefault();
}

//Display a number of random pixels to simulate light bouncing off a disco ball.  Yay 1977!
void DiscoBall(unsigned long time_delay, int loops, int numSparkles, CRGB color)
{
  int randRow;
  int randCol;
  int8_t ledIndex;

  if (firstTime) {
    DEBUG_PRINT_LN("Disco Baby!");
    firstTime = false;
    patternRunning = true;
    globalPatternLoops = loops * 2;
    ledPatternState = 0;
    // Clear the display the first time through
    allOFF(true);
  }

  updateLed = 0;

  if (checkDelay()) {
    switch (ledPatternState) {
      case 0:
        // We're going to set multiple pixels
        for (int s = 0; s < numSparkles; s++)
        {
          // Get a random row and column
          randRow = random(0, LEDS_PER_COLUMN);
          randCol = random(0, COLUMNS);
    
          //DEBUG_PRINT("Displaying at :"); DEBUG_PRINT(randRow); DEBUG_PRINT(" "); DEBUG_PRINT_LN(randCol);
    
          // set the LED in that row/column to the color
          ledIndex = ledMatrix[randCol][randRow];
          if (ledIndex != -1) {
            leds[ledIndex] = color;
          }
        }
        updateLed = 1;
        break;
        case 1: allOFF(false); updateLed = 1; break;
        default: break;
    }

    // Toggle the state.
    ledPatternState = ledPatternState ^ 1;
    globalPatternLoops--;
  }

  if (updateLed) {
    FastLED.show(brightness());
    set_delay(time_delay);
  }
  
  // Check to see if we have run the loops needed for this pattern
  loopsDonedoRestoreDefault();
}

void lightsaberBattle(unsigned long time_delay)
{
    if (firstTime) {
    DEBUG_PRINT_LN("Lightsaber Battle");
    firstTime = false;
    patternRunning = true;
    ledPatternState = 0;
    // Clear the display the first time through
    allOFF(true);
  }

  updateLed = 0;

  if (checkDelay()) {
    switch (ledPatternState) {
      // Note we set the display timeout large here so that the image stays displayed.
      case 0: displayMatrixColor(lightsaber0, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 1; updateLed = 1; break;
      case 1: displayMatrixColor(lightsaber1, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 2; updateLed = 1; break;
      case 2: displayMatrixColor(lightsaber2, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 3; updateLed = 1; break;
      case 3: displayMatrixColor(lightsaber3, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 4; updateLed = 1; break;
      case 4: displayMatrixColor(lightsaber4, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 5; updateLed = 1; break;
      case 5: displayMatrixColor(lightsaber5, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 6; updateLed = 1; break;
      case 6: displayMatrixColor(lightsaber6, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc, 0x999999); ledPatternState = 7; updateLed = 1; time_delay=100; break;
      case 7: displayMatrixColor(lightsaber4, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 8; updateLed = 1; break;
      case 8: displayMatrixColor(lightsaber7, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 9; updateLed = 1; break;
      case 9: displayMatrixColor(lightsaber8, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 10; updateLed = 1; break;
      case 10: displayMatrixColor(lightsaber9, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 11; updateLed = 1; break;
      case 11: displayMatrixColor(lightsaber10, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 12; updateLed = 1; break;
      case 12: displayMatrixColor(lightsaber11, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 13; updateLed = 1; break;
      case 13: displayMatrixColor(lightsaber12, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 14; updateLed = 1; break;
      case 14: displayMatrixColor(lightsaber13, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 15; updateLed = 1; break;
      case 15: displayMatrixColor(lightsaber14, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc, 0x999999); ledPatternState = 16; updateLed = 1; time_delay=100; break;
      case 16: displayMatrixColor(lightsaber15, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 17; updateLed = 1; break;
      case 17: displayMatrixColor(lightsaber16, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 18; updateLed = 1; break;
      case 18: displayMatrixColor(lightsaber17, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 19; updateLed = 1; break;
      case 19: displayMatrixColor(lightsaber18, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 20; updateLed = 1; break;
      case 20: displayMatrixColor(lightsaber19, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 21; updateLed = 1; break;
      case 21: displayMatrixColor(lightsaber20, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 22; updateLed = 1; break;
      case 22: displayMatrixColor(lightsaber21, 0xffffff, 0x000000, true, 0, 0xff0000, 0x0000cc); ledPatternState = 23; updateLed = 1; time_delay=500; break;
      case 23: ledPatternState = 99; updateLed = 0; break;
      default: {
          // Do nothing.
          break;
        }
    }
  }

  if (updateLed) {
    FastLED.show(brightness());
    set_delay(time_delay);
  }

  // Check to see if we have run the loops needed for this pattern
  if ((ledPatternState == 99) && (!alwaysOn))
  {
    // Set back to the default pattern
    lastPSIeventCode = defaultPattern;
    patternRunning = false;
  }
}

// Delay, loops, color
void Pulse(unsigned long time_delay, uint8_t loops)
{
  // We use the VU_chart matrix to define the colors used
  // Then we'll simply display as many or as few of the pixels as we want
  // and use a random number to determine rise and fall.

  if (firstTime) {
    DEBUG_PRINT_LN("Pulse");
    firstTime = false;
    patternRunning = true;
    globalPatternLoops = loops;
    ledPatternState = 0;
    // Clear the display the first time through
    allOFF(true);
  }

  updateLed = 0;

  if (checkDelay()) {
    // read the display, but don't show it.
    // we'll blank out some pixels in a sec
    displayMatrixColor(pulse, 0x110000, 0x555555, false, 0);

    switch (ledPatternState) {
      // Note we set the display timeout large here so that the image stays displayed.
      case 0:  leds[33] = 0xff0000; ledPatternState = 1; updateLed = 1; break;
      case 1:  leds[32] = 0xff0000; ledPatternState = 2; updateLed = 1; break;
      case 2:  leds[16] = 0xff0000; ledPatternState = 3; updateLed = 1; break;
      case 3:  leds[30] = 0xff0000; ledPatternState = 4; updateLed = 1; break;
      case 4:  leds[36] = 0xff0000; ledPatternState = 5; updateLed = 1; break;
      case 5:  leds[45] = 0xff0000; ledPatternState = 6; updateLed = 1; break;
      case 6:  leds[38] = 0xff0000; ledPatternState = 7; updateLed = 1; break;
      case 7:  leds[28] = 0xff0000; ledPatternState = 8; updateLed = 1; break;
      case 8:  leds[20] = 0xff0000; ledPatternState = 9; updateLed = 1; break;
      case 9:  leds[8]  = 0xff0000; ledPatternState = 10; updateLed = 1; break;
      case 10: leds[5]  = 0xff0000; ledPatternState = 11; updateLed = 1; break;
      case 11: leds[6]  = 0xff0000; ledPatternState = 12; updateLed = 1; break;
      case 12: leds[23] = 0xff0000; ledPatternState = 0; updateLed = 1; globalPatternLoops--; break;
    }
  }

  if (updateLed) {
    FastLED.show(brightness());
    set_delay(time_delay);
  }
  
  // Check to see if we have run the loops needed for this pattern
  loopsDonedoRestoreDefault();
}

// Scrolling text getting smaller and dimming as it rises up the screen
void StarWarsIntro(unsigned long time_delay, uint8_t loops, CRGB color)
{

  if (firstTime) {
    DEBUG_PRINT_LN("Star Wars");
    firstTime = false;
    patternRunning = true;
    globalPatternLoops = loops;
    ledPatternState = 2;
    // Clear the display the first time through
    allOFF(true);
  }

  updateLed = 0;

  if (checkDelay()) {
    switch (ledPatternState) {
      // Note we set the display timeout large here so that the image stays displayed.
      case 0: fill_row(5, color); ledPatternState=1; updateLed = 1; break;
      case 1: allOFF(false);      ledPatternState=2; updateLed = 1; break;
      case 2: fill_row(4, color); ledPatternState=3; updateLed = 1; break;
      case 3: allOFF(false);      ledPatternState=4; updateLed = 1; break;
      case 4: //fill_row(5, color); 
              fill_row(3, color); 
              leds[33]=0x000000;
              leds[24]=0x000000; ledPatternState=5; updateLed = 1; break;
      case 5: allOFF(false);
              fill_row(4, color);
              fill_row(2, color, 100); 
              // Turn off some pixels to shrink row 2
              leds[14]=0x000000;
              leds[15]=0x000000;
              leds[22]=0x000000;
              leds[23]=0x000000;
              ledPatternState=6; updateLed = 1; break;
      case 6: allOFF(false); 
              //fill_row(5, color);
              fill_row(3, color);
              leds[33]=0x000000;
              leds[24]=0x000000;
              fill_row(2, color, 100); 
              // Turn off some pixels to shrink row 2
              leds[14]=0x000000;
              leds[15]=0x000000;
              leds[22]=0x000000;
              leds[23]=0x000000;
              fill_row(1, color, 20);
              // Turn off some pixels to shrink row 1 
              leds[13]=0x000000;
              leds[12]=0x000000;
              leds[7]=0x000000;
              leds[6]=0x000000;
              ledPatternState=7; updateLed = 1; break;
      case 7: allOFF(false); 
              fill_row(4, color);
              fill_row(2, color, 100);
              // Turn off some pixels to shrink row 2
              leds[14]=0x000000;
              leds[15]=0x000000;
              leds[22]=0x000000;
              leds[23]=0x000000;
              fill_row(1, color, 20);
              // Turn off some pixels to shrink row 1 
              leds[13]=0x000000;
              leds[12]=0x000000;
              leds[7]=0x000000;
              leds[6]=0x000000;
              fill_row(0, color, 12);
              // Turn off some pixels to shrink row 0 
              leds[0]=0x000000;
              leds[1]=0x000000;
              leds[4]=0x000000;
              leds[5]=0x000000;ledPatternState=8; updateLed = 1; break;
      case 8: ledPatternState=6; updateLed = 0; globalPatternLoops--; break;
      default: {
          // Do nothing.
          break;
        }
    }
  }

  if (updateLed) {
    FastLED.show(brightness());
    set_delay(time_delay);
  }

  // Check to see if we have run the loops needed for this pattern
  loopsDonedoRestoreDefault();

 //ledMatrix[i][row];
  
}

///////////////////
// OTHER HELPERS //
///////////////////

//This is the non-blocking delay function
// When called it sets some global variables to allow checking of timer exipration
// To check if the timer has expired, call checkDelay()
void set_delay(unsigned long timeout)
{
  doNext = millis() + timeout;
  //DEBUG_PRINT("Set delay to "); DEBUG_PRINT_LN(doNext);
}

// Call this to see if the timer for set_delay() has expired
bool checkDelay()
{
  bool timerExpired = false;
  if (millis() >= doNext) timerExpired = true;
  return timerExpired;
}

//set the global pattern timeout
void set_global_timeout(unsigned long timeout)
{
  globalTimeout = millis() + timeout;
  //DEBUG_PRINT("Set global delay to "); DEBUG_PRINT_LN(doNext);
}

// Check if the global timeout has expired.
// This will return true if the timer has expired.
// If "alwaysOn" is set, the function will never return true.
bool globalTimeoutExpired()
{
  bool timerExpired = false;
  if ((millis() >= globalTimeout) && (!alwaysOn)) timerExpired = true;
  return timerExpired;
}

void loopsDonedoRestoreDefault()
{
  // Check to see if we have run the loops needed for this pattern
  if ((globalPatternLoops == 0) && (!alwaysOn))
  {
    // Set back to the default pattern
    lastPSIeventCode = defaultPattern;
    patternRunning = false;
  }
}

void globalTimerDonedoRestoreDefault()
{
  if (globalTimeoutExpired()) {
    // Set the loops to 0 to catch any cases like that.
    globalPatternLoops = 0;
    // Global timeout expired, go back to default mode.
    lastPSIeventCode = defaultPattern;
    patternRunning = false;
  }
}

// The following takes the Pattern code, and executes the relevant function
// This allows i2c and serial inputs to use the same function to start patterns
// so we avoid the need to duplicate this code.
void runPattern(int pattern) {

  if (lastPSIeventCode != pattern)
  {
    lastPSIeventCode = pattern;
    firstTime = true;
  }
  else
  {
    firstTime = false;
  }

  switch (pattern) {
    case 0:              //  0 = Turns Panel Off
      allOFF(true);
      break;
    case 1:              //  1 = Default Swipe Pattern
      swipe();
      break;
    case 2:             // Flash Panel (4s)
      flash(0xffffff, 60, 21);
      break;
    case 3:             //  3 = Alarm
      flash(0xffffff, 125, 13);
      break;
    case 4:              //  4 = Short circuit
      FadeOut(250, 3);
      break;
    case 5:              //  5 = Scream - Note this is the same as Alarm currently!
      flash(0xffffff, 125, 13);
      break;
    case 6:              //  6 = Leia message (34s)
      Cylon_Row(0xcccccc, 48, 3, 59); //5
      break;
    case 7:              //  7 = I heart U
      i_heart_u(500, 3);
      break;
    case 8:              //  8 = Radar sweep
      radar(0xff0000, 250, 6);
      break;
    case 9:              //   = Flashing red heart
      if (digitalRead(JUMP_FRONT_REAR)) {
        // Display the beating heart on the front
        red_heart(500, 3); //3x1s 500ms on, 500ms off
      } else {
        // Display the Pulse on the back
        Pulse(100,3); //12x100ms per loop
      }
      break;
    case 10:              //  10 = Star Wars Animation
      Cylon_Row(0xC8AA00, 500, 4, 5);
      break;
    case 11:              //  11 = Imperial March (47s)
      march(0xffffff, 555, 42);
      break;
    case 12:          // 12 - VU Meter
      VUMeter(250, 20);
      break;
    case 13:          // 13 - Disco Ball
      DiscoBall(150, 30, 3, CRGB::White); //gray /30
      break;
    case 14:          // 14 - Rebel Symbol
      // Pass the matrix a main color and a background color
      displayMatrixColor(rebel, 0xff0000, 0x909497, true, 5000);
      break;
    case 15:        // 15 - Knight Rider
      Cylon_Col(0xff0000, 250, 1, 5);
      break;
    case 16:        // All LED's On White Indefinitely
      allON(CRGB::Gray, true);
      break;
    case 17:              //  17 - Turns Panel On Red Indefinitely
      allON(CRGB::Red, true);
      break;
    case 18:              //  18 - Turns Panel On Green Indefinitely
      allON(CRGB::Green, true);
      break;
    case 19:              //  19 - Complex animation test, Lightsaber Battle
      lightsaberBattle(250);
      break;
    case 20:             // 20 - Star Wars Intro Text
      StarWarsIntro(500, 4, 0xC8AA00);
      break;
    default:
      // Do nothing
      break;
  }

}

// function that executes whenever data is received from an I2C master
// this function is registered as an event, see setup()
void receiveEvent(int eventCode) {

  while (Wire.available()) {

    // New I2C handling
    // Needs to be tested, but uses the same parser as Serial!
    bool command_available;
    char ch = (char)Wire.read();

    DEBUG_PRINT("I2C Character received "); DEBUG_PRINT_LN(ch);
    
    command_available=buildCommand(ch, cmdString);  // build command line
      
    if (command_available) 
    {
      parseCommand(cmdString);  // interpret the command
    } 
  }
}


/*
   SerialEvent occurs whenever a new data comes in the
  hardware serial RX.  This routine is run between each
  time loop() runs, so using delay inside loop can delay
  response.  Multiple bytes of data may be available.
*/
void serialEventRun(void)
{
  //if (Serial.available()) serialEvent();
  if (Serial1.available()) serialEvent();
}

void serialEvent() {

  //while (Serial.available()) {   
  while (Serial1.available()) {

    DEBUG_PRINT_LN("Serial In");

    //char ch = (char)Serial.read();                     // get the new byte
    char ch = (char)Serial1.read();                     // get the new byte
    bool command_available;

    // New improved command handling
    command_available=buildCommand(ch, cmdString);  // build command line
    if (command_available) 
    {
      parseCommand(cmdString);  // interpret the command
    }
  }
  sei();
}


////////////////////////////////////////////////////////
// Command language - JawaLite emulation
///////////////////////////////////////////////////////


////////////////////////////////
// command line builder, makes a valid command line from the input
byte buildCommand(char ch, char* output_str)
{
  static uint8_t pos=0;
  switch(ch)
 {
    case '\r':                          // end character recognized
      output_str[pos]='\0';   // append the end of string character
      pos=0;        // reset buffer pointer
      return true;      // return and signal command ready
      break;
    default:        // regular character
      output_str[pos]=ch;   // append the  character to the command string
      if(pos<=CMD_MAX_LENGTH-1)pos++; // too many characters, discard them.
      break;
  }
  return false;
}

///////////////////////////////////
// command parser and switcher, 
// breaks command line in pieces, 
// rejects invalid ones, 
// switches to the right command
void parseCommand(char* inputStr)
{
  byte hasArgument=false;
  int argument;
  int address;
  byte pos=0;
  byte length=strlen(inputStr);
  if(length<2) goto beep;   // not enough characters
  
  // get the adress, one or two digits
  char addrStr[3];
  if(!isdigit(inputStr[pos])) goto beep;  // invalid, first char not a digit
    addrStr[pos]=inputStr[pos];
    pos++;                            // pos=1
  if(isdigit(inputStr[pos]))          // add second digit address if it's there
  {  
    addrStr[pos]=inputStr[pos];
    pos++;                            // pos=2
  }
  addrStr[pos]='\0';                  // add null terminator
  
  address= atoi(addrStr);        // extract the address

  //DEBUG_PRINT(" I think this is the address! ");
  //DEBUG_PRINT_LN(address);
  
  // check for more
  if(!length>pos) goto beep;            // invalid, no command after address
  
  // special case of M commands, which take a string argument
  // Not currently implemented!!!!!
  //if(inputStr[pos]=='M')
  //{
  //  pos++;
  //  if(!length>pos) goto beep;     // no message argument
  //  doMcommand(address, inputStr+pos);   // pass rest of string as argument
  //  return;                     // exit
  //}
  
  // other commands, get the numerical argument after the command character

  pos++;                             // need to increment in order to peek ahead of command char
  if(!length>pos) hasArgument=false; // end of string reached, no arguments
  else
  {
    for(byte i=pos; i<length; i++)
    {
      if(!isdigit(inputStr[i])) goto beep; // invalid, end of string contains non-numerial arguments
    } 
    argument=atoi(inputStr+pos);    // that's the numerical argument after the command character
    hasArgument=true;
  }
  
  // switch on command character
  switch(inputStr[pos-1])               // 2nd or third char, should be the command char
  {
    case 'T':
      if(!hasArgument) goto beep;       // invalid, no argument after command
      doTcommand(address, argument);      
      break;
    case 'D':                           // D command is weird, does not need an argument, ignore if it has one
    case 'A':                           // A command does the same as D command, so just fall though.
      doDcommand(address);
      break;
    case 'P':    
      if(!hasArgument) goto beep;       // invalid, no argument after command
      doPcommand(address, argument);
      break;
    //case 'R':    
    //  if(!hasArgument) goto beep;       // invalid, no argument after command
    //  doRcommand(address, argument);
    //  break;
    //case 'S':    
    //  if(!hasArgument) goto beep;       // invalid, no argument after command
    //  doScommand(address, argument);
    //  break;
    default:
      goto beep;                        // unknown command
      break;
  }
  
  return;                               // normal exit
  
  beep:                                 // error exit
    // Dont know what this does ... idnoring it for now!
    //serialPort->write(0x7);             // beep the terminal, if connected
    return;
}

////////////////////
// Command Executors

// various commands for states and effects
void doTcommand(int address, int argument)
{
  DEBUG_PRINT_LN();
  DEBUG_PRINT("Command: T ");
  DEBUG_PRINT("Address: ");
  DEBUG_PRINT(address);
  DEBUG_PRINT(" Argument: ");
  DEBUG_PRINT_LN(argument);  

  // If we are the front PSI, respond to 0 or 4
  if ((digitalRead(JUMP_FRONT_REAR)) && (address == 4))
  {
    runPattern(argument);
  }
  // If we are the rear PSI, respond to 0 or 5
  else if ((!digitalRead(JUMP_FRONT_REAR)) && (address == 5))
  {
    runPattern(argument);
  }
  else if (address == 0) runPattern(argument);
  else
  {
    DEBUG_PRINT("Address "); DEBUG_PRINT(address); DEBUG_PRINT_LN(" not recognised");
  }
  
}

void doDcommand(int address)
{
  DEBUG_PRINT_LN();
  DEBUG_PRINT("Command: D ");
  DEBUG_PRINT("Address: ");
  DEBUG_PRINT_LN(address); 

  runPattern(defaultPattern);
}

// alphabet switching
void doPcommand(int address, int argument)
{
  DEBUG_PRINT_LN();
  DEBUG_PRINT("Command: P ");
  DEBUG_PRINT("Address: ");
  DEBUG_PRINT(address);
  DEBUG_PRINT(" Argument: ");
  DEBUG_PRINT_LN(argument);  
  switch(address)
  {
    case 0:
      // Set the always on Mode
      if (argument == 0) {
        alwaysOn = false;
        EEPROM.write(alwaysOnAddress, 0);
        DEBUG_PRINT_LN("Disable always on mode ");
      }
      if (argument == 1) {
        alwaysOn = true;
        EEPROM.write(alwaysOnAddress, 1);
        DEBUG_PRINT_LN("Enable always on mode ");
      }
      break;
    case 1:
      // Use either the external POT or internal brightness value
      if (argument == 0){
        // Set the brightness using the POT
        internalBrightness = false;
        EEPROM.write(externalPOTAddress, 0);
        DEBUG_PRINT_LN("Use External POT ");
      }
      if (argument == 1){
        // Set the brightness using the internal value.
        internalBrightness = true;
        EEPROM.write(externalPOTAddress, 1);
        DEBUG_PRINT_LN("Use internal Brightness setting ");
      }
      break;
    case 2:
      //// Brightness Control ////
      //
      // This PSI CAN DRAW MORE POWER THAN YOUR USB PORT CAN SUPPLY!!
      // When using the USB connection on the Pro Micro to power the PSI (during programming
      // for instance) be sure to have the brightness POT turned nearly all the way COUNTERCLOCKWISE.  
      // Having the POT turned up too far when plugged into USB can damage the Pro Micro
      // and/or your computer's USB port!!!!
      // If you are connected to USB, KEEP THIS VALUE LOW, not higher than 20.
      if (argument > 200) globalBrightnessValue = 200;
      else globalBrightnessValue = argument;
      EEPROM.write(internalBrightnessAddress, globalBrightnessValue);
      
      DEBUG_PRINT("Setting brightness to: ");
      DEBUG_PRINT_LN(globalBrightnessValue);
      break;
    default:
      break;
  }  
}

// Brightness control
// This is where we'll read from the pot, etc
uint8_t brightness() {
  //LED brightness is capped at 200 (out of 255) to reduce heat and extend life of LEDs. 
  if (!internalBrightness) return map(analogRead(POT_BRIGHT_PIN), 0, 1024, 0, 200); 
  else return globalBrightnessValue;
}
