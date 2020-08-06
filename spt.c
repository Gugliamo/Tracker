/**\file spt.c
 * \brief Solar Panel Tracker main function
 * \author Alexander Guglenko
 * \date 10JAN2020
 */
#include <stdio.h>
#include "wxstn.h"
#include "tsl2561.h"
#include "panel.h"

/**\brief Weather Main Control Loop
 * \return int status
 */
int main(void)
{
    reading_s creads = {0};
	ldrsensor_s cldr={0};
    panelpos_s selaz={0};
	
	int clux=0;
    int i=0;     
    int az[]={45,60,75,90,105,120,135,150,165,180,195,210,225,240,255,270,285,300,315};
    int el[]={0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90};

    StPanelInitialization();
    fprintf(stdout,"\nCENG252 Weather Station\n");
	
    while(1)
    {
        creads=WsGetReadings();
		WsDisplayReadings(creads);
		
        clux=tsl2561GetLux();
        cldr=StGetLdrReadings();
        
        tsl2561DisplayLux(clux);
        StDisplayLdrReadings(cldr);

        selaz.Elevation=el[i];
        selaz.Azimuth=az[i];
        fprintf(stdout,"AA: %3d EA: %3d\n",az[i],el[i]);
        StSetPanelPosition(selaz);
        selaz=StGetPanelPosition();
        fprintf(stdout,"Azimuth: %3.0f  Elevation: %3.0f\n",selaz.Azimuth,selaz.Elevation);
        i++;
        if(i>18) {i=0;}

        WsDelay(3000);
    }
}
