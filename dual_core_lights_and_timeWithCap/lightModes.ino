void sunRise1(uint16_t startTime,uint16_t endTime){

int t;

  
}


 
void colorFill(CRGB c){
  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = c;
    FastLED.show();
    delay(50);
  }
  delay(500);
}
 
void fillWhite(){
  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGBW(0, 0, 0, 255);
    FastLED.show();
    delay(50);
  }
  delay(500);
}
 
void rainbow(){
  static uint8_t hue;
 
  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = CHSV((i * 256 / NUM_LEDS) + hue, 255, 255);
  }
    
  FastLED.show();
  hue++;
}
 
void rainbowLoop(){
  long millisIn = millis();
  long loopTime = 5000; // 5 seconds
 
  while(millis() < millisIn + loopTime){
    rainbow();
    delay(5);
  }
}
