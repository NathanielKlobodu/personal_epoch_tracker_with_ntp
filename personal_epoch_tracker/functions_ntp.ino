// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);

  // Initialize values needed to form NTP request
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision

  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}



// convert the date & time rrecieved to human readable form
void getUnixTime() {
  Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

  // extract NTP timestamp
  secsSince1900 = (unsigned long)packetBuffer[40] << 24 |
                                (unsigned long)packetBuffer[41] << 16 |
                                (unsigned long)packetBuffer[42] << 8 |
                                (unsigned long)packetBuffer[43];

  // Convert NTP timestamp to Unix timestamp (seconds since Jan 1 1970)
  unixTime = secsSince1900 - 2208988800UL;
}



/**
 * Converts a Unix timestamp into a human-readable date and time in string format.
 * 
 * @param unixTime The Unix timestamp to convert.
 * @param date A pointer to a String object to store the output date in.
 * @param time A pointer to a String object to store the output time in.
 * @return None.
 */
void calculateDateTime(unsigned long unixTime, String* date, String* time) {
  // calculate second, minute and hour
  int second = unixTime % 60; unixTime /= 60;
  int minute = unixTime % 60; unixTime /= 60;
  int hour = unixTime % 24;   unixTime /= 24;

  // calculate year
  int year = 1970;
  while (unixTime >= 365 + ((year%4==0 && year%100!=0) || year%400==0)) {
      unixTime -= 365 + ((year%4==0 && year%100!=0) || year%400==0);
      year++;
  }

  // calculate month and day
  int daysInMonth, daysInYear = ((year%4==0 && year%100!=0) || year%400==0) ? 366 : 365;
  int month = 1;
  while (unixTime >= 
         (daysInMonth = (month==2)
         ?(daysInYear==366? 29 : 28)
         :((month==4 || month==6 || month==9 || month==11) ? 30 : 31))) {
      unixTime -= daysInMonth;
      month++;
  }
  int day = unixTime + 1;

  *date = stringDate(day, month, year);
  *time = stringTime(hour, minute, second);
}


/**
 * Converts a date into a formatted string.
 * 
 * @param day The day of the month.
 * @param month The month of the year.
 * @param year The year.
 * @return A string in the format "DD/MM/YYYY" representing the input date.
 */
String stringDate(int day, int month, int year) {
  String date = "";

  if (day < 10) date += "0";
  date += String(day) + "/";
  
  if (month < 10) date += "0";
  date += String(month) + "/";

  date += String(year);

  return date;
}


/**
 * Converts a time into a formatted string.
 * 
 * @param hour The hour of the day.
 * @param minute The minute of the hour.
 * @param second The second of the minute.
 * @return A string in the format "HH:MM:SS" representing the input time.
 */
String stringTime(int hour, int minute, int second) {
  String time = "";

  if (hour < 10) time += "0";
  time += String(hour) + ":";

  if (minute < 10) time += "0";
  time += String(minute) + ":";

  if (second < 10) time += "0";
  time += String(second);

  return time;
}

