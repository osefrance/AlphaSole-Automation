//Read the photodiods values and compare them to control the motor
void heliostatReaction() {
  //sweeper if the last value in the forecast is -1
  if(nbCapture)//if we aren't at the first capture we check if the previous value isn't defined
  { 
    
    if(forecast[nbCapture-1] < 0 && in_forecast)
     {
      in_sweep = 1;
      sweeper();
      in_forecast = 0;
     }
  }
  else{
   
    if(forecast[nbCapture] < 0 && in_forecast)
     {
      in_sweep = 1;
      sweeper();
      in_forecast = 0;
     }
  }
  float cd = analogRead(PIN_IN_PHOTODIOD_RIGHT);
  cd=cd/1024.0*5.0;
  float cg = analogRead(PIN_IN_PHOTODIOD_LEFT);
  cg=cg/1024.0*5.0;

  float delt = cd-cg;
  
  if(delt > DELTA){
     runMotor(100*direc,1); //run the motor for 1 rev
     return;
  } else if (delt < -DELTA) {
    runMotor(-100*direc,1);
    return;
  }
  return;
}

//Function who runs the heliostat in the forecast mode
void heliostatForecast(){
  int delta;
  //compute the difference between the actual position and the forecast one
  delta = forecast[nbCapture] - revsTot;
  if(delta < 0)
    runMotor(-100*direc,-delta);
  else
    runMotor(100*direc,delta);
  return;
}
