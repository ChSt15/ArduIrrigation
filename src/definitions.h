#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#include <Arduino.h>


//##### Versions #####
#define BoardVersionV1_0 //Board Version for pins etc.
#define SoftwareVersion 1


//General Settings
#define CPU_SPEED 80

#define BASESTATION_ID 200
#define NODE_BROADCAST BASESTATION_ID+1

#define DISPLAY_HEIGHT 64
#define DISPLAY_WIDTH 128

#define MAX_NUM_NODES 255

#define POWERSAVING_ENABLE_THRESHOLD 3.5f
#define POWERSAVING_DISABLE_THRESHOLD 3.6f
#define POWERSHUTDOWN_THRESHOLD 3.3f


//Max number of watering per day
#define MAX_VALVE_TIMING 20


//GPIO Expander
#define GPIOExpanderCSPin 13
#define GPIOExpanderAddr 0

//Valve Control Pins (Pins of GPIO expander)
#define Valve1APin 1
#define Valve1BPin 0

#define Valve2APin 3
#define Valve2BPin 2

#define Valve3APin 5
#define Valve3BPin 4

#define Valve4APin 7
#define Valve4BPin 6

#define Valve5APin 9
#define Valve5BPin 8

#define Valve6APin 11
#define Valve6BPin 10

#define Valve7APin 13
#define Valve7BPin 12

#define Valve8APin 15
#define Valve8BPin 14

//ADC Input Pins
#define ADCInput1Pin 26
#define ADCInput1Factor 1/4096.0f
#define ADCInput2Pin 27
#define ADCInput2Factor 1/4096.0f

//VBat Input
#define ADCINPUTVBATPIN 34
#define ADCINPUTVBATFACTOR 2.0f*3.3f

//Touch Input
#define TOUCHINPUTPIN1 14
#define TOUCHINPUTPIN2 12
#define TOUCHINPUTPIN3 15

//VEXT Power Control
#define VEXTCONTROLPIN 4

//RGB LED
#define RGBLEDPin 5

//SX1280
#define RFBUSY 35
#define DIO1 32
#define NSS 33
#define NRESET 25
#define TXEN 17
#define RXEN 16



#endif