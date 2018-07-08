#include "solarConcentrator.h"


File myFile;
unsigned long nbCapture = 0; //Every 5 minutes, all datas are collected and stored
int forecast[TAKES_PER_DAY]; //Datas of previous days, used by the forecast mode

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println(F("Initiating datalogger"));
  initDatalogger();//function of initialisation of the datalogger
  Serial.println(F("Initiating Heliostat"));
  initHeliostat();//function of initialisation of the heliostat
  wdtInitialisation();
  ini_led();
  
  //Setup Channel A
  pinMode(12, OUTPUT); //Initiates Motor Channel A pin
  pinMode(9, OUTPUT); //Initiates Brake Channel A pin

  pinMode(PIN_REV_COUNTER, INPUT); //pin connect to the system that count the revs
  pinMode(24, INPUT); //connect to the switch
  pinMode(PIN_MECH_STOP_1, INPUT);
  pinMode(PIN_MECH_STOP_2, INPUT_PULLUP);
  
	attachInterrupt(digitalPinToInterrupt(PIN_IN_FLOW), pulseCounter, FALLING);//interruption for the the flow sensor
  Serial.println(F("Entering sweeper"));
  sweeper();
  Serial.println(F("Launching"));
  
  MsTimer2::set(TIME_BETWEEN_TAKES, timer); // 5 min period
  MsTimer2::start();  //function that is call every 5 minutes
}

void loop() {
  //test the weather in order to choose between the reactive fonction and the forecasted one
  calculateLux();
  calculateFlow();
  refreshInterface();
  EmergencySwitch();
  if(lux>REACTIVE_LUM){//there is enouth sun to work in reactionnel
    heliostatReaction();
  }
  else if(lux>FORECAST_LUM){//there is not enough sun to work in reactionnel but enouth to work
    if(forecast[nbCapture] > 0)
      heliostatForecast();
    else
      in_forecast = 1;
  }
  //There is not enough sun to work
  else{
    
    if(f_wdt == 1)
    {
      /* Toggle the LED */
      Serial.println("Veille");
      f_wdt = 0;
    
      /* Re-enter sleep mode. */
      enterSleep();
    }
    else
    {
      /* Do nothing. */
    }
  }

  //call the emergency mode if current sensor values are over the limit values
  if(checkLimits() == 1){
    refreshInterface();//refresh the interface
    if(flagSteam||flagTemp1Max||flagTemp2Max||flagPressureMax||flagFlowMax){
      emergencyMode();//reach the position of the emergency and wait the end of the issue    
    }
  }
}

void timer()
{
  //Calculation of all the sensor values
  Serial.println("timer");
  calculatePower();
  //Save the data
  saveDatalogger();
  if(lux>REACTIVE_LUM && !in_sweep)
    forecast[nbCapture] = revsTot; 
  nbCapture++;
  Serial.println(nbCapture);
  Serial.println(forecast[nbCapture]);
  if(nbCapture == TAKES_PER_DAY)//When we have reached the end of the day
   {
    Serial.println("save...");
    //Increment the number of days
    nbCapture=0;
    nb_day++;
    //Create the new file for the datalogger
    initDatalogger();
    //Stock the new values of the heliostat 
    refreshHeliostat();
   }
}

