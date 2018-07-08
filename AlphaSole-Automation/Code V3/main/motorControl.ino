

//this function is called in order to initiate the system
void sweeper(){
  float lumMax = 0.0;
  float temp;
  int revSun=0;
  Serial.println("Entering sweeper initializer");
  initialization();
  Serial.println("Sweeper initialized");
  Serial.println("researched sol");

  int d;
  if(revsTot<NB_ROT_TOT/2){//half the maximum revolutions
    d=1;
  }
  else{
    d=-1;
  }
  while((10<revsTot) and (revsTot<NB_ROT_TOT-10)){
    Serial.print(revSun);
    Serial.print(", lux : ");
    Serial.println(lumMax);
    
    Serial.println(revsTot);
    temp = sweep(lumMax,d);
    if(temp > lumMax){
      lumMax = temp;
      revSun = revsTot;
    }
  }
  runMotor(-100*direc*d, abs(revsTot - revSun));
  Serial.println("end initialisation");
  Serial.println(revsTot);
  in_sweep = 0;
}

//this function search the beginning of the screw and its direction
void initialization(){
    runMotor(100,10000);
    if(revsTot > NB_ROT_TOT/2){
      direc = 1;
      Serial.println("end");
    }
    else{
      direc = -1;
      Serial.println("begin");
    }
    
}

//function that check the luminosity at each step
float sweep(float lumMax,int d){
  float cd = analogRead(PIN_IN_PHOTODIOD_RIGHT);
  cd=cd/1024.0*5.0;
  if(cd < 0)
    flagBug = 1;
    
  runMotor(100*direc*d, 5);
  if(cd>lumMax) {
    lumMax = cd;
    return cd;
  } 
  return lumMax;
}

//Turn the screw at 'percent' of the max speed and during 'revs' revolutions
void runMotor(int percent, int revs) {
  if(percent == 0 or revs == 0){
    return;
  }
  //Going forward or backward at the given percentage
  digitalWrite(9, LOW);   //Disable brake on Channel A
  if(percent<0){
    digitalWrite(12, LOW); //Backward on Channel A
    analogWrite(3, 255*(-percent)/100);   //Spins the motor on Channel A at 'percent' of the max speed 
    revsTot-=revs*direc;
  }
  else{
    digitalWrite(12, HIGH); //Forward on Channel A
    analogWrite(3, 255*percent/100);   //Spins the motor on Channel A at 'percent' of the max speed
    revsTot+=revs*direc;
  }
  int mechanicalstop;
  mechanicalstop=nbRevs(revs); //Wait for the motor to complete 'revs' revolutions 

  analogWrite(3,0); //stop the motor
  digitalWrite(9, HIGH); //Enabe brake on channel A
  if(mechanicalstop==1){
    exitstop(-percent);
  }
}

//Wait 'revs' revolutions
int nbRevs(int revs) {
  Serial.print("rev ");
  Serial.println(revs);
  int t=revs;
  int flagPre, flag;
  flagPre = 1;
  flag=0;
  while(t!=0){
    
    if(digitalRead(PIN_MECH_STOP_1)==HIGH)
    {
      revsTot=NB_ROT_TOT;
      Serial.println(F("buté photo"));
      return 1;  
    }
    if(digitalRead(PIN_MECH_STOP_2)==LOW)
    {
      revsTot=0;
      Serial.println(F("buté mech"));
      return 1;  
    }
    flag = digitalRead(22);  //Read pin 22
    
    if(flagPre==0 and flag == 1)//if we have a rising front
    {
      t--;//we decreased the number of revolution
      Serial.println(t);
    }
    flagPre=flag;
  }
  return 0;
}

//function called when a mechanical stop is triggered. It controls the motor to do 20 turns in the opposite direction of the mechanical stop.
void exitstop(int percent){
  Serial.print(F("exitstop "));
  Serial.println(percent);
   //Going forward or backward at the given percentage
  digitalWrite(9, LOW);   //Disable brake on Channel A
  if(percent<0){
    digitalWrite(12, LOW); //Backward on Channel A
    analogWrite(3, 255*(-percent)/100);   //Spins the motor on Channel A at 'percent' of the max speed 
  }
  else{
    digitalWrite(12, HIGH); //Forward on Channel A
    analogWrite(3, 255*percent/100);   //Spins the motor on Channel A at 'percent' of the max speed
  }
  if(revsTot==0){
    revsTot=20;
  }
  else{
    revsTot-=20;
  }
  int t=20;//nb of rotation in order to go out of the mechanical stop
  int flagPre, flag;
  flagPre = 0;
  flag=0;
  while(t!=0){
    flag = digitalRead(22);  //Read pin 22
    
    if(flagPre==0 and flag == 1)//if we have a rising front
    {
      t--;//we decreased the number of revolution
      Serial.print(F("Revolutions : "));
      Serial.println(t);
    }
    flagPre=flag;
  }
  analogWrite(3,0); //stop the motor
  digitalWrite(9, HIGH); //Enabe brake on channel A
}


//Function who gets the system in emergency mode.
void emergencyMode(){
  Serial.println("Urgence");
  if(URG-revsTot<0){
    runMotor(-100*direc,abs(URG-revsTot));
    
  }
  else{
    runMotor(100*direc,URG-revsTot);
  }
  
  while((checkLimits() == 1)||(digitalRead(SWITCH)==LOW)){
    calculateFlow();
    calculatePower();
  }

  refreshInterface();
  heliostatForecast(); //When it leaves the emergency mode, it must find it tracks, it is only possible in all cases with the forecast mode.
}
