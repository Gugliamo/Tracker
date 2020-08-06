//
//  This example demonstrates how to use GLG control widgets as they are,
//  without emdedding them into another GLG drawing, and handle user
//  interaction.
//
//  The drawing name is GhGUI.g.
//

/**\file sptglgmain.c
 * \brief Glg main function
 * \author Created:Alexander Guglenko
 * \date 14Feb2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GlgApi.h>
#include <time.h>
#include "sptglgmain.h"
#include "wxstn.h"
#include "tsl2561.h"
#include "panel.h"

// Top level global variables
GlgAppContext AppContext;
GlgObject SptDrawing;
GlgLong UpdateInterval = UPDATE_INTERVAL;
int TrackOn = STOFF;

// Defines a platform-specific program entry point
#include "GlgMain.h"

/** \brief Glg main
 * \author Alexander Guglenko
 * \return 0
 * \date 14Feb2020
 */
int GlgMain(int argc,char *argv[],GlgAppContext InitAppContext )
{
	// Initialization Section
	AppContext = GlgInit( False, InitAppContext, argc, argv );
	GlgSetDResource(AppContext,"$config/GlgOpenGLMode",0.); // Disable openGL

    // Initialize the Solar Panel Tracker
	StPanelInitialization();

	// Load a drawing from the file.
	SptDrawing = GlgLoadWidgetFromFile( "SPTrack.g" );

	if(!SptDrawing) { exit( 1 ); }

	// Setting widget dimensions using world coordinates [-1000;1000].
	GlgSetGResource(SptDrawing,"Point1",-600.,-600.,0.);
	GlgSetGResource(SptDrawing,"Point2",600.,600.,0.);

	// Setting the window name (title).
	GlgSetSResource(SptDrawing,"ScreenName","Alex Guglenko's Solar Panel Tracker" );

	// Add Input callback to handle user interraction in the GLG control.
	GlgAddCallback(SptDrawing,GLG_INPUT_CB,(GlgCallbackProc)Input,NULL );

	// Paint the drawing.
	GlgInitialDraw(SptDrawing);
	GlgAddTimeOut(AppContext,UpdateInterval,(GlgTimerProc)UpdateControls,NULL );
	return (int) GlgMainLoop( AppContext );
}

//
// This callback is invoked when user interacts with input objects in GLG
// a  drawing, such as a slider, dial or a button.
//
/** \brief Detecting user input
 * \author Alexander Guglenko
 * \date 14Feb2020
 */
void Input(GlgObject GhDrawing,GlgAnyType client_data,GlgAnyType call_data)
{
	GlgObject message_obj;
	char * format,* action,* origin, * full_origin;
	double pstate,svalue;
	message_obj = (GlgObject) call_data;

	// Get the message's format, action and origin.
	GlgGetSResource(message_obj,"Format", &format);
	GlgGetSResource(message_obj,"Action",&action);
	GlgGetSResource(message_obj,"Origin",&origin);
	GlgGetSResource(message_obj,"FullOrigin",&full_origin);

	// Handle window closing. May use GhDrawing's name.
	if(strcmp(format,"Window") == 0 &&
		strcmp(action,"DeleteWindow") == 0)
	{ exit(0); }

    // Retrieve OnState value from Track1
    if( strcmp(format,"Button") == 0 && strcmp(action,"ValueChanged") == 0)
    {
        GlgGetDResource(SptDrawing,"Track1/OnState", &svalue );
        if(svalue == 1.0)
        {
            TrackOn = 1;
            GlgSetDResource(SptDrawing,"Elevation1/DisableInput",1);
            GlgSetDResource(SptDrawing,"Azimuth1/DisableInput",1);
        }
        else
        {
            TrackOn = 0;
            GlgSetDResource(SptDrawing,"Elevation1/DisableInput",0);
            GlgSetDResource(SptDrawing,"Azimuth1/DisableInput",0);
        }
    }
}

//
// Timer procedure to update controls, and drawing values
//
/** \brief Timer procedure to update controls, and drawing values
 * \author Alexander Guglenko
 * \date 14Feb2020
 */
void UpdateControls(GlgAnyType data,GlgIntervalID * id)
{
	reading_s creads = {0};
	ldrsensor_s cldr={0};
    panelpos_s selaz={0};
	int clux=0;
	srand(time(0));
	double elValue=selaz.Elevation;
	double azValue=selaz.Azimuth;

	cldr = StGetLdrReadings();
    creads=WsGetReadings();
    clux=tsl2561GetLux();
	selaz=StGetPanelPosition();
	srand(time(0));

	if(TrackOn)
	{
	#if SIMTRACK
			selaz.Elevation=(double) rand()%91;
			selaz.Azimuth=(double) rand()%181;
	#else
			selaz = StTrackSun();
	#endif
	}
	else
	{
		GlgGetDResource(SptDrawing,"Elevation1/Value",&selaz.Elevation);
		GlgGetDResource(SptDrawing,"Azimuth1/Value",&selaz.Azimuth);
	}	
	StSetPanelPosition(selaz);


	GlgSetDResource(SptDrawing,"Temp1/Value",creads.temperature);
	GlgSetDResource(SptDrawing,"Humid1/Value",creads.humidity);
	GlgSetDResource(SptDrawing,"Press1/Value",creads.pressure);
	GlgSetDResource(SptDrawing,"Windspeed1/Value",creads.windspeed);
	GlgSetDResource(SptDrawing,"WindDirection1/Value",creads.winddirection);
	GlgSetDResource(SptDrawing,"Luminosity1/Value",clux);
	GlgSetDResource(SptDrawing,"Elevation1/Value",selaz.Elevation);
	GlgSetDResource(SptDrawing,"Azimuth1/Value",selaz.Azimuth);
	GlgSetDResource(SptDrawing,"Latitude1/Value",selaz.gpsdata.latitude);
	GlgSetDResource(SptDrawing,"Longitude1/Value",selaz.gpsdata.longitude);
	GlgUpdate(SptDrawing);
	GlgAddTimeOut(AppContext,UpdateInterval,(GlgTimerProc)UpdateControls,NULL );
}
