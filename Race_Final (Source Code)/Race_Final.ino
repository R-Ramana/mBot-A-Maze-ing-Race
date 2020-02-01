#include <MeMCore.h>

// Button
#define STARTBUTTON 7

// Buzzer
MeBuzzer buzzer;

// Motors
MeDCMotor motor1(M1);                               // Left Motor
MeDCMotor motor2(M2);                               // Right Motor
#define MOTORSPEED 255                              // Speed of mBot
#define LEFTTURN 225                                // Delay for left turn
#define RIGHTTURN 225                               // Delay for right turn
#define UTURN 470                                   // Delay for u turn

// Black sensor
MeLineFollower lineFinder(PORT_2);

// Light Sensor
MeRGBLed led(0, 30);
MeLightSensor lightSensor(PORT_8);
//floats to hold colour arrays
float blackArray[] = {323.00,253.00,261.00};        // Preset black values
float greyDiff[] = {127.00,98.00,102.00};           // Preset grey values
float colourData[] = {0,0,0};

// Ultrasonic sensor
MeUltrasonicSensor ultrasonicSensor(PORT_1);
#define DISTSPACE 12.0                              // Distance to detect wall

// Infrared LED and Detector
#define LEFTIR  A0                                  // Infrared Dectector left can use any A0 ~ A5
#define RIGHTIR  A1                                 // Infrared Dectector right
#define IRDISTSPACE 3.2                             // As wall get closer to sensor,
                                                    // voltage increases, 3.2V threshold

// Mic Sensor
#define MIC A2

void setup() {
  Serial.begin(9600);
  pinMode(STARTBUTTON, INPUT);
  pinMode(LEFTIR,INPUT);
  pinMode(RIGHTIR,INPUT);
  pinMode(MIC, INPUT);
  while (analogRead(STARTBUTTON) > 100)             // Infinite loop until button is pressed
    delay(50);
}

void loop() {
  forward();
}

///////////////////////////
// Black Sensor Function //
///////////////////////////
int blackLine() {
  int sensorState = lineFinder.readSensors();
  switch (sensorState) {
    case 3 : return 0;
    default : return 1;
  }
}

/////////////////////////
// Movements Functions //
/////////////////////////
void forward() {
  int lValue, rValue;
  float lVoltage, rVoltage;
  
  lValue = analogRead(LEFTIR);                      // Reads how far the left wall is
  rValue = analogRead(RIGHTIR);                     // Reads how far the right wall is
  lVoltage = lValue * (5.0 / 1023.0);               // Get values in voltage
  rVoltage = rValue * (5.0 / 1023.0);
  Serial.println(rVoltage);
  
  if (blackLine()) {                                // Black stripe detection
    motor1.stop();
    motor2.stop();
    Serial.println("Black Strip Detected");
    delay(1000);
    colourScanner();
  } 
  else if (lVoltage >= IRDISTSPACE) {               // Wall detection
      motor1.stop();
      motor2.run(-100);
  }
  else if (rVoltage >= IRDISTSPACE) {               // Wall detection
      motor1.run(100);
      motor2.stop();
  }
  else {                                            // Forward
    motor1.run(-MOTORSPEED);
    motor2.run(MOTORSPEED);
  }
}

void movements(char colour) {
  int freqResult;
  switch(colour) {
    case 'B':                                       // Black paper
      freqResult = frequencyCheck();                // Scan for sound

      if (freqResult <= 30) {                       // Minimal (No) freq
        celebration();
      } else if (freqResult < 130) {                // Low freq
        turns('L', 1);
      } else if (freqResult <= 300) {               // High freq
        turns('R', 1);
      } else {
        movements('B');                             // Rescan sound
      }
      break;
      
    case 'R':                                       // Red paper
      turns('L', 1);
      break;
      
    case 'G':                                       // Green paper
      turns('R', 1);
      break;
      
    case 'Y':                                       // Yellow paper
      turns('R', 2);
      break;
      
    case 'L':                                       // Light Blue paper
      turns('R', 1);
      while (ultrasonicSensor.distanceCm() > DISTSPACE){ // Scan for wall
         forward();                                 // if dist > distance from sensor, move forward
      }
      motor1.stop();
      motor2.stop();
      delay(200);
      
      turns('R', 1);                                // Turn again
      break;
      
    case 'P':
      turns('L', 1);
      while (ultrasonicSensor.distanceCm() > DISTSPACE){ // Scan for wall
         forward();                                 // if dist > distance from sensor, move forward
      }
      motor1.stop();
      motor2.stop();
      delay(200);
      
      turns('L', 1);                                // Turn again
      break;
  }
}

void turns(char direct, int angle) {
    if (direct == 'R' && angle == 1) {              // Right turn
      Serial.println("RIGHT TURN");
      motor1.run(-MOTORSPEED);
      motor2.run(-MOTORSPEED);
      delay(RIGHTTURN);
    }
    else if (direct == 'L' && angle == 1) {         // Left turn
      Serial.println("LEFT TURN");
      motor1.run(MOTORSPEED);
      motor2.run(MOTORSPEED);
      delay(LEFTTURN);
    }
    else if (direct == 'R' && angle == 2) {         // U-TURN
      Serial.println("180 TURN");
      motor1.run(-MOTORSPEED);
      motor2.run(-MOTORSPEED);
      delay(UTURN);
    }
  motor1.stop();                                    // Stop
  motor2.stop();
}

////////////////////////////
// Light Sensor Functions //
////////////////////////////
void colourScanner() {
  led.setpin(13);
  char colourStr[3][6] = {"Red", "Green", "Blue"};
  int reading;
  
  for(int c=0; c<=2; c++) {                         // Flash red, green, blue one at a time
    int total = 0, average = 0, count = 0;

    Serial.println(colourStr[c]);
    
    if(c == 0)
      redlight();
    else if(c == 1)
      greenlight();
    else if(c == 2)
      bluelight();

    for(int i=0; i<100; i++) {                      // taking one sample at a time, total of 100 samples
      reading = (lightSensor.read() - blackArray[c])/(greyDiff[c])*255;
      if (reading > 255) {
        reading = 255;
      }
      if (reading >= 0) {
        count++;
        total += reading;
        printColour(reading);
      }
    }
    
    average = total / count;                        // Get average readings
    if (count < 0) {
      average = 0;
    }
    Serial.print("Average Result:");
    Serial.println(average);
    Serial.print("\n");
    colourData[c] = average;
    nolight();                                      // Turn off led 
  }
  colourCheck(colourData);                          // Check for action based on colours
}

void colourCheck(float colourData[]) {
  if ( (colourData[0]<= 50) && (colourData[1]<=50) && (colourData[2]<=50) ) {
    Serial.println("\nColour detected Black");
    movements('B');
  }
  else if ( (colourData[0]<=100) && (colourData[1]<=200) && (colourData[2]<=130)) {
    Serial.println("\nColour detected Green");
    movements('G');
  }
  else if ( (colourData[0]<=255) && (colourData[1]<=125) && (colourData[2]<=120) ) {
    Serial.println("\nColour detected Red");
    movements('R');
  }
  else if ( (colourData[0]<=170) && (colourData[1]<=195) && (colourData[2]<=240) ) {
    Serial.println("\nColour detected Purple");
    movements('P');
  }
  else if ( (colourData[0]<=200) && (colourData[1]<=255) && (colourData[2]<=255) ) {
    Serial.println("\nColour detected Light Blue");
    movements('L');
  }
  else if ( (colourData[0]<=255) && (colourData[1]<=255) && (colourData[2]<=180) ) {
    Serial.println("\nColour detected Yellow");
    movements('Y');
  }
  else {
    colourScanner();                                // Rescan for colour
  }
}

void redlight(){
  led.setColor(255, 0, 0);                          // Set both LED to Red
  led.show();                                       // Use .show() to make new colour take effect.
}
void greenlight(){
  led.setColor(0, 255, 0);                          // Set both LED to Green
  led.show();
}
void bluelight(){
  led.setColor(0, 0, 255);                          // Set both LED to Blue
  led.show();
}
void nolight(){
  led.setColor(0, 0, 0);                            // Set both LED to None
  led.show();
}
void printColour(int reading){
  Serial.print(millis());
  Serial.print(",");
  Serial.println(reading);
}

//////////////////////////
/// Microphone Function //
//////////////////////////
int frequencyCheck() {
  float reading = 0,
        count = 0;
        
  for (int i=0; i<300; i++) {                       // Read 300 times to read freq
    reading = analogRead(MIC) * (5/1023.0) * 1000;
    if (reading >= 30) {                            // If it has a reading of 30 or higher
      count++;                                      // Add into counter
    }
    delay(10);
    Serial.print("Readings : ");
    Serial.println(reading);
  }
  Serial.print("Count : ");
  Serial.println(count);
  
  return count;
}

/////////////////////
// Buzzer Function //
/////////////////////
void celebration() {                                // Nokia tone
  Serial.println("END");
  buzzer.tone(1318.50, 125);   
  buzzer.tone(1174.70, 125);   
  buzzer.tone(739.99, 250);  
  buzzer.tone(830.61, 250);   
  
  buzzer.tone(1108.70, 125);   
  buzzer.tone(987.77, 125);   
  buzzer.tone(587.33, 250);
  buzzer.tone(659.26, 250); 
  
  buzzer.tone(987.77, 125); 
  buzzer.tone(880.00, 125);   
  buzzer.tone(554.37, 250);   
  buzzer.tone(659.26, 250);   
  
  buzzer.tone(880.00, 750); 
  
  
  buzzer.tone(1318.50, 125);   
  buzzer.tone(1174.70, 125);   
  buzzer.tone(739.99, 250);  
  buzzer.tone(830.61, 250);   
  
  buzzer.tone(1108.70, 125);   
  buzzer.tone(987.77, 125);   
  buzzer.tone(587.33, 250);
  buzzer.tone(659.26, 250); 
  
  buzzer.tone(987.77, 125); 
  buzzer.tone(880.00, 125);   
  buzzer.tone(554.37, 250);   
  buzzer.tone(659.26, 250);   
  
  buzzer.tone(880.00, 750); 
  
  
  buzzer.tone(1318.50, 125);   
  buzzer.tone(1174.70, 125);   
  buzzer.tone(739.99, 250);  
  buzzer.tone(830.61, 250);   
  
  buzzer.tone(1108.70, 125);   
  buzzer.tone(987.77, 125);   
  buzzer.tone(587.33, 250);
  buzzer.tone(659.26, 250); 
  
  buzzer.tone(987.77, 125); 
  buzzer.tone(880.00, 125);   
  buzzer.tone(554.37, 250);   
  buzzer.tone(659.26, 250);   
  
  buzzer.tone(880.00, 750);

  while(1){}                                        // Infinite loop END
}
