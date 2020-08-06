#ifndef PANEL_H
#define PANEL_H
/** \file panel.h
 * \brief Constants, structures, function prototypes for panel functions
 * \author Alexander Guglenko
 * \date 21JAN2019
*/
#include "gps.h"

// I2C Device Addresses
#define ST_RTC_I2CADR		0x68
#define ST_MCP23017_I2CADR	0x20
#define ST_PCF8591_I2CADR	0x48
#define ST_BME280_I2CADR	0x77

// wiringPi Device Pin Base
#define ST_MCP23017_PINBASE	100
#define ST_PCF8591_PINBASE	200
#define ST_BME280_PINBASE	300

// Panel Position constants
#define STSIMPOS 1

// Theoretical Limits
#define STMAXEL     90.0
#define STMINEL     0.0
#define STMAXAZ     360.0
#define STMINAZ     0.0

// Hardware Limits
#define STMAXELDEG  90.0
#define STMINELDEG  0.0
#define STMAXAZDEG  315.0
#define STMINAZDEG  60.0

// Servo Constants
#define STELPIN     1
#define STSERVORANGE  90.0     // Servo maximum travel 90 degrees
#define STPWMCLOCK  384
#define STPWMRANGE  1000
#define STPWMMAX    105.0
#define STPWMMIN    40.0         // 5.0 lowest value offset 30 degrees up, 45.0
#define STPWMDELAY  250

// Stepper Constants
#define STAZSTEP    2
#define STAZDIR     3
#define STSTEPRANGE 360.0
#define STSTEPMAX   1000
#define STSTEPMIN   0
#define STSTEPDELAY 5

// Position Feedback Constants
#define STPE000     45.0        // Low end offset by 30 degrees
#define STPE090     105.0
#define STPA000     19.0
#define STPA060     20.0
#define STPA315     245.0
#define STPA360     255.0
#define STSAVEPRD   1
#define STMAXPSZ    360

//Position Table Constants
#define STNOTABLE   0
#define STPOSTBLTXT 1
#define STMAXTBLSZ  361
#define STMAXSTEPSZ 360
#define STMAXSERVSZ 90

// Solar Position Constants
#define DELTAUT1 0.0
#define DELTAT 67.0
#define DTIMEZONE -4.0
#define DLATITUDE 43.63409
#define DLONGITUDE -79.45930
#define DALTITUDE 166.0
#define DPANELHDG 0.0
#define PAZIMUTH 180.0
#define PELEVATION 0.0
#define DSLOPE 0.0
#define DAZROT 0.0
#define DATMREF 0.5667


//other constants
#define SIMLDR 0
#define STSECTR 127
#define STSACTR 127
#define SIMTRACK 0

// Structures
typedef struct panelpos
{
    double Azimuth;
    double Elevation;
	loc_t gpsdata;
} panelpos_s;

typedef struct ldrsensor
{
    int aset;
    int eset;
} ldrsensor_s;

typedef struct positiondata
{
    int apos;   //< Analog azimuth position
    int cnt;    //< Azimuth stepper count
    int epos;   //< Analog elevation position
    int pwm;    //< Elevation pwm value
} positiondata_s;


///\cond INTERNAL
// Function Prototypes
int StPanelInitialization(void);
ldrsensor_s StGetLdrReadings(void);
void StDisplayLdrReadings(ldrsensor_s dsens);
void StServoSetup(void);
void StStepperSetup(void);
panelpos_s StGetPanelPosition(void);
void StSetPanelPosition(panelpos_s newpos);
int StSavePositionTable(void);
int StRetrievePositionTable(void);
panelpos_s StCalculateNewPanelPosition(void);
panelpos_s StTrackSun(void);


///\endcond

#endif
