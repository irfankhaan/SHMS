#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define FIREBASE_AUTH "_YOUR_API_KEYS"   // Enter Firebase API keys/secret here
#define FIREBASE_HOST "YOUR_PROJECT-rtdb.firebaseio.com"   // Enter host URL
#define WIFI_SSID "YOUR_WiFi_SSID"   // Enter WiFi SSID
#define WIFI_PASSWORD "YOUR_WiFI_password"    // Enter WiFi passwords

const long utcOffsetInSeconds = 19800;    // UTC offset for GMT +5:30
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);

String patient_key = "845425413699";    // Random patient key
String sensor_data, bpm, bpm_val, btemp, btemp_val, hum, temp, hum_val, temp_val;

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
        sensor_data=Serial.readString();   // Get sensor data from serial output in sensor_data
        Sr=true;          
    }
  
  timeClient.update();

  delay(1000);
  
  if(Sr==true){  

  bpm = sensor_data.substring(0, sensor_data.indexOf(","));
  bpm_val = bpm.substring(bpm.indexOf(" ")+1);
  sensor_data.remove(0, sensor_data.indexOf(",")+1);

  btemp = sensor_data.substring(0, sensor_data.indexOf(","));
  btemp_val = btemp.substring(btemp.indexOf(" ")+1);
  sensor_data.remove(0, sensor_data.indexOf(",")+1);

  hum = sensor_data.substring(0, sensor_data.indexOf(","));
  hum_val = hum.substring(hum.indexOf(" ")+1);
  
  temp = sensor_data.substring(sensor_data.indexOf(","), sensor_data.indexOf("\n"));
  temp_val = temp.substring(temp.indexOf(" ")+1);

  Firebase.setFloat("/"+patient_key+"/BPM/"+timeClient.getEpochTime()+"-"+timeClient.getFormattedTime(),bpm_val.toFloat());   // Push BPM values to RTDB Firebase
  Firebase.setFloat("/"+patient_key+"/Body Temperature/"+timeClient.getEpochTime()+"-"+timeClient.getFormattedTime(),btemp_val.toFloat());
  Firebase.setFloat("/"+patient_key+"/Humidity/"+timeClient.getEpochTime()+"-"+timeClient.getFormattedTime(),hum_val.toFloat());
  Firebase.setFloat("/"+patient_key+"/Room Temperature/"+timeClient.getEpochTime()+"-"+timeClient.getFormattedTime(),temp_val.toFloat());
  
  delay(1000);
  
  if (Firebase.failed()) {  
      return;
  }
  }   
}
