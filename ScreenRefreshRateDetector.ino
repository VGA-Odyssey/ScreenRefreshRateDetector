/*
 * Screen Refresh Rate Detector
 *
 * This sketch is for the ArduinOLED board. for more info, visit:
 * 
 * Johan.Vandegriff.net/ArduinOLED
 * 
 * It tests all the parts of the board:
 *   Screen
 *   Buttons
 *   Joystick
 *   Buzzer
 */

#include <Arduino.h>
#include <U8x8lib.h> //library to control the display in character mode
#include <pitches.h> //list of pitches for the buzzer
#include <ArduinOLED.h> //library to read the buttons and joysticks

//set up the display
//https://github.com/olikraus/u8g2/wiki/u8x8setupcpp
U8X8_SSD1306_128X64_NONAME_HW_I2C display(/* reset=*/ U8X8_PIN_NONE);


/*
 * function to display test to the screen
 * implements wrap-around and inverted text
 * if it reaches the end of the last line, the rest of the text is not shown
 */
void output(uint8_t y, uint8_t x, String text, boolean isInverted) {
  display.setInverseFont(isInverted);
  uint8_t i = x;
  uint8_t j = y;
  uint8_t n = 0;
  while (j < 8 && n < text.length()) {
    display.setCursor(i, j);
    display.print(text.charAt(n));
    n++;
    i++;
    if (i >= 16) {
      i = 0;
      j++;
    }
  }
}






#define HISTORY_LENGTH 32
uint16_t history[HISTORY_LENGTH];
uint8_t onOff[HISTORY_LENGTH];
unsigned long lastTime, currTime; // timestamps[HISTORY_LENGTH];
uint8_t risingEdgeIdx[HISTORY_LENGTH];
uint8_t i = 0, j, k, num;
uint16_t lastVal, val, minVal, maxVal, avgVal;
int16_t diff, avgDiff;
float dist, freq;


void setup() {
  ArduinOLED.begin(); //start the ArduinOLED library, which controls the buttons and joystick
  display.begin(); //start the u8x8, which controls the display in character mode
  display.setFont(u8x8_font_amstrad_cpc_extended_f); //set the font
//  display.setFont(u8x8_font_chroma48medium8_r); //an alternate font. there are a lot more of these available
  display.clear(); //clear the display
  //output a long string of 8*16 zeroes using inverted text.
  //the F("string") function stores strings in flash memory. More at the bottom of this page:
  //https://www.arduino.cc/en/Reference/PROGMEM
  output(0, 0, F("                                                                                                                                "), true );
//  delay(1000); //wait for 1 second
  display.clear(); //clear the display
//  //display the info text
//  output(0, 0, F("ArduinOLED v1.0 "), true );
//  output(2, 0, F("Johan.Vandegriff"), false);
//  output(3, 0, F(".net/ArduinOLED "), false);
//  output(5, 0, F("  Test Program  "), true );
//  //wait for each button to be pressed, then play a tone
//  output(7, 0, F("Press A         "), false);
//  tone(BUZZER_PIN, NOTE_G3, 100); //BUZZER_PIN is defined as pin 3 in the ArduinOLED library
//  ArduinOLED.pause(BTN_A);
//  tone(BUZZER_PIN, NOTE_A3, 100);
//  output(7, 6, F("B"), false);
//  ArduinOLED.pause(BTN_B);
//  tone(BUZZER_PIN, NOTE_B3, 100);
//  output(7, 6, F("X"), false);
//  ArduinOLED.pause(BTN_X);
//  tone(BUZZER_PIN, NOTE_C4, 100);
//  output(7, 6, F("Y"), false);
//  ArduinOLED.pause(BTN_Y);
//  tone(BUZZER_PIN, NOTE_D4, 100);
//  output(7, 6, F("H"), false);
//  ArduinOLED.pause(BTN_H);
//  tone(BUZZER_PIN, NOTE_E4, 100);
//  output(7, 6, F("L"), false);
//  ArduinOLED.pause(BTN_L);
//  tone(BUZZER_PIN, NOTE_FS4, 100);
//  output(7, 6, F("R"), false);
//  ArduinOLED.pause(BTN_R);
//  tone(BUZZER_PIN, NOTE_G4, 100);
//  output(7, 6, F("U"), false);
//  ArduinOLED.pause(BTN_U);
//  tone(BUZZER_PIN, NOTE_A4, 100);
//  output(7, 6, F("D"), false);
//  ArduinOLED.pause(BTN_D);
//  tone(BUZZER_PIN, NOTE_B4, 100);
//  output(6, 0, F("Press Z (joy-   stick button)"), false);
//  ArduinOLED.pause(BTN_Z);
//  tone(BUZZER_PIN, NOTE_C5, 100);
//  output(6, 0, F("Move the joy-   stick left   "), false);
//  ArduinOLED.pause(BTN_JOY_L);
//  tone(BUZZER_PIN, NOTE_D5, 100);
//  output(7, 6, F("right"), false);
//  ArduinOLED.pause(BTN_JOY_R);
//  tone(BUZZER_PIN, NOTE_E5, 100);
//  output(7, 6, F("up   "), false);
//  ArduinOLED.pause(BTN_JOY_U);
//  tone(BUZZER_PIN, NOTE_FS5, 100);
//  output(7, 6, F("down"), false);
//  ArduinOLED.pause(BTN_JOY_D);
//  tone(BUZZER_PIN, NOTE_G5, 100);
//
//  //after the buttons have all been pressed, show the joystick values
//  output(6, 0, F("Joystick X:     "), false );
//  output(7, 0, F("Joystick Y:     "), false );
  Serial.begin(9600);
}


String str;

void loop() {

  history[i] = analogRead(A0);
  lastTime = currTime;
  currTime = millis();
//  timestamps[i] = millis();

//  num = 0;
//  avgDiff = 0;
//  for (j = 1; j < HISTORY_LENGTH; j++) {
//    if (j != i && j - 1 != i) {
//      diff = timestamps[j] - timestamps[j - 1];
//      //    Serial.print((int) diff);
//      //    Serial.print(",");
//      avgDiff += diff;
//      num++;
//    }
//  }
//  avgDiff /= num*1.0;
  avgDiff = currTime - lastTime;

  minVal = history[0];
  maxVal = minVal;
  for (j = 1; j < HISTORY_LENGTH; j++) {
    val = history[j];
    if (val < minVal) minVal = val;
    if (val > maxVal) maxVal = val;
  }
  avgVal = (minVal + maxVal) / 2;
  onOff[i] = (history[i] > avgVal);
  k = 0;
  for (j = 1; j < HISTORY_LENGTH; j++) {
    lastVal = onOff[j - 1];
    val = onOff[j];
    if (!lastVal && val) {
      //  Serial.print(j);
      //  Serial.print("|");
      risingEdgeIdx[k] = j;
      k++;
    }
  }
  dist = 0;
  if (k > 1) {
    for (j = 1; j < k; j++) {
      diff = avgDiff * abs(risingEdgeIdx[j - 1] - risingEdgeIdx[j]); //timestamps[risingEdgeIdx[j - 1]] - timestamps[risingEdgeIdx[j]];
      dist += diff / (k-1.0);

      //    Serial.print(risingEdgeIdx[j]);
      //    Serial.print(",");
    }
  }

  if (dist == 0.0) {
    freq = -1;
  } else {
    freq = 1000.0/dist;
  }
/*
  Serial.print(history[i]);
  Serial.print(",");
  //  Serial.print(minVal);
  //  Serial.print(",");
  //  Serial.print(maxVal);
  //  Serial.print(",");
  Serial.print(avgVal);
  Serial.print(",");
  Serial.print(freq);
//  Serial.print(dist);
  //  Serial.print(k);
  //  Serial.print(",");
  //  int x = i-1;
  //  if (x == -1) x = HISTORY_LENGTH;
  //  Serial.print(timestamps[i] - timestamps[x]);
  //  Serial.print(",");
  //  Serial.print(5*onOff);
  //
  Serial.println();*/

  str = freq;
  str += " Hz      ";
  if (i == 0 && freq != -1.0) output(1, 1, str, false);
  Serial.print(history[i]);
  Serial.print(",");
  Serial.println(freq);
  i++;
  if (i >= HISTORY_LENGTH) i = 0;
  delay(1);
//
//  
//  //convert the joystick x and y to a string
//  x = ArduinOLED.joyX;
//  y = ArduinOLED.joyY;
//
//  //put spaces on the end of each string to make it 4 chars long
//  if (x.length() == 1) {
//    x += "   ";
//  } else if (x.length() == 2) {
//    x += "  ";
//  } else if (x.length() == 3) {
//    x += " ";
//  }
//  if (y.length() == 1) {
//    y += "   ";
//  } else if (y.length() == 2) {
//    y += "  ";
//  } else if (y.length() == 3) {
//    y += " ";
//  }
//
//  //display the strings
//  output(6, 12, x, false);
//  output(7, 12, y, false);
}
