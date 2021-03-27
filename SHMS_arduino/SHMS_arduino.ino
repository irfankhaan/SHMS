#define Type DHT11
#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

const int PulseWire = 0;      // input signal read pin for pulse sensor (analog pin)
int tempPin = 1, temp_val, sensePin = 12, setTime = 500, myBPM, Threshold = 550; // tempPin for LM35 SENSOR, sensPin for DHT111 sensor, setTime for delay, myBPM and Threshold for pulse sensor
float mv, cel, room_temp, humidity;     // variables for sesnor inputs
String values;                // string to be printed in serial monitor

LiquidCrystal LCD(10,9,5,4,3,2);
DHT HT(sensePin,Type);
PulseSensorPlayground pulseSensor;

void setup() {
  Serial.begin(9600);

  LCD.begin(16,2);
  
  HT.begin();
  delay(setTime); 

  pulseSensor.analogInput(PulseWire);     // set analogInput for pulse sensor to the PulseWire pin (analog pin)
  pulseSensor.setThreshold(Threshold);    // set threshold value 
  pulseSensor.begin();                    // begin pulse sensor playground
}

void loop() {

  values= ("BPM "+get_bpm()+','+"Body-Temperature "+get_body_temp()+','+"Humidity "+get_hum_Value()+','+"Room-Temperature "+get_temp_Value()+"\n");   // print all sensor values in serial monitor
       delay(1000);
       // removed any buffered previous serial data.
       Serial.flush();
       delay(1000);
       // sent sensors data to serial (sent sensors data to ESP8266)
       Serial.print(values);
       delay(2000);

}

String get_hum_Value(){ 
    LCD.setCursor(0,0);
    LCD.print("                "); 

    delay(100);
    humidity = HT.readHumidity();   // get humidity value from DHT111 sensor
    
    
    LCD.setCursor(0,0);
    LCD.print("Humidity: ");
    LCD.setCursor(10,0);
    LCD.print(humidity);          // print humidity value on LCD
    delay(1000);
    
    return String(humidity);      // return value of humidity to void loop
}

String get_temp_Value(){
    LCD.setCursor(0,1);
    LCD.print("                ");

    delay(100);
    room_temp = HT.readTemperature();    // get room temperature from DHT111 sensor
    

    LCD.setCursor(0,1);
    LCD.print("Room Temp ");
    LCD.setCursor(11,1);
    LCD.print(room_temp);         // print room temperature value on LCD
    delay(1000);
    
    return String(room_temp);     // return value of room temperature to void loop
}

String get_body_temp(){
    LCD.setCursor(0,0);
    LCD.print("                ");
    
    delay(100);
    temp_val = analogRead(tempPin);     // read temperature value from LM35
    mv = ( temp_val/1023.0)*5000;            // convert voltage value to temperature
    cel = mv/10;                        // convert temperature to celcius

    LCD.setCursor(0,0);
    LCD.print("Body Temp ");     
    LCD.setCursor(11,0);
    LCD.print(cel);             // print celsius temperature value on LCD

    return String(cel);         // return celcius temerature value to void loop
}

String get_bpm(){
    LCD.setCursor(0,1);
    LCD.print("                ");

    myBPM = pulseSensor.getBeatsPerMinute();      // get BPM from sensor 
    while(!pulseSensor.sawStartOfBeat()){         // check if sensible value of BPM is available 
      myBPM = pulseSensor.getBeatsPerMinute();    // update BPM from sensor
      delay(20);
      }

    LCD.setCursor(0,1);
    LCD.print("BPM: ");
    LCD.setCursor(10,1);
    LCD.print(myBPM);             // print BPM on LCD

    return String(myBPM);         // return BPM to void loop
}
