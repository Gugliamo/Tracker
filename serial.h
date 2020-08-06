#ifndef SERIAL_H
#define SERIAL_H
/** \file serial.h
 * \brief Constants, structures, function prototypes for serial functions
 * \author Alexander Guglenko
 * \date 14JAN2020
*/
#include <inttypes.h>

#ifndef PORTNAME
#define PORTNAME "/dev/ttyS0"
#endif

///\cond INTERNAL
// Function Prototypes
void serial_init(void);
void serial_config(void);
void serial_println(const char *, int);
void serial_readln(char *, int);
void serial_close(void);
///\endcond
#endif
