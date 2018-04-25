#include <Wire.h>
#include "MAX30105.h"

#include "heartRate.h"
#include <Wire.h>
#include "MAX30105.h"
#include "MovingAverage.h"
#include <math.h>

MAX30105 particleSensor;

MovingAverage average(0.1f);

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;
double bpm;
double past_values[10];
uint8_t x=0;
  
void setup()
{
  Serial.begin(115200);
  
  //average.reset( particleSensor.getIR() );
  
  Serial.println("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

void loop()
{
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 100 && beatsPerMinute > 60)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
      
    }


  }


  //Serial.print("IR=");
  //Serial.print(irValue);
  //Serial.print(", BPM=");
  //Serial.print(beatsPerMinute);
  //Serial.print("Avg BPM=");
  
  bpm = average.update( beatAvg );
  //Serial.println(bpm);
  if(x<10){
    past_values[x]=bpm;
    x++;
    //if 10 past values are the same print the value
    if(past_values[0]==past_values[1] && past_values[1]==past_values[2] && past_values[2]==past_values[3] && past_values[3]==past_values[4] && past_values[4]==past_values[5] && past_values[5]==past_values[6] && past_values[6]==past_values[7])
    {
      Serial.println("");
      Serial.println("");
      Serial.println("");
      Serial.println("");
      Serial.print("THIS IS THE BPM VALUE:   ");
      Serial.println(past_values[0]);
    }
    }
  //flush values after 10 iterations
  if(x==10){
    x=0;
    for(int i=0;i<10;i++){
      Serial.print("Value ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(past_values[i]);
      Serial.print(" ");
      past_values[i]=0;
    }
    Serial.println(" ");
  }
  
  //Serial.println( average.update( particleSensor.getIR() ) );
  //Serial.println(beatsPerMinute);
  //Serial.print("Average hearbeat ");
  //Serial.print(beatAvg);
  //Serial.print(irValue);

    
  //if (irValue < 50000)
   // Serial.print(" No Grace, no life?");
  //Serial.println();

  
}

//Threshold Function
//lag = 30;
//threshold = 5;
//influence = 0;

//void Threshold(){


//}
