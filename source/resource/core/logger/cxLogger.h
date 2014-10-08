/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


#ifndef CXLOGGER_H_
#define CXLOGGER_H_

#include "cxResourceExport.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
* \file
*
* The file sscLogger contains c-style logging functions and macros.
*
 * \addtogroup cx_resource_core_logger
*
* @{
*/


//int SSC_Logging_Init_Default(const char *busName); ///< Initialize SonoWand logging system and databus connection.
//int SSC_Logging_Init(const char *busName, const char* applicationPath); ///< Initialize SonoWand logging system and databus connection.
//void SSC_Logging_Done( void ); ///< Properly close down connection to databus.
//void SSC_Logging( bool on ); ///< Turn logging on or off.

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
	}									\
}

/**
* @}
*/

cxResource_EXPORT void SSC_Log( const char *file, int line, const char *function, const char *format, ... ); ///< internal function. use SSC_LOG
cxResource_EXPORT void SSC_Error( const char *file, int line, const char *function, const char *format, ...); ///< internal function. use SSC_ERROR
cxResource_EXPORT void SSC_Warning( const char *file, int line, const char *function, const char *format, ...); ///< internal function. use SSC_WARNING

#ifdef __cplusplus
}
#endif

#endif
