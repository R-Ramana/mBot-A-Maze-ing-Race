#include <MeMCore.h>
MeRGBLed led(0, 30);
MeLightSensor lightSensor(PORT_8);

//floats to hold colour arrays
float colourArray[] = {0,0,0};
float whiteArray[] = {0,0,0};
float blackArray[] = {323.00,253.00,261.00};
float greyDiff[] = {127.00,98.00,102.00};
float testColour[] = {0,0,0};

void setup(){
  Serial.begin(9600);
  led.setpin(13);
  setBalance();                                   // Calibrate Black white grey values
  char colourStr[3][6] = {"Red", "Green", "Blue"};
  int currentReading;
  int reading[100];
  //On red, green, blue one at a time
  for(int c=0; c<=2; c++) {
    int testcolourtotalreading = 0;
    int testcolouravgresult = 0;
    int count = 0;
    Serial.println(colourStr[c]);
    
    if(c == 0)
      redlight();
    else if(c == 1)
      greenlight();
    else if(c == 2)
      bluelight();

    for(int i = 0; i< 100; i++) {                  //taking one sample at a time, total of 100 samples
      currentReading = (lightSensor.read() - blackArray[c])/(greyDiff[c])*255;
      if (currentReading > 255) {
        currentReading = 255;
      }
      if (currentReading >= 0) {
        count++;
        testcolourtotalreading += currentReading;
        printColour(currentReading);
      }
    }
    testcolouravgresult = testcolourtotalreading / count;
    Serial.print("Average Result:");
    Serial.println(testcolouravgresult);
    Serial.print("\n");
    testColour[c] = testcolouravgresult;
    nolight();                                          //turn off led
  }

  // Colour detection
  if( (testColour[0]<=50) && (testColour[1]<=50) && (testColour[2]<=50) )
    Serial.println("\nColour detected Black");
  else if( (testColour[0]<=100) && (testColour[1]<=200) && (testColour[2]<=130) )
    Serial.println("\nColour detected Green");
  else if( (testColour[0]<=255) && (testColour[1]<=125) && (testColour[2]<=120) )
    Serial.println("\nColour detected Red");
  else if( (testColour[0]<=170) && (testColour[1]<=195) && (testColour[2]<=240) )
    Serial.println("\nColour detected Purple");
  else if( (testColour[0]<=200) && (testColour[1]<=255) && (testColour[2]<=255) )
    Serial.println("\nColour detected Blue");
  else if( (testColour[0]<=255) && (testColour[1]<=255) && (testColour[2]<=180) )
    Serial.println("\nColour detected Yellow");
  else
    Serial.println("\nColour detected White");
}


// Ommited. Average values was used instead
int getPopularElement(int a[]) {
  int count = 1, tempCount;
  int popular = a[0];
  int temp = 0;
  for (int i = 0; i < (100 - 1); i++)
  {
    temp = a[i];
    tempCount = 0;
    for (int j = 1; j < 100; j++)
    {
      if (temp == a[j])
        tempCount++;
    }
    if (tempCount > count)
    {
      popular = temp;
      count = tempCount;
    }
  }
  return popular;
}

void loop(){
}

void redlight(){
  led.setColor(255, 0, 0); //Set both LED to Red
  led.show();              //Must use .show() to make new colour take effect.
}
void greenlight(){
  led.setColor(0, 255, 0); //Set both LED to Green
  led.show();              //Must use .show() to make new colour take effect.
}
void bluelight(){
  led.setColor(0, 0, 255); //Set both LED to Blue
  led.show();              //Must use .show() to make new colour take effect.
}
void nolight(){
  led.setColor(0, 0, 0);   //Set both LED to None
  led.show();              //Must use .show() to make new colour take effect.
}
void printColour(int reading){       //prints the colour in the colourArray on the Serial Monitor
  Serial.print(millis());
  Serial.print(",");
  Serial.println(reading);
}

// Calibration
void setBalance(){
  //set white balance
  Serial.println("Put White Sample For Calibration ...");
  delay(5000);     //delay for five seconds for getting sample ready
  nolight();
  //scan the white sample.
  //go through one colour at a time, set the maximum reading for each colour -- red, green and blue to the white array
  for(int i = 0;i<=2;i++){
     if(i == 0){
      redlight();
      delay(100);
      whiteArray[i] = getAvgReading(5);
      nolight();
      delay(100);
     }
     if(i == 1){
      greenlight();
      delay(100);
      whiteArray[i] = getAvgReading(5);
      nolight();
      delay(100);
     }
     if(i == 2){
      bluelight();
      delay(100);
      whiteArray[i] = getAvgReading(5);
      nolight();
      delay(100);
     }
  }
//done scanning white, time for the black sample.

//set black balance
  Serial.println("Put Black Sample For Calibration ...");
    delay(5000);     //delay for five seconds for getting sample ready 
//go through one colour at a time, set the minimum reading for red, green and blue to the black array
  for(int i = 0;i<=2;i++){
    if(i == 0){
      redlight();
      delay(100);
      blackArray[i] = getAvgReading(5);
      nolight();
      delay(100);
     }
     if(i == 1){
      greenlight();
      delay(100);
      blackArray[i] = getAvgReading(5);
      nolight();
      delay(100);
     }
     if(i == 2){
      bluelight();
      delay(100);
      blackArray[i] = getAvgReading(5);
      nolight();
      delay(100);
     }
     //the differnce between the maximum and the minimum gives the range
     greyDiff[i] = whiteArray[i] - blackArray[i];
  }
  //delay another 5 seconds to allow us to prepare the colour objects
  Serial.println("Put Colour Sample For Data Collection  ...");
  delay(5000); //get ready the colour paper

  Serial.print("White : ");
  Serial.print(whiteArray[0]);
  Serial.print(" ");
  Serial.print(whiteArray[1]);
  Serial.print(" ");
  Serial.println(whiteArray[2]);
  
  Serial.print("Black : ");
  Serial.print(blackArray[0]);
  Serial.print(" ");
  Serial.print(blackArray[1]);
  Serial.print(" ");
  Serial.println(blackArray[2]);
  
  Serial.print("Grey : ");
  Serial.print(greyDiff[0]);
  Serial.print(" ");
  Serial.print(greyDiff[1]);
  Serial.print(" ");
  Serial.println(greyDiff[2]);
}

int getAvgReading(int times){      
  //find the average reading for the requested number of times of scanning LDR
  int reading;
  int total = 0;
  //take the reading as many times as requested and add them up
  for(int i = 0;i < times;i++){
     reading = lightSensor.read();
     total = reading + total;
     delay(10);
  }
  //calculate the average and return it
  return total/times;
}
  
