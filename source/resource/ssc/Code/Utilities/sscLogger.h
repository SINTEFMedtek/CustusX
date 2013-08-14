// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

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

/**
* \file
*
* The file sscLogger contains c-style logging functions and macros.
*
* \addtogroup sscUtility
*
* @{
*/


int SSC_Logging_Init_Default(const char *busName); ///< Initialize SonoWand logging system and databus connection.
int SSC_Logging_Init(const char *busName, const char* applicationPath); ///< Initialize SonoWand logging system and databus connection.
void SSC_Logging_Done( void ); ///< Properly close down connection to databus.
void SSC_Logging( bool on ); ///< Turn logging on or off.

/** General low-level log function. */
#define SSC_LOG( ... ) SSC_Log(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ ) ///< log using printf-style. Example: SSC_LOG(s%, "Hello world!");
#define SSC_WARNING( ... ) SSC_Warning(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ ); ///< log using printf-style. Example: SSC_WARNING(s%, "Hello world!");
#define SSC_ERROR( ... ) SSC_Error(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ ); ///< log using printf-style. Example: SSC_ERROR(s%, "Hello world!");

/**
  */
#define SSC_ASSERT(STATEMENT)			\
{										\
	if (!(STATEMENT))					\
	{									\
		SSC_ERROR("%s","Assert failure!");\
		exit(1);						\
	}									\
}

/**
* @}
*/

void SSC_Log( const char *file, int line, const char *function, const char *format, ... ) sw__attribute((format (printf, 4, 5))); ///< internal function. use SSC_LOG
void SSC_Error( const char *file, int line, const char *function, const char *format, ...) sw__attribute((format (printf, 4, 5))); ///< internal function. use SSC_ERROR
void SSC_Warning( const char *file, int line, const char *function, const char *format, ...) sw__attribute((format (printf, 4, 5))); ///< internal function. use SSC_WARNING

#ifdef __cplusplus
}
#endif

#endif
