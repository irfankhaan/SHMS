#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define FIREBASE_AUTH "YOUR_API_KEYS"   // Enter Firebase API keys/secret here
#define FIREBASE_HOST "YOUR_PROJECT-default-rtdb.firebaseio.com"   // Enter host URL
#define WIFI_SSID "SSID"   // Enter WiFi SSID
#define WIFI_PASSWORD "PASSWORD"    // Enter WiFi passwords

const long utcOffsetInSeconds = 19800;    // UTC offset for GMT +5:30
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);

String patient_id = "YpZ4JfZNT1YMnsecoqswsTJ7NO63";    // Unique patient ID
String sensor_data, bpm, btemp, hum, temp;
float bpm_val, btemp_val, hum_val, temp_val;

void setup() {
   Serial.begin(9600);
   
   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
    }
  timeClient.begin();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); 
  
  
}
void loop() {
  
  bool Sr =false;
 
  while(Serial.available()){
        sensor_data=Serial.readString();   // Get sensor data from serial put in sensor_data
        Sr=true;          
    }
  
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();

  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;
  String currentDate = String(currentMonth) + "-" + String(monthDay) + "-" + String(currentYear);

  delay(1000);
  
  if(Sr==true){  

  bpm = sensor_data.substring(0, sensor_data.indexOf(",")); // Parse data from serial monitor
  bpm_val = bpm.substring(bpm.indexOf(" ")+1).toFloat();
  
  sensor_data.remove(0, sensor_data.indexOf(",")+1);

  btemp = sensor_data.substring(0, sensor_data.indexOf(","));
  btemp_val = btemp.substring(btemp.indexOf(" ")+1).toFloat();
  sensor_data.remove(0, sensor_data.indexOf(",")+1);

  hum = sensor_data.substring(0, sensor_data.indexOf(","));
  hum_val = hum.substring(hum.indexOf(" ")+1).toFloat();
  
  temp = sensor_data.substring(sensor_data.indexOf(","), sensor_data.indexOf("\n"));
  temp_val = temp.substring(temp.indexOf(" ")+1).toFloat();

  Firebase.setFloat("/users/"+patient_id+"/readings/BPM/"+currentDate+"/"+timeClient.getFormattedTime(),bpm_val);   // Push BPM values to RTDB Firebase
  Firebase.setFloat("/users/"+patient_id+"/readings/Body Temp/"+currentDate+"/"+timeClient.getFormattedTime(),btemp_val);
  Firebase.setFloat("/users/"+patient_id+"/readings/Humidity/"+currentDate+"/"+timeClient.getFormattedTime(),hum_val);
  Firebase.setFloat("/users/"+patient_id+"/readings/Room Temp/"+currentDate+"/"+timeClient.getFormattedTime(),temp_val);

  if(btemp_val < 30.0)
    Firebase.setFloat("/users/"+patient_id+"/readings/Body Temp/Alert",1);  // set alert value
  else if(btemp_val > 40)
    Firebase.setFloat("/users/"+patient_id+"/readings/Body Temp/Alert",2);
  else
    Firebase.setFloat("/users/"+patient_id+"/readings/Body Temp/Alert",0);

  if(bpm_val < 30)
    Firebase.setFloat("/users/"+patient_id+"/readings/BPM/Alert",1);
  else if(bpm_val > 180)
    Firebase.setFloat("/users/"+patient_id+"/readings/BPM/Alert",2);
  else
    Firebase.setFloat("/users/"+patient_id+"/readings/BPM/Alert",0);

  
  
  delay(1000);
  
  if (Firebase.failed()) {  
      return;
  }
  }   
}
