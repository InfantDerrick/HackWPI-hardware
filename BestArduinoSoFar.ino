#include <dht.h>

#include <Servo.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <string>
dht DHT;
#define FIREBASE_HOST "soteria-589ca.firebaseio.com"
#define FIREBASE_AUTH "43U33RvW6G7uyaKXnnXdvFcFrpAjqEs4UffXGiBn"
#define WIFI_SSID "yeswifi"
#define WIFI_PASSWORD "matthew1"
//#define DTH11_PIN 5

Servo myservo;
int pos = 0;
/*
 * PIN LAYOUT:
 * 1: Relay
 * 4:Door Sensor
 * 5: Temp/Humidity 
 * 
 * 14: Servo
 * 
 */
 int in1 = 1;
float oldfaren = 0.0;
double InitialTemp = 73;
float faren;
float cel;
int DHT11_PIN = 5;
int relayPin = 1;
int doorSensorState = 0;
int doorSensorPin = 4;
int soundSensorPin = 3;
boolean soundVal = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(soundSensorPin, INPUT);
  pinMode(relayPin , OUTPUT);
  pinMode(doorSensorPin, INPUT);
   myservo.attach(14);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}


void loop() {
  // put your main code here, to run repeatedly:
  //  int chk = DHT.read11(DHT11_PIN);


  // READ DATA
  Serial.print("DHT11, \t");
  int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:
      Serial.print("OK,\t");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.print("Checksum error,\t");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.print("Time out error,\t");
      break;
    default:
      Serial.print("Unknown error,\t");
      break;
  }
  // DISPLAY DATA
  Serial.print(DHT.humidity, 1);
  Serial.print(",\t");
  Serial.println(DHT.temperature, 1);
  if (DHT.temperature > -40) {
    Firebase.setFloat("devices/913/temperature", 1.8 * DHT.temperature + 32);
  }

if (DHT.humidity > -1) {
    Firebase.setFloat("devices/913/humidity", 1.8 * DHT.humidity);
  }

  
  if (Firebase.getBool("devices/913/lightstate")) {
    Serial.print("High");
    digitalWrite(relayPin , HIGH );
  }
  else {
    Serial.print("LOW");
    digitalWrite(relayPin ,LOW );
  }

  doorSensorState = digitalRead(doorSensorPin);
  if (doorSensorState == HIGH) {
    Firebase.setBool("devices/913/doorstate", false);
  }
  else {
    Firebase.setBool("devices/913/doorstate", true);
  }


  if (Firebase.getBool("devices/913/lockstate")) {
    pos = 0;
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);

  }
  else {
    pos = 180;
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position

  }



  delay(500);
}
