// attempt to connect to WiFi network:
void wifiConnect() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to SSID: ");
    Serial.print(ssid);

    while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      Serial.print(".");
      delay(100);
    }
    Serial.println("");
    Serial.println("Wi-Fi connected!\n");
    digitalWrite(LED_BUILTIN, LOW);
  }
}


bool getNTPTime() {
  // update time from ntp server; send an NTP packet to a time server
  Serial.println("Requesting from ntp server...");
  sendNTPpacket(timeServer);

  // wait to see if a reply is available
  delay(700);

  // check if a reply is available
  if (Udp.parsePacket()) {
    // weve received a packet, read the data from it
    Serial.println("Reply received from ntp server");
    
    // calulate Unix time from the NTP packet
    getUnixTime();

    // print Unix time (seconds since Jan 1 1970):
    Serial.print("Unix time = ");
    Serial.println(unixTime);


    // convert Unix time into everyday time
    // also makes a strings containing the date and the time
    // of the previous second and the current second
    calculateDateTime(unixTime, &strDate, &strTime);
    calculateDateTime((unixTime - 1UL), &prevStrDate, &prevStrTime);

    //print preivous date and time
    Serial.print("Previous date and time: ");
    Serial.print(prevStrDate);
    Serial.print("   ");
    Serial.println(prevStrTime);

    // print current date and time
    Serial.print("Current date and time: ");
    Serial.print(strDate);
    Serial.print("   ");
    Serial.println(strTime);
    
    return true;
  }
  
  Serial.println("No reply from ntp server");
  return false;
}