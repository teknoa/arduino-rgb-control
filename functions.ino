/**
 * This file contains all functions avalable to be called over serial.
 * 
 * Each function has a init function which is called once before the actual function
 * is executed.
 * The function is called every time in the loop as long as the 'runstate' is 
 * set to R_RUNNING.
 * If a function is finished, it must set the 'runstate' to R_FINISHED
 * 
 * Functions must be non blocking and not using the 'delay' function.
 * To make your function run "slower" uee the wait variable and count it
 * until your waited long enough.
 */
byte rgb[3] = {0, 0, 0};
byte oldrgb[3] = {0, 0, 0};
byte stepping[3];
int wait;

#define DELAY 300

void funcSetColorInit(byte startparamidx){
  //debugprintln("funcSetColorInit");
  for(byte i = 0; i < 3; i++) {
    oldrgb[i] = rgb[i];
  }
  if(parseParameters(startparamidx, rgb, 3) == -1) {
    runstate = R_NONE;
  } else {
    runstate = R_RUNNING;
    debugprint("stepping ");
    for(byte i = 0; i < 3; i++) {
      if(rgb[i] > oldrgb[i])
        stepping[i] = 1;
      else if(rgb[i] < oldrgb[i])
        stepping[i] = -1;
      else
        stepping[i] = 0;
      debugprint(stepping[i]);
      debugprint(",");
    }
    wait = 0;
    debugprintln();
  }
}

/*
 * is called, if init function executed without error
 */
void funcSetColor(){
  //debugprintln("writing pins");
  //debugprint(rgb[0]);
  //debugprint(",");
  //debugprint(rgb[1]);
  //debugprint(",");
  //debugprint(rgb[2]);
  
  if(wait++ >= DELAY) {
    //debugprintln("wait done.. doing pixel");
    wait = 0;
    byte same = 0;
    
    for(byte i = 0; i < 3; i++) {
      if(oldrgb[i] != rgb[i])
        oldrgb[i] += stepping[i];
      else
        same++;
    } 
   // debugprint(oldrgb[0]);debugprint(",");
    //debugprint(oldrgb[1]);debugprint(",");
    //debugprintln(oldrgb[2]);
    analogWrite(RPIN, oldrgb[0]);
    analogWrite(GPIN, oldrgb[1]);
    analogWrite(BPIN, oldrgb[2]);
    if(same == 3) {
      runstate = R_FINISHED;
     // debugprintln("finished");
    }
  }
  
}

byte lenR;
byte lenG;
byte lenB;
bool switchR;
bool switchG;
bool switchB;
byte stateR;
byte stateG;
byte stateB;
byte idx;

void funcPWMInit(byte startparamidx) {
  funcSetColorInit(startparamidx);
  lenR = rgb[0];
  lenG = rgb[1];
  lenB = rgb[2];
  idx = 0;
  switchR = switchG = switchB = false;
  stateR = stateG = stateB = 1;
  //debugprintln("PWMinit writing pins");
  //debugprint(rgb[0]);
  //debugprint(",");
  //debugprint(rgb[1]);
  //debugprint(",");
  //debugprint(rgb[2]);

}
void funcPWM(){
  if(idx >= lenR && stateR == 1)
    switchR = true;
  if(idx < lenR && stateR == 0)
    switchR = true;
    
  if(switchR) {
    stateR = 1 - stateR;
    digitalWrite(RPIN, stateR);
    switchR = false;
  }

  if(idx >= lenG && stateG == 1)
    switchG = true;
  if(idx < lenG && stateG == 0)
    switchG = true;
    
  if(switchG) {
    stateG = 1 - stateG;
    digitalWrite(GPIN, stateG);
    switchG = false;
  }

  if(idx >= lenB && stateB == 1)
    switchB = true;
  if(idx < lenB && stateB == 0)
    switchB = true;
    
  if(switchB) {
    stateB = 1 - stateB;
    digitalWrite(BPIN, stateB);
    switchB = false;
  }

  
  idx++; // it's a byte, so overflow will be to zero
  
}

