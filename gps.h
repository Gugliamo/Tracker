#ifndef GPS_H
#define GPS_H
/** \file gps.h
 * \brief Constants, structures, function prototypes for gps functions
 * \author Alexander Guglenko
 * \date 12JAN2020
*/
#define round(x) ((x < 0) ? (ceil((x)-0.5)) : (floor((x)+0.5)))
#define SIMGPS	1
#define	GPSBUFFERSZ	20
#define	GPSDATASZ	256

struct location
{
	double utc;
	double date;
    double latitude;
    double longitude;
    double speed;
    double altitude;
    double course;
};

typedef struct location loc_t;

///\cond INTERNAL
// Function Prototypes
// Initialize device
extern void gps_init(void);

// Activate device
extern void gps_on(void);

// Get the actual location
loc_t gps_location(void);


// Turn off device (low-power consumption)
extern void gps_off(void);

// -------------------------------------------------------------------------
// Internal functions
// -------------------------------------------------------------------------

// convert deg to decimal deg latitude, (N/S), longitude, (W/E)
void gps_convert_deg_to_dec(double *, char, double *, char);
double gps_deg_dec(double);
//read gps data from file
int StReadGPSData(char * fname);
///\endcond
#endif
