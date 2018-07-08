//Function that computes the temperature in Celsius
float calculateTemperature(int pinNumber)
{
  int val_read = 0;  //Value read on the pin
  float vin = 0.0;
  float rth = 0.0;   //Value of the thermal resistor
  float temp = 0.0;
  float discriminatory = 0.0;

  delay(100); //this delay is necessary to avoid bug on the sensor value
  val_read = analogRead(pinNumber);
  vin = (val_read * 5.0) / 1024.0;
  //Computation of the thermal resistance thanks to a tension divider bridge.
  rth =((SUPPLY_3V3 - vin) * RESISTOR_SERIE) / vin;

  //Computation of the temperature(formulas are from the datasheets of the NB-PTCO-035).
  discriminatory = (A*A - 4.0*B*(1.0 - rth/1000.0));

  temp = -A + sqrt(discriminatory);
  temp = temp / (2.0*B);
  if(temp < 0)
  {
    flagBug = 1;
    Serial.print("TEMP BUG ; TEMP < 0 ");
    Serial.println(pinNumber);
  }
  return temp;
}

//Compute the flow rate in Liter by minutes
void calculateFlow()
{
   if((millis() - oldTime) > 1000)
    {
      //Disable the interrupt while calculating flow rate and sending the value to the host
      detachInterrupt(PIN_IN_FLOW);
      
      //Because this loop may not complete in exactly 1 second intervals we calculate
      //the number of milliseconds that have passed since the last execution and use
      //that to scale the output. We also apply the calibrationFactor to scale the output
      //based on the number of pulses per second per units of measure (litres/minute in
      //this case) coming from the sensor.
      flowRate = ((1000.0/ (millis() - oldTime)) * pulseCount) / CALIBRATION_FACTOR;
      
      //Note the time this processing pass was executed. Note that because we've
      //disabled interrupts the millis() function won't actually be incrementing right
      //at this point, but it will still return the value it was set to just before
      //interrupts went away.
      oldTime = millis();
      
      //Reset du compteur de pulse
      pulseCount = 0;
      
      //Réactivation de l'interruption
      attachInterrupt(PIN_IN_FLOW, pulseCounter, FALLING);
      //Serial.print("Flow : ");
      //Serial.println(flowRate);
   }
}

//Compute the pressure in bar
void calculatePressure()
{
  int val_read_pressure;
  float vin_pressure;
  val_read_pressure = analogRead(PIN_IN_PRESSURE);
  vin_pressure = val_read_pressure * 5.0 / 1024.0;
  pressure = FUNCTION_PRESSURE(vin_pressure);
  if(pressure < 0)
  {
    flagBug = 1;
    Serial.println("Pressure BUG ; Pressure < 0");
  }
  //Serial.print("Pressure : ");
  //Serial.println(pressure);
}

//Compute the thermic power in W
void calculatePower()
{
  temp1 = calculateTemperature(PIN_IN_TEMPERATURE_1);
  temp2 = calculateTemperature(PIN_IN_TEMPERATURE_2);
  calculatePressure();
  power = 60*flowRate*VOLUM_MASS_DENSITY*HEAT_CAPACITY*abs(temp2-temp1)/3600.0; //power in kcal/h
  Serial.print("Power : ");
  Serial.println(power);
}

void pulseCounter()
{
  pulseCount++;
}

//Compute a value in a unity close to the lux
void calculateLux()
{
    float Vresistor = analogRead(PIN_IN_LUX);
    Vresistor = Vresistor *5.0/1023.0;
    float photodiod = 5.0 - Vresistor;
    float Rphotodiod = photodiod / Vresistor * 4700;
    lux = LUX_FACTOR * pow(Rphotodiod,LUX_EXPONENT);
    //Serial.println(lux);
    if(lux < 0)
    {
      flagBug = 1;
      Serial.println("LUX BUG ; LUX < 0");
    }
}

//This function check if all the values are over the limits, if it is the case it returns 1, else 0.
int checkLimits()
{
  
  if((abs(temp1- temp2) < TEMP_DELTA)&&(temp1> TEMP_MIN)){ //Only steam in the receiver
    flagSteam = 1;
  }
  else{
    flagSteam = 0;
  }
  if(temp1 >= TEMPERATURE_MAX)
    flagTemp1Max = 1;
  else
    flagTemp1Max = 0;
  if(temp2 >= TEMPERATURE_MAX)
    flagTemp2Max = 1;
  else
    flagTemp2Max = 0;
  if(pressure >= PRESSURE_MAX)
    flagPressureMax = 1;
  else
    flagPressureMax = 0;
  if(flowRate >= FLOW_MAX)
    flagFlowMax = 1;
  else
    flagFlowMax = 0;
        
  if(flagPressureMax || flagTemp1Max || flagTemp2Max || flagFlowMax || flagSteam)
    return 1;     
  return 0;
}



