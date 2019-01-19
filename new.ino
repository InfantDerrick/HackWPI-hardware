#include <dht.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <string>

dht DHT;
// Set these to run example.
#define SENSORPIN 4 //Metal touch (door sensor) is on pin 4
#define DHT11_PIN 5 //Temperature/humidity sensor is on pin 5
#define LIGHTPIN 1 //Pin for light switch is on pin 1
#define FIREBASE_HOST "soteria-589ca.firebaseio.com"
#define FIREBASE_AUTH "43U33RvW6G7uyaKXnnXdvFcFrpAjqEs4UffXGiBn"
#define WIFI_SSID ""
#define WIFI_PASSWORD "!"

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
pinMode(LIGHTPIN, OUTPUT); 
double setTemp() {
  InitialTemp = Firebase.getFloat("thermostat");
  return InitialTemp;
}

void turnOn() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void turnOff() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}

void loop() {


  Serial.print("DHT11, \t");

  uint32_t start = micros();
  int chk = DHT.read22(DHT11_PIN);
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

  adoorstate = analogRead(analogIn);
  doorstate = digitalRead(SensorPin);
  if (doorstate == HIGH)
  {
    
    Serial.println("Closed");
  }
  else
  {
    
    Serial.println("Open");
  }


  // DISPLAY DATA
  Serial.print(DHT.humidity , 1);
  Serial.print(",\t");
  Serial.print(DHT.temperature, 1);
  Serial.print(",\t");
  Serial.print(stop - start);
  Serial.println();
  cel = DHT.temperature;
  faren = (cel * 1.8) + 32;
  delay(300);

  double Lowerbound = setTemp() - 2;
  double Upperbound = setTemp() + 2;

  Serial.println (Firebase.getFloat("thermostat"));

  if (faren > Upperbound) {
    Serial.println("increase temp");
    turnOn();
  }
  if (faren < Lowerbound) {
    Serial.println("turn off heat");
    turnOff();
  }



  if (abs(faren - oldfaren) >= 1 && abs(faren - oldfaren) < 5 && faren < 150 && faren > -50) {

    Firebase.setFloat("temperature", faren);
    oldfaren = faren;
    delay(1000);
    Firebase.pushFloat("temp_logs", faren);

  }


  if (doorstate != olddorstate) {
    Firebase.setBool("doorstate", doorstate);
    olddoorstate = doorstate;
    Firebase.pushBool("doorstate_logs", doorstate);

  }
  int onoroff = Firebase.getInt("LIGHTPIN")
  if ( LIGHTPIN != onoroff) {
  digitalWrite(LIGHTPIN , onoroff);
}





delay(100);
}
