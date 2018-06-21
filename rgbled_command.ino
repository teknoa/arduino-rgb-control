#define MAXBUF 20

#define RPIN 3
#define GPIN 5
#define BPIN 6

char* commandbuffer;
byte cmdbufIdx;
char valSerial;

enum COMMANDSTATE {
  NONE,
  BEGINREAD,
  ENDREAD,
  EXECUTE
};
COMMANDSTATE state;

enum RUNSTATE {
  R_NONE,
  R_RUNNING,
  R_FINISHED
};
RUNSTATE runstate;


void readCommand() {
   valSerial = Serial.read();
  if(valSerial != -1) {
    *(commandbuffer + cmdbufIdx) = valSerial;
    cmdbufIdx++;
    if(state == NONE) {
      if(*(commandbuffer) != 'C') {
        cmdbufIdx = 0;
        Serial.println("expected 'C' as start of command found.. resetting");
      }
      // check, if we have a valid command start
      if(cmdbufIdx == 2) {
        if(*(commandbuffer + 1) == 'M') {
           state = BEGINREAD;
           Serial.println("start of command found");
        } else {
          cmdbufIdx = 0;
          Serial.println("expected 'CM' as start of command found.. resetting");
        }
      }
    }
    // end of command found
    if(cmdbufIdx < MAXBUF && valSerial == '.'){
      // check, if we have a valid command start
      if(state == BEGINREAD) {
        state = ENDREAD;
        Serial.println("end of command found");
      } else {
        cmdbufIdx = 0;
        state = NONE;
        Serial.println("endcommand byte found without start command.. resetting");
      }
    }
    // if sender is still sending command bytes.. reset 
    if(cmdbufIdx >= MAXBUF) {
      state = NONE;
      cmdbufIdx = 0;
      Serial.println("command too long.. resetting");
    }
  }
  
  if(state == ENDREAD) {
     cmdbufIdx = 0;
     state = EXECUTE; 
  }

  if(state == EXECUTE) {
    // stop previous running command in case it's an animation
    // run the command
    if(runstate == R_NONE) {
      Serial.println("running command");
      // third position in command buffer is function index starting from 1
      runstate = R_RUNNING;
      
     
    }
    
    
    state = NONE;
  }

  
}

void parseCommand() {
  Serial.println("parsing command");
  
}

void setup() {
  // put your setup code here, to run once:
  commandbuffer = (char*)malloc(MAXBUF * sizeof(char));
  cmdbufIdx = 0;
  state = NONE;
  runstate = R_NONE;
  Serial.begin(9600);

  pinMode(RPIN, OUTPUT);
  pinMode(GPIN, OUTPUT);
  pinMode(BPIN, OUTPUT);
}



void loop() {
  // put your main code here, to run repeatedly:
  readCommand();
  if(runstate == R_RUNNING){
     switch((char)commandbuffer[2]) {
        case '1':
          funcSetColor(4);   // param = start of parameters
          break;
      }
      
  }
  if(runstate == R_FINISHED)
    runstate = R_NONE;
  
}
