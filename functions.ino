void funcSetColor(byte startparamidx) {
  Serial.println("Executing setcolor");
  // our parameters start after fnID
  byte paramidx = startparamidx;
  
  byte rgb[] = {0,0,0};
  byte colidx = 2;
  byte factor = 1;
  char curc;

  // fast forward to end of command
  while(commandbuffer[paramidx] != '.' && paramidx++ < MAXBUF);
  paramidx--;
  Serial.print("found end of command ");
  Serial.print(paramidx);
  while(colidx >= 0 && paramidx >= startparamidx - 1) {
    curc = commandbuffer[paramidx--];
    Serial.print(curc);
    if(curc == ','){
      Serial.println(rgb[colidx]);
      colidx--;
      factor = 1;
      
    } else {
      byte c = (byte)curc - 48; // get actual byte value from ascii cipher
      rgb[colidx] += factor * c;
      factor *= 10;
      if(factor > 1000) {
        Serial.print("Number exceeding 255..invalid");
        break;
      }
    }
  }
  
  if(colidx == 255) { // after decrement from 0, byte is 255
    
    Serial.println("writing pins");
    Serial.print(rgb[0]);
    Serial.print(",");
    Serial.print(rgb[1]);
    Serial.print(",");
    Serial.print(rgb[2]);
    
    analogWrite(RPIN, rgb[0]);
    analogWrite(GPIN, rgb[1]);
    analogWrite(BPIN, rgb[2]);
  } else {
    Serial.print("Color missing.");
  }
  runstate = R_FINISHED;
  Serial.println("finished");
}

void funcFade(byte startparamidx) {
  
}

