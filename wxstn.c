/** \file wxstn.c
 *  \brief Weather station control
 * \author Created: Alexander Guglenko
 * \date 28DEC019
*/

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <pcf8591.h>
#include "wxstn.h"
#include "hshbme280.h"
#include "tsl2561.h"

/** \brief Weather Station Initialization
 */
int WsInit(void)
{
	BME280Setup();

	return 1;
}

/** \brief Gets Weather Reading
 * \return reading_s
 *
 * Returns temperature, pressure, humidity for a specific time.
 */
reading_s WsGetReadings(void)
{
	reading_s now = {0};

	now.rtime = time(NULL);
	now.temperature = WsGetTemperature();
	now.humidity = WsGetHumidity();
	now.pressure = WsGetPressure();
	now.light = WsGetLight();
	now.windspeed = WsGetWindspeed();
	now.winddirection = WsGetWinddirection();
	return now;
}

/** \brief Gets Temperature
 * \return double
 *
 * Returns temperature in degrees Celsius
 */
double WsGetTemperature(void)
{
#if SIMTEMP
	return (double) WsGetRandom(USTEMP-LSTEMP)+LSTEMP;
#else
	return GetBME280TempC();
#endif
}

/** \brief Gets Humidity
 * \return double
 *
 * Returns relative humidity
 */
double WsGetHumidity(void)
{
#if SIMHUMID
	return (double) WsGetRandom(USHUMID-LSHUMID)+LSHUMID;
#else
	return GetBME280Humidity();
#endif
}

/** \brief Gets Pressure
 * \return double
 *
 * Returns pressure in milliBars
 */
double WsGetPressure(void)
{
#if SIMPRESS
	return (double) WsGetRandom(USPRESS-LSPRESS)+LSPRESS;
#else
	return PaTomB(GetBME280Pressure());
#endif
}

/** \brief Gets Light
 * \return double
 *
 * Returns relative value for light intensity
 */
double WsGetLight(void)
{
#if SIMLIGHT
	return (double) WsGetRandom(USLIGHT-LSLIGHT)+LSLIGHT;
#else
	return 0.0;
#endif
}

/** \brief Gets Windspeed
 * \return double
 *
 * Returns windspeed in kph
 */
double WsGetWindspeed(void)
{
#if SIMWINDSPD
    return (double) WsGetRandom(USWINDSPD-LSWINDSPD)+LSWINDSPD;
#else
	return 20.0;
#endif
}

/** \brief Gets Wind Direction
 * \return double
 *
 * Returns wind direction in degrees true
 */
double WsGetWinddirection(void)
{
#if SIMWINDDIR
    return (double) WsGetRandom(USWINDDIR-LSWINDDIR)+LSWINDDIR;
#else
	return 320.0;
#endif
}

/** \brief Gets Random Values
 * \param range integer
 * \return int
 *
 * Returns random value between 0 and range
 */
int WsGetRandom(int range)
{
	return rand() % range;
}

/** \brief Displays Weather Data to Console
 * \param dreads reading_s weather data structure
 */
void WsDisplayReadings(reading_s dreads)
{
    time_t now;

    now = time(NULL);

    printf("\n%s",ctime(&now));
    printf("Readings\t T: %3.1lfC\t H: %3.0lf%%\t P: %5.1lfmb\t WS: %3.0lfkmh\t WD: %3.0lfdegrees\n",
            dreads.temperature,dreads.humidity,dreads.pressure,dreads.windspeed,dreads.winddirection);
}


/** \brief Converts Pascals to millibars
 * \param pa double
 * \return double
 *
 */
double PaTomB(double pa)
{
	return pa/100.0;
}

/** \brief Millisecond Delay
 * \param milliseconds int
 *
 */
void WsDelay(int milliseconds)
{
    long wait;
    clock_t now,start;

    wait = milliseconds*(CLOCKS_PER_SEC/1000);
    start = clock();
    now = start;

    while((now-start) < wait)
    {
        now = clock();
    }
}

