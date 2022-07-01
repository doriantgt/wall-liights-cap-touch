void ambientLightingMAIN(DscreenAlarmClock &thisScreen,DayMillisSeconds timestuff,RGBWstrip stripIn,ButtonInterpreter buttonsArrayIn[]){  //this is the screens main funcion. And you pass itself into it.
    thisScreen.channels[ thisScreen.currentChannel ]( timestuff, stripIn, buttonsArrayIn ); //run the current channel/mode that you're on.
    //cout << " in ambient light main " <<  thisScreen.currentChannel << endl;
   
   
    if(clickUp.is(buttonsArrayIn[1])){
      thisScreen.currentChannel ++ ;
      thisScreen.currentChannel %= thisScreen.numChannels;
    }
    if(LpressUp.is(buttonsArrayIn[1])){
      thisScreen.currentChannel += (thisScreen.numChannels-1);
      thisScreen.currentChannel %= thisScreen.numChannels;
    
    }
       if(HpressDown.is(buttonsArrayIn[1])){
      cout << "brightness ++ " << globalBrightness <<endl;
       globalBrightness+=64;
     //  FastLED.setBrightness(globalBrightness);
    }
  
   // thisScreen.currentChannel++; //move through the channels
   // thisScreen.currentChannel%=thisScreen.numChannels;
}

//twinkle
void mode1(DayMillisSeconds timestuff,RGBWstrip stripIn,ButtonInterpreter buttonsArrayIn[] ){
//     cout << "mode 1 "  << endl;

//twinkle button 0 picks color
  static byte hue;
if(clickUp.is(buttonsArrayIn[0])){
  cout << "hue + "  << endl;
  hue+=4;
}
if(LpressUp.is(buttonsArrayIn[0])){
  cout << "hue -" << endl;
  hue-=16;
}
if(HpressDown.is(buttonsArrayIn[0])){
  cout << "hue +++" << endl;
  hue+=64;
}
        
  
     
    CRGBW crgwfromhsv ;
    crgwfromhsv = CHSV(hue, 255,255);
    
    static Twinkler twikle;
    
    twikle.twinkleMin =100; //these 2 randomize the twinke freq
    twikle.twinkleMax =200; //in milliseconds
    twikle.fadeFrequency = 400;// how often it fades
    twikle.fadeAmount = .95;// perentage
   
      
    twikle.twinkle( timestuff ,stripIn, crgwfromhsv);

    
      uint16_t static lastTimehueInc;     
     if(timestuff.timePassed16(lastTimehueInc, 1000 ) ) {
    //  hue++;
     }

  
}
void mode2(DayMillisSeconds timestuff,RGBWstrip stripIn,ButtonInterpreter buttonsArrayIn[] ){
//reverse twinklie with white
 //  cout << "mode 2 "  << endl;

   byte hue = random8();
    
  
     
    CRGBW crgwfromhsv ;
    crgwfromhsv = CHSV(hue, 255,32);
    crgwfromhsv.w=1;
    static Twinkler twikle;
    
    twikle.twinkleMin =1; //these 2 randomize the twinke freq
    twikle.twinkleMax =10;
    twikle.fadeFrequency = 200;// how often it fades
    twikle.fadeAmount = 1.01;// in ms;

      
    twikle.twinkle( timestuff ,stripIn, crgwfromhsv);

    
      uint16_t static lastTimehueInc;     
     if(timestuff.timePassed16(lastTimehueInc, 1000 ) ) {
     
      
 
     }


  
}
void mode3(DayMillisSeconds timestuff,RGBWstrip stripIn,ButtonInterpreter buttonsArrayIn[] ){
  // cout << "mode 3 "  << endl;
  static byte hue=0-16;

     CRGBW crgwfromhsv ;
    crgwfromhsv = CHSV(hue, 255,255);
  stripIn.fillwith(crgwfromhsv);
  // for (uint16_t i =0; i<NUM_LEDS; i++) {   stripIn.array[i] =  crgwfromhsv;}
if(clickUp.is(buttonsArrayIn[0])){
//  cout << "hue + "  << endl;
  hue+=4;
    Serial.println(hue,BIN);
}
if(LpressUp.is(buttonsArrayIn[0])){
//  cout << "hue -" << endl;
  hue-=16;
}
if(HpressDown.is(buttonsArrayIn[0])){
 // cout << "hue +++" << endl;
  hue+=64;
}
 
}

void mode4(DayMillisSeconds timestuff,RGBWstrip stripIn,ButtonInterpreter buttonsArrayIn[] ){
static byte warmthPal=8;//8 is 9th //nth is white
   
    
    CRGBW crgwfromhsv ;
  

    if(warmthPal<8){
       crgwfromhsv = CHSV(warmthPal*32, 128,255);
    }else{
        crgwfromhsv = CHSV(warmthPal, 0,255);
    }
    crgwfromhsv.w=255;
      stripIn.fillwith(crgwfromhsv);


if(clickUp.is(buttonsArrayIn[0])){
//  cout << "hue + "  << endl;
  warmthPal+=1;
  warmthPal%=9;
}
if(LpressUp.is(buttonsArrayIn[0])){
  warmthPal+=8; //plus size of pallet minus 1
  warmthPal%=9;
}
if(HpressDown.is(buttonsArrayIn[0])){
 // cout << "hue +++" << endl;
//  hue+=64;
}




}

void mode5(DayMillisSeconds timestuff,RGBWstrip stripIn,ButtonInterpreter buttonsArrayIn[] ){

static byte rolling; //this make the rainbow move
static uint16_t widthWave=600;

static uint16_t lastTimeWaved=0;
static uint16_t lastTimeRolled=0;
static uint16_t rollSpeed=100;

   if( timestuff.timePassed16(lastTimeRolled, rollSpeed ) ){ //it takes 3ms for Show() 
              if(rollSpeed == 1)
              rolling+=3;
              
              rolling++;

   }

   if( timestuff.timePassed16(lastTimeWaved, 1 ) ){
          static int8_t flipper =1;
          if(widthWave >= 700 || widthWave <= 1 ){
            flipper *= -1;
            cout << "flipper " << endl;
          }
           widthWave+=flipper;
    
        }

        
       CRGBW crgwfromhsv ;    
    for(uint16_t i=0; i<stripIn.numLeds; i++){

      byte hue =   scale8bitAccurate( i , stripIn.numLeds + widthWave);
      hue+=rolling;
      crgwfromhsv = CHSV(hue, 255,255);

     stripIn.leds[i] = crgwfromhsv;
   
      
    }
if(clickUp.is(buttonsArrayIn[0])){
//  cout << "hue + "  << endl;
  if(rollSpeed>1){  
  rollSpeed/=10;}
  cout << "rollspeed faster " << rollSpeed << endl;

}
if(LpressUp.is(buttonsArrayIn[0])){
  if(rollSpeed<9999){  
  rollSpeed*=10;}
    cout << "rollspeed slower " << rollSpeed << endl;

}
if(HpressDown.is(buttonsArrayIn[0])){
 // cout << "hue +++" << endl;
//  hue+=64;
}     
}//mode5





void ChasingRainbowsB(DayMillisSeconds timestuff,RGBWstrip stripIn,ButtonInterpreter buttonsArrayIn[] ){

#define numOfStar 15  
static Dstar myStars[numOfStar];

static RainbowChasers starsObj(myStars,numOfStar,1,2);

static bool firstTime=true;
static bool nextMode=false;
static uint8_t gBrightness = 255;
static int8_t flipper= 8;

static uint8_t directionSelector=2;



     
      
      if(firstTime==true){
      
         flipper=1;
         
        firstTime=false;
        nextMode=false;
        CRGBW  black= CRGBW(0,0,0,0); //reset leds
            stripUtil.fillwith(black);
      }
      
       //stars move and update at full processor
      byte newTime=millis();
      starsObj.starSpeed(stripUtil,newTime);
       
      EVERY_N_MILLISECONDS(100){  stripUtil.fader(.80);   }
      
      
      //display loop and color incriment
      //and bring lights back on
      EVERY_N_MILLISECONDS( 30 ){ 
      
      
       if( starsObj.stars[0].hsv.v<200){ //ramp up bightness check only the 1 star though...
         for(byte i=0; i<starsObj.numOfStars; i++){
        starsObj.stars[i].hsv.v++;}}
      
        
        for(byte i=0; i<starsObj.numOfStars; i++){  
          starsObj.stars[i].hsv.h+=9;
        }
      
      }
      
      //speed incriment and loop cinemeatics
      EVERY_N_MILLISECONDS( 500 ){ 
        
      starsObj.stars[0].var.counter7++;
      
      if((starsObj.stars[0].var.counter7%5)==0){ //every five ticks speed faster
        for(byte i=0; i<numOfStar; i++){

           if( starsObj.stars[i].var.speed16 < 0){
                 starsObj.stars[i].var.speed16-=flipper;

           }else{ starsObj.stars[i].var.speed16+=flipper; }
           
        }
      }       
      
       if(starsObj.stars[0].var.counter7 == 120 )  { //were just using th 1 stars variable to track all of the stars speed change
        flipper*=-1;
        starsObj.stars[0].var.counter7=0;
       }
      
      }//every milli


if(clickUp.is(buttonsArrayIn[0])){
directionSelector++; directionSelector%=3;

 starsObj = RainbowChasers(myStars,numOfStar,directionSelector-1,2);
  

}


      
      }//chasing rainbowsB
