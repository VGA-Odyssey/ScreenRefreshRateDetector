/*
  Screen Refresh Rate Detector

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

#define HISTORY_LENGTH 32
uint16_t history[HISTORY_LENGTH];
uint8_t onOff[HISTORY_LENGTH];
unsigned long lastTime, currTime; // timestamps[HISTORY_LENGTH];
uint8_t risingEdgeIdx[HISTORY_LENGTH];
uint8_t i = 0, j, k, num;
uint16_t lastVal, val, minVal, maxVal, avgVal;
int16_t diff, avgDiff;
float dist, freq;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
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
  Serial.println(freq);
  i++;
  if (i >= HISTORY_LENGTH) i = 0;
  delay(1);
}
