#ifndef SPTGLGMAIN_H
#define SPTGLGMAIN_H
/** \file sptglgmain.h
 * \brief Constants, function prototypes for glg main
 * \author Alexander Guglenko
 * \date 18Feb2020
*/
//includes
#include "GlgApi.h"

// CONSTANTS
#define UPDATE_INTERVAL 5000	// Initially 5 second update
#define STON 1
#define STOFF 0

///\cond INTERNAL
// Function Prototypes
void Input(GlgObject viewport,GlgAnyType client_data,GlgAnyType call_data);
void UpdateControls(GlgAnyType data,GlgIntervalID * id);

///\endcond

#endif
