void sendMessage(String message){
  // process data to send with HTTPS POST
  String url = "https://api.callmebot.com/whatsapp.php?phone="+
                phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);



  // Create a WiFiSSLClient object.
  WiFiSSLClient whatsappApiClient;

  // attempt connect to WhatsApp API
  Serial.print("connecting to api.callmebot.com ..........");
  if (!whatsappApiClient.connect("api.callmebot.com", 443)) {////////////////may fail port used
    Serial.println("Connection failed! - WhatsApp");
    return;
  }
  
  // send message
  // Serial.print("requesting URL: ");
  // Serial.println(url);

  whatsappApiClient.print(String("POST ") + url + " HTTP/1.1\r\n" +
                                 "Host: api.callmebot.com\r\n" +
                                 "Content-Type: application/x-www-form-urlencoded\r\n" +
                                 "Connection: close\r\n" +
                                 "Content-Length: " + String(url.length()) + "\r\n" +
                                 "\r\n" + url);

  // wait for the server's response
  while (whatsappApiClient.connected()) {
    String line = whatsappApiClient.readStringUntil('\n');
    
    // determine if successful
    if (line.startsWith("HTTP/1.1 200 OK") || line.startsWith("HTTP/1.1 302")) {
      Serial.println("Success! - WhatsApp");
      break;
    }
    else Serial.println("Failed! - WhatsApp");
    
  }
  Serial.println("");

  // free resources
  whatsappApiClient.stop();
}