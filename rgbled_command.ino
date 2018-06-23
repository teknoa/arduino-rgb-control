/**
 * RGB LED Command
 * A small program to control RGB led using PWM pins on the arduino
 * or a manual PWM using digital pins.
 * The command is received over serial (either USB or bluetooth)
 * Command form is
 * CM<fnID>,pval1,pval2,..,pvaln.
 * 
 * A command has to start with the chars 'CM'
 * Followed by a function id, which is the function index
 * Followed by a list of parameters for the function.
 * 
 * Two functions have been implemented for diplaying RGB values using Leds.
 * See functions file.
 */

#define MAXBUF 20

#define RPIN 10
#define GPIN 9
#define BPIN 8

char* commandbuffer;
byte cmdbufIdx;
char valSerial;
char fnidx;

/*
 * The command state shows the state, of reading 
 * a command over serial
 */
enum COMMANDSTATE {
  NONE,
  BEGINREAD,
  ENDREAD,
  EXECUTE
};
COMMANDSTATE state;

/*
 * Represents the running state of a function
 */
enum RUNSTATE {
  R_NONE,
  R_RUNNING,
  R_FINISHED
};
RUNSTATE runstate;

/*
 * DEBUG block with custom functions
 * to turn on serial output when debugging but not in production
 */
#define DEBUG false
#define SERIALPORT  Serial1

void debugprintln() {
  if(DEBUG) SERIALPORT.println();
}
void debugprint(byte text) {
  if(DEBUG) SERIALPORT.print(text);
}
void debugprintln(byte text) {
  if(DEBUG) SERIALPORT.println(text);
}
void debugprint(const char text[]) {
  if(DEBUG) SERIALPORT.print(text);
}
void debugprintln(const char text[]) {
  if(DEBUG) SERIALPORT.println(text);
}

void readCommand() {
  valSerial = SERIALPORT.read();
  if(valSerial != -1) {
    *(commandbuffer + cmdbufIdx) = valSerial;
    cmdbufIdx++;
    if(state == NONE) {
      if(*(commandbuffer) != 'C') {
        cmdbufIdx = 0;
        debugprintln("expected 'C' as start of command found.. resetting");
      }
      // check, if we have a valid command start
      if(cmdbufIdx == 2) {
        if(*(commandbuffer + 1) == 'M') {
           state = BEGINREAD;
           debugprintln("start of command found");
        } else {
          cmdbufIdx = 0;
          debugprintln("expected 'CM' as start of command found.. resetting");
        }
      }
    }
    // end of command found
    if(cmdbufIdx < MAXBUF && valSerial == '.'){
      // check, if we have a valid command start
      if(state == BEGINREAD) {
        state = ENDREAD;
        debugprintln("end of command found");
      } else {
        cmdbufIdx = 0;
        state = NONE;
        debugprintln("endcommand byte found without start command.. resetting");
      }
    }
    // if sender is still sending command bytes.. reset 
    if(cmdbufIdx >= MAXBUF) {
      state = NONE;
      cmdbufIdx = 0;
      debugprintln("command too long.. resetting");
    }
  }
  
  if(state == ENDREAD) {
     cmdbufIdx = 0;
     fnidx = (char)commandbuffer[2];
     state = EXECUTE; 
  }

  if(state == EXECUTE) {
    // stop previous running command in case it's an animation
    // run the command
    runstate = R_NONE;
    debugprintln("running command init");
    // third position in command buffer is function index starting from 1
    
    // call the init part of the function
    switch(fnidx) {
      case '1':
        funcSetColorInit(4);   // param = start of parameters in command buffer
        break;
      case '2':
        funcPWMInit(4);   // param = start of parameters in command buffer
        break;
    }
    
    
    
    state = NONE;
  }

  
}

/*
 * Reads parameters from command buffer and stores them into given paramarray
 * It tries to read numparams and not more than numparams.
 * It will fail, if it cannot read numparams.
 */
char parseParameters(byte startparamidx,  byte paramarray[], byte numparams) {
  debugprintln("parseParameters");
  // our parameters start after fnID
  byte paramidx = startparamidx;

  //byte rgb[] = {0, 0, 0};
  byte colidx = numparams - 1;
  byte factor = 1;
  char curc;

  // reset paramarray
  for(byte i = 0; i< numparams; i++)
     paramarray[i] = 0;

  // fast forward to end of command
  while (commandbuffer[paramidx] != '.' && paramidx++ < MAXBUF);
  paramidx--;
  debugprint("found end of command at idx");
  debugprintln(paramidx);
  while (colidx >= 0 && paramidx >= startparamidx - 1) {
    curc = commandbuffer[paramidx--];
    debugprint(curc);
    if (curc == ',') {
      debugprintln();
      debugprint(paramarray[colidx]);
      colidx--;
      factor = 1;

    } else {
      byte c = (byte)curc - 48; // get actual byte value from ascii cipher
      paramarray[colidx] += factor * c;
      factor *= 10;
      if (factor > 1000) {
        debugprint("Number exceeding 255..invalid");
        break;
      }
    }
  }
  if (colidx == 255) { // after decrement from 0, byte is 255
    for(byte i = 0; i< numparams; i++)
      debugprint(paramarray[i]);
      debugprintln("parseParameters complete");
    return 0; //OK
  } else {
     debugprintln("parseParameters incomplete");
    return -1;
  }
}

/*
 * Arduino setup routine
 */
void setup() {
  // put your setup code here, to run once:
  commandbuffer = (char*)malloc(MAXBUF * sizeof(char));
  cmdbufIdx = 0;
  state = NONE;
  runstate = R_NONE;
  SERIALPORT.begin(9600);

  pinMode(RPIN, OUTPUT);
  pinMode(GPIN, OUTPUT);
  pinMode(BPIN, OUTPUT);
}



void loop() {
  
  readCommand();
  
  if(runstate == R_RUNNING){
     switch(fnidx) {
        case '1':
          funcSetColor();   // param = start of parameters
          break;
        case '2':
          funcPWM();   // param = start of parameters
          break;
      }
      
  }
  if(runstate == R_FINISHED)
    runstate = R_NONE;
  
}
