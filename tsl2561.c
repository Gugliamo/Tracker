/** \file tsl2561.c
 *  \brief tsl2561 functions
 * \author Created: Alexander Guglenko
 * \date 10JAN2020
*/

#include <inttypes.h>
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "tsl2561.h"

//file scope variable
static int TSL2561fd;

/** \brief tsl2561 Initialization
 */
void tsl2561Setup(void)
{
    TSL2561fd=wiringPiI2CSetup(TSL2561_ADDR_FLOAT);
}

/** \brief Gets Light Intensity
// * \return int visible_and_ir * 2
// */
int tsl2561GetLux(void)
{
    uint16_t visible_and_ir = 0;

    wiringPiI2CWriteReg8(TSL2561fd,TSL2561_COMMAND_BIT,TSL2561_CONTROL_POWERON);
    wiringPiI2CWriteReg8(TSL2561fd,TSL2561_REGISTER_TIMING,TSL2561_GAIN_AUTO);
    delay(LUXDELAY);
    visible_and_ir = wiringPiI2CReadReg16(TSL2561fd,TSL2561_REGISTER_CHAN0_LOW);
    wiringPiI2CWriteReg8(TSL2561fd,TSL2561_COMMAND_BIT,TSL2561_CONTROL_POWEROFF);

    return visible_and_ir * 2;
}

/** \brief Displays Lux Readings
// * \return int LUX
//
// * \author Alexander Guglenko
// * \date 24JAN2020
// */
void tsl2561DisplayLux(int dlux)
{
    fprintf(stdout,"Light Intensity: %6d Lux\n",dlux);
}
