#include <WiFiNINA.h>
#include <WiFiSSLClient.h>
#include <WiFiUdp.h>
#include <UrlEncode.h>
#include "config.h"


// WiFi config
char ssid[] = USER_SSID;    // your network SSID (name)
char pass[] = USER_PASS;    // your network password

// ntp
bool timeSuccess = 0; // flag to check if time was successfully received
unsigned int localPort = 2390;  // local port to listen for UDP packets
const int NTP_PACKET_SIZE = 48; // NTP timestamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets
IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
WiFiUDP Udp;  // A UDP instance to send and receive packets over UDP

// whatsapp API
String phoneNumber = USER_PHONE;
String apiKey = USER_API;

// google script web app
String GOOGLE_SCRIPT_ID = USER_GOOGLE_SCRIPT_ID;

// time variables
unsigned long secsSince1900;
unsigned long unixTime;
String strDate = "";
String strTime = "";
String prevStrDate = "";
String prevStrTime = "";
bool dayChanged = 0;

//room state
bool doorOpened = 0;
bool personPresent = 1;
bool prevPersonPresent;

int reedPin = 2;
int pirPin = 3;

unsigned long prevMillis;
bool once = 1;
bool doorState;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(reedPin, INPUT);
  pinMode(pirPin, INPUT);


  Serial.println("START OF SETUP  ---------------------------------------");

  // attempt to connect to WiFi network:
  wifiConnect();

  // initialize UDP object
  Udp.begin(localPort);
  
  delay(1000);

  // get current date and time
  timeSuccess = getNTPTime();
  Serial.println("timeSuccess: " + String(timeSuccess));

  if (timeSuccess) {
    // send "active" message to whatsapp stating that you are in the room
    Serial.println("Sending message to WhatsApp");
    sendMessage("Device Active!\nYou are currently in your room at " + strTime);
  
    // log to google sheets
    Serial.println("logging start state....");
    logData(strDate, strTime, personPresent);

    // reset variables
    prevPersonPresent = personPresent;
    timeSuccess = 0;
  }

  delay(5000);

  // debugging
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

  Serial.println("END OF SETUP  ---------------------------------------");
}

void loop() {
  // ensure WiFi connected
  wifiConnect();


  // serial debug print outs
  // Serial.print("doorState:")            Serial.print(doorState);
  // Serial.print("  doorOpened:")         Serial.print(doorOpened);
  // Serial.print("  timeSuccess:")        Serial.print(timeSuccess);
  // Serial.print("  once:")               Serial.print(once);
  // Serial.print("  prevPersonPresent:")  Serial.print(prevPersonPresent));
  // Serial.print("  personPresent:")      Serial.println(personPresent));

  digitalWrite(4, doorState);
  digitalWrite(5, doorOpened);
  digitalWrite(6, timeSuccess);
  digitalWrite(7, once);
  digitalWrite(8, prevPersonPresent);
  digitalWrite(9, personPresent);



  // check if door is opened with reed module
  doorState = digitalRead(reedPin);
  if (doorState) doorOpened = 1;

  // if the door was opened and is now closed
  if (doorOpened && !doorState) {
    //get time from ntp server
    if (!timeSuccess) timeSuccess = getNTPTime();

    
    // check if person entered room with the pir motion sensor
    // continuously poll the module for any changes in state for some time with millis()
    if (once) {
      personPresent = 0;
      prevMillis = millis();
      once = 0;
    }
    delay(250);

    if (millis() - prevMillis <= 2000) {
      if (digitalRead(pirPin)) personPresent = 1;
    }
    else {
      // reset variables
      once = 1;
      doorOpened = 0;
    }
  }
  if (timeSuccess && once) {
    // timestamp was successfully received and converted to date and time
    // check if room state has changed
    if (personPresent != prevPersonPresent) {
        Serial.println("room state changed");

      // send message to whatsapp
      if (personPresent) sendMessage("You entered your room at " + strTime);
      else sendMessage("You left your room at " + strTime);

      // log to google sheets
      //log end time of previous state
        Serial.println("logging end time of previous state");
      logData(prevStrDate, prevStrTime, prevPersonPresent);
      // log start time of new state
        Serial.println("logging start time of new state");
      logData(strDate, strTime, personPresent);


      // reset variables
      prevPersonPresent = personPresent;
    }
    Serial.println("");
      
    // reset variables
    timeSuccess = 0;
  }


  // check if day has changed
  // then log the end of the previous day with current personPresent state
  // and log the start of the new day with current personPresent state


}


