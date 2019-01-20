#include <dht.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <string>

dht DHT;
// Set these to run example.
#define SENSORPIN 4
#define LIGHTPIN 4
#define DHT11_PIN 5
#define FIREBASE_HOST "soteria-589ca.firebaseio.com"
#define FIREBASE_AUTH "43U33RvW6G7uyaKXnnXdvFcFrpAjqEs4UffXGiBn"
#define WIFI_SSID "yeswifi"
#define WIFI_PASSWORD "matthew1"

struct
{
  uint32_t total;
  uint32_t ok;
  uint32_t crc_error;
  uint32_t time_out;
  uint32_t connect;
  uint32_t ack_l;
  uint32_t ack_h;
  uint32_t unknown;

} stat = { 0, 0, 0, 0, 0, 0, 0, 0};

//int trigPin = 15;    
//int echoPin = 13;   
//long duration, cm, inches;
//long oldin=999;
//boolean Mot = false;
//boolean initMot = true;
boolean lightCondition;
int Sensor = A0;
int Val = 0;
boolean initCondition = true;
int in1 = 4;
int in2 = 16;
int in3 = 4;
int relayPin = 1;
int sensorState = 0, lastState = 0;
float faren;
float cel;
boolean irstate;
boolean oldirstate = false;
float oldfaren = 0.0;
double InitialTemp = 73;
int setState; // 0 is off and 1 is on

void setup() {
  //pinMode(trigPin, OUTPUT);
 // pinMode(echoPin, INPUT);
  
  pinMode(in1, OUTPUT);
  digitalWrite(in1, HIGH);
  pinMode(in2, OUTPUT);
  digitalWrite(in2, HIGH);
  pinMode(LIGHTPIN, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(SENSORPIN, INPUT);
  digitalWrite(SENSORPIN, HIGH); //starts off (relay engaged)
  Serial.begin(115200);

  // connect to wifi.
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

double setTemp() {
  InitialTemp = Firebase.getFloat("devices/913/temperature_goal");
  return InitialTemp;
}

void turnOn() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}
void setLight(){
  String s;
  boolean b = Firebase.getBool("devices/913/lightstate");
  if (b){ 
    s="HIGH";
    digitalWrite(relayPin,HIGH);
  }
  else {
    s="LOW";
    digitalWrite(relayPin, LOW);
  }
  Serial.println(s);
  Serial.println(digitalRead(LIGHTPIN));
}

void turnOff() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}

void loop() {
//  digitalWrite(trigPin, LOW);
//  delayMicroseconds(5);
//  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(10);
//  digitalWrite(trigPin, LOW);
//
//
//  pinMode(echoPin, INPUT);
//  duration = pulseIn(echoPin, HIGH);
//
//
//  cm = (duration / 2) / 29.1;  
//  inches = (duration / 2) / 74; 
//
//  Val = analogRead(Sensor);
//  sensorState = digitalRead(SENSORPIN);
//
//  if (sensorState == LOW) {
//    // turn LED on:
//    irstate = false;
//    Serial.println("high");
//  }
//  else {
//    // turn LED off:
//    irstate = true;
//    Serial.println("low");
//  }

  Serial.print("DHT11, \t");

  uint32_t start = micros();
  int chk = DHT.read11(DHT11_PIN);
  uint32_t stop = micros();

  stat.total++;
  switch (chk)
  {
    case DHTLIB_OK:
      stat.ok++;
      Serial.print("OK,\t");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      stat.crc_error++;
      Serial.print("Checksum error,\t");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      stat.time_out++;
      Serial.print("Time out error,\t");
      break;
    default:
      stat.unknown++;
      Serial.print("Unknown error,\t");
      break;
  }
  // DISPLAY DATA
   Serial.print("humidity: ");
  Serial.print(DHT.humidity ,1);
  Serial.print(",\t");
  Serial.print("Temperature (c): ");
  Serial.print(DHT.temperature,1);
  Serial.print(",\t");
//  Serial.print(stop - start);
  
  cel = DHT.temperature;
  faren = (cel * 1.8) + 32;
  Serial.print("Temperature (f): ");
  Serial.print(faren,1);
  Serial.print(",\t");
  Serial.println();
  delay(300);

  double Lowerbound = setTemp() - 2;
  double Upperbound = setTemp() + 2;

  Serial.println (Firebase.getFloat("devices/913/temperature_goal"));

  if (faren < Upperbound) {
    Serial.println("increase temp");
    turnOn();
  }
  if (faren > Lowerbound) {
    Serial.println("turn off heat");
    turnOff();
  }



  if (abs(faren - oldfaren) > 1&&faren<150&&faren>-50) {
    
    Firebase.setFloat("devices/913/temperature", faren);
    oldfaren = faren;
    delay(1000);
    Firebase.pushFloat("temp_logs", faren);

  }

  if ( irstate != oldirstate) {
    Firebase.setBool("irstate", irstate);
    oldirstate = irstate;
    Firebase.pushBool("irstate_logs", irstate);
  }
//
//  if (Val < 150) {
//    lightCondition = true; //true light is off
//  }
//  else {
//    lightCondition = false; //false is light is on
//  }

//  if (lightCondition != Firebase.getBool("devices/913/lightstate")) {
//    
//    
//    
//    }
  
  boolean b= Firebase.getBool("devices/913/lightstate");
  if (b){ 
    Serial.print("High");
    digitalWrite(relayPin , HIGH ); // off Light
  }
  else {
  Serial.print("LOW");
    digitalWrite(relayPin , LOW ); // On Light
  }
    
  
  // setLight();

//  if (inches < 9 && inches != 0){
//    Mot = true;
//  }
//  else{
//    Mot = false;
//  }
//  
//  if (Mot != initMot){
//    Firebase.setBool("motion", Mot);
//    initMot = Mot;
//  }


  digitalWrite(relayPin,LOW);
  delay(500);
    digitalWrite(relayPin, HIGH);
  delay(500);


  delay(500);
}
