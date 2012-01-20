#ifndef _SSCLOGGER_H_
#define _SSCLOGGER_H_

#ifdef __cplusplus
extern "C"
{
#endif

// standard includes
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stdio.h>

#include "misc.h"

int SSC_Logging_Init_Default(const char *busName);	///< Initialize SonoWand logging system and databus connection.
int SSC_Logging_Init(const char *busName, const char* applicationPath);	///< Initialize SonoWand logging system and databus connection.
void SSC_Logging_Done( void );			///< Properly close down connection to databus.
void SSC_Logging( bool on );		///< Turn logging on or off.
void SSC_Log(     const char *file, int line, const char *function, const char *format, ... ) sw__attribute((format (printf, 4, 5)));
void SSC_Error(   const char *file, int line, const char *function, const char *format, ...)  sw__attribute((format (printf, 4, 5)));
void SSC_Warning( const char *file, int line, const char *function, const char *format, ...)  sw__attribute((format (printf, 4, 5)));

/** General low-level log function. */
#define SSC_LOG( ... ) SSC_Log(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ )
#define SSC_WARNING( ... ) SSC_Warning(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ )
#define SSC_ERROR( ... ) SSC_Error(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ )

#ifdef __cplusplus
}
#endif

/* @} */

#endif
