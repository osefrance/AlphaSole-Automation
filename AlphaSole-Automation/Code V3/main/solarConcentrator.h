#include <SPI.h>
#include <SD.h>
#include <stdio.h>
#include <MsTimer2.h>
#include <string.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

//alterable parameter
#define NB_ROT_TOT 783

#define DELTA 0.10 //The delta between the values of the 2 photodiods from which the motor reposition itself
#define URG 10 //Emergency position in turns number

#define REACTIVE_LUM 5000
#define FORECAST_LUM 500

#define TEMP_DELTA 3
#define TEMP_MIN 90

#define TEMPERATURE_MAX 300 //degree Celsius
#define PRESSURE_MAX 40 //bar
#define FLOW_MAX 30 // litres/minute
#define POWER_MAX 10000 

#define VOLUM_MASS_DENSITY 1000 //The volumetric mass density of the fluid kg/m^3
#define HEAT_CAPACITY 1.850 //The heat capacity kJ/(kg.K)



//Datalogger
#define TAKES_PER_DAY 288  //288 for 24h, 168 for 14h
#define TIME_BETWEEN_TAKES 300000 //5 min (1000 for 1 sec)

//Supply
#define SUPPLY_3V3 3.3
#define SUPPLY_5V 5.0


//Analog Pins
#define PIN_IN_TEMPERATURE_1 A13
#define PIN_IN_TEMPERATURE_2 A12
#define PIN_IN_PRESSURE A9 
#define PIN_IN_PHOTODIOD_RIGHT A14
#define PIN_IN_PHOTODIOD_LEFT A6
#define PIN_IN_LUX A7

//Digital Pins
#define PIN_IN_FLOW 20
#define PIN_REV_COUNTER 22
#define PIN_MECH_STOP_1 18
#define PIN_MECH_STOP_2 19

//Led Pin
#define WALL_1 30
#define WALL_2 31
#define WALL_3 32
#define WALL_4 33
#define WALL_5 34
#define WALL_6 35
#define WALL_7 36
#define WALL_8 37
#define WALL_9 38
#define WALL_10 39

#define RED_1 40
#define RED_2 41
#define RED_3 42
#define RED_4 43

#define GREEN_1 44
#define GREEN_2 45

//Switch Pin
#define SWITCH 26


//Factors for the temperature sensor
#define A 0.0039083
#define B -0.0000005775
#define RESISTOR_SERIE 2200.0


//Factor for the flow sensor
#define CALIBRATION_FACTOR 4.5 //Number of pulse per second per litres/minutes

//Factors for the pressure sensor
#define SLOPE_PRESSURE 17.045     //Volt par bar
#define CONSTANT_PRESSURE -4.625
#define FUNCTION_PRESSURE(x) (x * SLOPE_PRESSURE + CONSTANT_PRESSURE) //Give the pressure in bar

//Factor for the computation of the illuminance
#define LUX_EXPONENT -1.405
#define LUX_FACTOR 12518931



//Variables for the emergency mod
int flagTemp1Max = 0;
int flagTemp2Max = 0;
int flagPressureMax = 0;
int flagFlowMax = 0;
int flagBug = 0;
int flagSteam = 0;

byte pulseCount = 0;
unsigned long oldTime;

//All the sensor variavles
float temp1;	//Temperature1 sensors variables
float temp2; //Temperature2 sensors variables
float flowRate; //Flowrate sensor variables
float pressure; //Pressure sensor variables
float power;  //Power variable
double lux; //Illuminance variable

//Motor control variables
int nb_day = 0;
int flag = 0;
int revsTot = 0; //System state, total of motor revolutions
volatile byte mechStop = LOW; //Flag to know if the motor is at a mechanical stop
int direc=0;
int in_sweep = 0;
int in_forecast = 0;

//Sleep Mode variables

volatile int f_wdt=1;

