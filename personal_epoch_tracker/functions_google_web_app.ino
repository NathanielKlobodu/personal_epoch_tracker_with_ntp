// Subroutine for logging data to Google Sheets
void logData(String date, String time, bool roomState) {
  // process data to send with HTTPS GET
  String url = "/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?"+
               "date='" + date + "'&" +
               "time='" + time + "'&" +
               "roomState=" + String(roomState);
  // String url = "/macros/s/AKfycbxrkwlfL7HRnYRa_NEkrC2d-qT0kNJ3uTpljFfVoTYQQS6wV2tqKZkNzwFhqzTar7fK/exec?date='10-10-2023'&time='05:12:12'&rand1=32&rand2=2";



  // Create WiFiSSLClient object.
  WiFiSSLClient webAppClient;
  
  // attempt connect to Google host
  Serial.print("connecting to script.google.com ..........");
  if (!webAppClient.connect("script.google.com", 443)) {///////////////////may fail port used
    Serial.println("Connection failed! - Web App");
    return;
  }

  // log data
  // Serial.print("requesting URL: ");
  // Serial.println(url);

  webAppClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
                            "Host: script.google.com\r\n" +
                            "User-Agent: BuildFailureDetectorArduinoNanoRP2040Connect\r\n" +
                            "Connection: close\r\n\r\n");
  
  // wait for the server's response
  while (webAppClient.connected()) {
    String line = webAppClient.readStringUntil('\n');
    
    // determine if successful
    if (line.startsWith("HTTP/1.1 200 OK") || line.startsWith("HTTP/1.1 302")) {
      Serial.println("Success! - Web App");
      break;
    }
    else Serial.println("Failed! - Web App");
    
  }
  Serial.println("");

  // free resources
  webAppClient.stop();
} 