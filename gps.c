/** \file gps.c
 * \brief gps Functions
 * \author Created: Alexander Guglenko
 * \date 12JAN2020
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "gps.h"
#include "nmea.h"
#include "serial.h"

nmeamsg_s gpsbuffer[GPSBUFFERSZ] = {0};

extern void gps_init(void)
{
	#if SIMGPS
		StReadGPSData("gpslog02.txt");
	#else
		serial_init();
		serial_config();
	#endif
}

extern void gps_on(void)
{
    //Write on
}

// Compute the GPS location using decimal scale
loc_t gps_location(void)
{
    uint8_t status = _EMPTY;
	loc_t clos = {0};
	int i = 0;
	char buffer[GPSDATASZ] = {0};
	gpgga_t gpgga;
    gprmc_t gprmc;

    while(status != _COMPLETED)
	{
		#if SIMGPS
			strncpy(buffer,gpsbuffer[i].msgstr,NMEAMSGSZ+1);
			i++;
		#else
			serial_readln(buffer,GPSDATASZ);
		#endif //SIMGPS
        switch (nmea_get_message_type(buffer))
		{
            case NMEA_GPGGA:
                nmea_parse_gpgga(buffer, &gpgga);
				clos.utc = gpgga.utc;
                gps_convert_deg_to_dec(&(gpgga.latitude), gpgga.lat, &(gpgga.longitude), gpgga.lon);
                clos.latitude = gpgga.latitude;
                clos.longitude = gpgga.longitude;
                clos.altitude = gpgga.altitude;
				status |= NMEA_GPGGA;
				break;
            case NMEA_GPRMC:
                nmea_parse_gprmc(buffer, &gprmc);
				clos.speed = gprmc.speed;
                clos.course = gprmc.course;
				clos.date = gprmc.date;
                status |= NMEA_GPRMC;
 				break;
        }
    }
    return clos;
}

extern void gps_off(void)
{
    //Write off
//    serial_close();
}

// Convert lat e lon to decimals (from deg)
void gps_convert_deg_to_dec(double *latitude, char ns,  double *longitude, char we)
{
    double lat = (ns == 'N') ? *latitude : -1 * (*latitude);
    double lon = (we == 'E') ? *longitude : -1 * (*longitude);

    *latitude = gps_deg_dec(lat);
    *longitude = gps_deg_dec(lon);
}

double gps_deg_dec(double deg_point)
{
    double ddeg;
    double sec = modf(deg_point, &ddeg)*60;
    int deg = (int)(ddeg/100);
    int min = (int)(deg_point-(deg*100));

    double absmlat = round(min * 1000000.);
    double absslat = round(sec * 1000000.);
    double absdlat = round(deg * 1000000.);

    return round(absdlat + (absmlat/60) + (absslat/3600)) /1000000;
}

int StReadGPSData(char * fname)
{
	FILE *fp;
	int i;
	fp = fopen(fname,"r");
	if(fp == NULL){return 0;}
	for(i = 0;i < GPSBUFFERSZ; i++)
	{
		fgets(gpsbuffer[i].msgstr,NMEAMSGSZ,fp);
	}
	fclose(fp);
	return 1;
}
