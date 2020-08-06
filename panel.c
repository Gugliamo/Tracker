/** \file panel.c
 *  \brief panel functions
 * \author Created: Alexander Guglenko
 * \date 14JAN2020
*/
#include <wiringPi.h>
#include <pcf8591.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "panel.h"
#include "wxstn.h"
#include "tsl2561.h"
#include "spa.h"
#include "gps.h"

positiondata_s positiontable [STMAXTBLSZ] = {0};

/** \brief Panel Initialization
 * \return
 */
int StPanelInitialization(void)
{
    int status;
    int i;

    wiringPiSetup();
    pcf8591Setup(ST_PCF8591_PINBASE,ST_PCF8591_I2CADR);
    WsInit();
    tsl2561Setup();
    StServoSetup();
    StStepperSetup();
	gps_init();


    //unsuccessful call to StRetrievePositionTable
    //executes a loop to initialize and save the table
    status = StRetrievePositionTable();
    if(status == STNOTABLE)
    {
        // Set up stepper motor azimuth part
        for(i=0; i<STMAXSTEPSZ;i++)
        {
            positiontable[i].apos = (int) ((double)(i)/STMAXAZ*(STPA360-STPA000))+STPA000;
            positiontable[i].cnt = (int) ((double)(i)/STMAXAZ*(STSTEPMAX-STSTEPMIN))+STSTEPMIN;
        }

        // Set up servomotor elevation part
        for(i=0; i<STMAXSERVSZ;i++)
        {
            positiontable[i].epos = (int) ((double)i/STMAXELDEG*(STPE090-STPE000))+STPE000;
            positiontable[i].pwm = (int) ((double)i/STMAXELDEG*(STPWMMAX-STPWMMIN))+STPWMMIN;
        }
        for(i=STMAXSERVSZ; i<STMAXSTEPSZ;i++)
        {
            positiontable[i].epos = (int) STPE090;
            positiontable[i].pwm = (int) STPWMMAX;
        }
        StSavePositionTable();

    }
}

/** \brief Get LDR readings
 * \return csens
 * \author AlexanderGuglenko
 * \date 24JAN2020
 */
ldrsensor_s StGetLdrReadings(void)
{
    ldrsensor_s csens = {0};

    #if SIMLDR
        csens.aset=STSACTR;
        csens.eset=STSECTR;
    #else
        csens.aset=analogRead(ST_PCF8591_PINBASE+2);
        csens.eset=analogRead(ST_PCF8591_PINBASE+3);
    #endif // SIMLDR

    return csens;

}

/** \brief Display LDR readings
 * \author Alexander Guglenko
 * \date 24JAN2020
 */
void StDisplayLdrReadings(ldrsensor_s dsens)
{
    fprintf(stdout,"Elevation LDR: %3d Azimuth LDR: %3d\n",dsens.eset,dsens.aset);
}

/** \brief Servo Motor Setup
 * \author Alexander Guglenko
 * \date 31JAN2020
 */
void StServoSetup(void)
{
    pinMode(STELPIN,PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(STPWMCLOCK);
	pwmSetRange(STPWMRANGE);
}

/** \brief Servo Motor Setup
 * \author Alexander Guglenko
 * \date 31JAN2020
 */
void StStepperSetup(void)
{
    pinMode(STAZSTEP,OUTPUT);
    pinMode(STAZDIR,OUTPUT);
}

/** \brief Get Panel Position
 * \return cpos
 * \author AlexanderGuglenko
 * \date 24JAN2020
 */
panelpos_s StGetPanelPosition(void)
{
    panelpos_s cpos = {0.0};
    double el,az;

    az = analogRead(ST_PCF8591_PINBASE+0);
    el = analogRead(ST_PCF8591_PINBASE+1);
    cpos.Elevation = STMAXELDEG * (double) (el-STPE000)/(double) (STPE090-STPE000);
    cpos.Azimuth = ((STMAXAZDEG - STMINAZDEG) * (double) (az - STPA060) / (double) (STPA315-STPA060));
    if(cpos.Azimuth<0) {cpos.Azimuth=0.0;}
    return cpos;
}

/** \brief Set Panel Position
 * \author Alexander Guglenko
 * \date 31JAN2020
 */
void StSetPanelPosition(panelpos_s newpos)
{
    panelpos_s cpos;
    int pwmnel, stepnaz, stepcaz, dir, steps,i;

    cpos = StGetPanelPosition();

    if(newpos.Azimuth < STMINAZDEG) {newpos.Azimuth=STMINAZDEG;}
    if(newpos.Elevation < STMINELDEG) {newpos.Elevation=STMINELDEG;}
    if(newpos.Azimuth > STMAXAZDEG) {newpos.Azimuth=STMAXAZDEG;}
    if(newpos.Elevation > STMAXELDEG) {newpos.Elevation=STMAXELDEG;}
    pwmnel=positiontable[(int)newpos.Elevation].pwm;
	stepnaz=positiontable[(int)newpos.Azimuth].cnt;
    stepcaz=positiontable[(int)cpos.Azimuth].cnt;
    pwmWrite(STELPIN,pwmnel);
    delay(STPWMDELAY);

    if((stepnaz-stepcaz) < 0)
        {dir=LOW;}
    else
        {dir=HIGH;}

    digitalWrite(STAZDIR,dir);
    steps=abs(stepnaz-stepcaz);

    for(i=0;i<steps;i++)
    {
        digitalWrite(STAZSTEP,HIGH);
        delay(STSTEPDELAY);
        digitalWrite(STAZSTEP,LOW);
        delay(STSTEPDELAY);
    }

}

/** \brief Save Position to File
 * \return 1
 * \author Alexander Guglenko
 * \date 10Feb2020
 */
int StSavePositionTable(void)
{
#if STPOSTBLTXT
    FILE * fp;
    int i,deg;
    char dg ='$';

    fp = fopen("position.txt","w");
    if(fp == NULL) { return 0; }
    for(i=0; i<STMAXTBLSZ; i++)
    {
        fprintf(fp,"%c%3d,%3d,%4d,%3d", dg,i,positiontable[i].apos,positiontable[i].cnt,
        positiontable[i].epos,positiontable[i].pwm);
    }
    fclose(fp);
    return 1;
#else
    FILE * fp;

    fp = fopen("position.dat","w");
    if(fp == NULL) { return 0; }
    fread(&positiontable,sizeof(positiontable),1,fp);
    fclose(fp);
    return 1;
#endif // STPOSTBLTXT

}

/** \brief Retrieve Position from File
 * \return 1
 * \author AlexanderGuglenko
 * \date 10JFEB2020
 */
int StRetrievePositionTable(void)
{
#if STPOSTBLTXT
    FILE * fp;
    int i,deg;
    char dg;

    fp = fopen("position.txt","r");
    if(fp == NULL) { return 0; }
    for(i=0; i<STMAXTBLSZ; i++)
    {
        fscanf(fp,"%c%3d,%3d,%4d,%3d", &dg,&deg,&positiontable[i].apos,&positiontable[i].cnt,
        &positiontable[i].epos,&positiontable[i].pwm);
    }
    fclose(fp);
    return 1;
#else
    FILE * fp;

    fp = fopen("position.dat","r");
    if(fp == NULL) { return 0; }
    fwrite(&positiontable,sizeof(positiontable),1,fp);
    fclose(fp);
    return 1;
#endif // STPOSTBLTXT
}

/** \brief Calculate Panel position using spa.c/h file functions
 * \return Calculated position in relation to the sun at current data/time
 * \author AlexanderGuglenko
 * \date 06Mar2020
 */
panelpos_s StCalculateNewPanelPosition(void)
{
	panelpos_s newpos = {0.0};
	spa_data csp = {0.0};
	time_t rawtime;
	struct tm *ct;
	loc_t gpsloc;

	time(&rawtime);
	ct = localtime(&rawtime);
	gpsloc = gps_location();

	//setting up spa_data structure
	csp.year			= ct->tm_year+1900;
	csp.month			= ct->tm_mon+1;
    csp.day				= ct->tm_mday;
    csp.hour			= ct->tm_hour;
    csp.minute			= ct->tm_min;
    csp.second			= ct->tm_sec;
    csp.delta_ut1		= DELTAUT1;
    csp.delta_t			= DELTAT;
	csp.timezone 		= DTIMEZONE;
	csp.pressure		= WsGetPressure();
    csp.temperature 	= WsGetTemperature();
    csp.slope       	= DSLOPE;
    csp.azm_rotation	= DAZROT;
    csp.atmos_refract   = DATMREF;
    csp.function        = SPA_ALL;
	if(gpsloc.latitude != 0 && gpsloc.longitude != 0)
	{
		csp.longitude  = gpsloc.longitude;
        csp.latitude   = gpsloc.latitude;
        csp.elevation  = gpsloc.altitude;
		newpos.gpsdata = gpspos;
	}
	else
	{
		csp.latitude = DLATITUDE;
		csp.longitude = DLONGITUDE;
		csp.elevation = PELEVATION;
		newpos.gpsdata.latitude = csp.latitude;
        newpos.gpsdata.longitude = csp.longitude;
	}
	spa_calculate(&csp);
	newpos.Azimuth = csp.azimuth;
	newpos.Elevation = 90.0 - csp.incidence;

	return newpos;

}

/** \brief Move solar panel in position with spa.c/h calculated values
 * \return Calculated position with LDR adjustments
 * \author AlexanderGuglenko
 * \date 06Mar2020
 */
panelpos_s StTrackSun(void)
{
	panelpos_s tpos = {0.0};
	ldrsensor_s spos = {0.0};
	int i;

	tpos = StCalculateNewPanelPosition();

	//if night time, park Solar panel
	if (tpos.Elevation <= 0)
	{
		tpos.Elevation = PELEVATION;
		tpos.Azimuth = PAZIMUTH;
		StSetPanelPosition(tpos);
	}
	else
	{
		StSetPanelPosition(tpos);
	}

	//Testing ldr sensor and adjusting if more than 10 units side of centre
	for(i=0;i<10;i++)
	{
		spos = StGetLdrReadings();
		if(spos.eset > STSECTR + 10) {tpos.Elevation++;}
		if(spos.eset < STSECTR - 10) {tpos.Elevation--;}
		if(spos.aset > STSACTR + 10) {tpos.Azimuth++;}
		if(spos.aset < STSACTR - 10) {tpos.Azimuth--;}
		StSetPanelPosition(tpos);
	}
	return tpos;
}

