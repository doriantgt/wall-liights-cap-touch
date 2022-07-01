


                  //argument isnt used at the moment
uint32_t printLocalTime(uint32_t millisTime){ //returns millis time;

 
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return millisTime;
  }
    time_t someTime = mktime(&timeinfo);
    String times = ctime(&someTime);
   // Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S"); 
    Serial.println(times);
 // Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  uint32_t daySeconds = timeinfo.tm_sec + timeinfo.tm_min * 60 + timeinfo.tm_hour * 60 * 60;
  
  Serial.println((float)daySeconds);
  //%=86400


   return  millisTime = daySeconds * 1000;
 

}

uint32_t StringTimeDaySeconds(String stringTime){ //converts string time to uint seconds
 uint32_t seconds;
 String temp ="12";
  temp.setCharAt(0,stringTime.charAt(0));
  temp.setCharAt(1,stringTime.charAt(1));
  seconds = temp.toInt()*60*60; 
  temp.setCharAt(0,stringTime.charAt(3));
  temp.setCharAt(1,stringTime.charAt(4));
  seconds += temp.toInt()*60; 
  temp.setCharAt(0,stringTime.charAt(6));
  temp.setCharAt(1,stringTime.charAt(7));
  seconds += temp.toInt();

  return seconds;
}
