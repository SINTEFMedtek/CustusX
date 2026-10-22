/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.md for details.
=========================================================Plus=header=end*/

/*! \file ndicapi_socket.h
This file contains the platform-dependent portions of the
NDICAPI C API that talk to the serial port.
*/

#ifndef NDICAPI_SOCKET_H
#define NDICAPI_SOCKET_H

#include "ndicapiExport.h"

/*=====================================================================*/
/*! \defgroup NDISocket NDI Socket Methods
These are low-level methods that provide a platform-independent
interface to the network.
*/

#ifdef __cplusplus
extern "C" {
#endif

/*! \ingroup NDISocket
\typedef NDISocketHandle
The socket handle is a platform-specific type, for which we use the typedef NDISocketHandle.
*/
#ifdef _WIN32
#include <winsock2.h>
typedef SOCKET NDISocketHandle;
#define NDI_INVALID_SOCKET INVALID_SOCKET
#elif defined(unix) || defined(__unix__) || defined(__APPLE__)
typedef int NDISocketHandle;
#define NDI_INVALID_HANDLE -1
#elif defined(macintosh)
typedef long NDISocketHandle;
#define NDI_INVALID_HANDLE -1
#endif

/*! \ingroup NDISocket
Open the specified socket.
A return value of false means that an error occurred.

/return Connected or not
/param hostname URL to connect to
/param port Port to connect to
/param outSocket variable to store the created socket
*/
ndicapiExport bool ndiSocketOpen(const char* hostname, int port, NDISocketHandle& outSocket);

/*! \ingroup NDISocket
Close the socket.
*/
ndicapiExport void ndiSocketClose(NDISocketHandle socket);

/*! \ingroup NDISocket
Flush out the serial I/O buffers. The following options are available:
- NDI_IFLUSH:  discard the contents of the input buffer
- NDI_OFLUSH:  discard the contents of the output buffer
- NDI_IOFLUSH: discard the contents of both buffers.

<p>The return value of this function will be if the call was successful.
*/
ndicapiExport bool ndiSocketFlush(NDISocketHandle socket, int flushtype);

#define  NDI_IFLUSH  0x1
#define  NDI_OFLUSH  0x2
#define  NDI_IOFLUSH 0x3

/*! \ingroup NDISocket
Change the timeout for the socket in milliseconds.
The default is 0.5 seconds, but this might be too long for certain applications.

The return value will be true if the call was successful.
*/
ndicapiExport bool ndiSocketTimeout(NDISocketHandle socket, int milliseconds);

/*! \ingroup NDISocket
Write a stream of 'n' characters from the string 'text' to the socket.
The number of characters actually written is returned.

If the return value is negative, then an IO error occurred.
If the return value is less than 'n', then a timeout error occurred.
*/
ndicapiExport int ndiSocketWrite(NDISocketHandle socket, const char* text, int n);

/*! \ingroup NDISocket
Read characters from the serial port until a carriage return is
received.  A maximum of 'n' characters will be read.  The number
of characters actually read is returned.  The resulting string will
not be null-terminated.

If the return value is negative, then an IO error occurred.
If the return value is zero, then a timeout error occurred.
If the return value is equal to 'n' and the final character
is not a carriage return (i.e. reply[n-1] != '\r'), then the
read was incomplete and there are more characters waiting to
be read.
*/
ndicapiExport int ndiSocketRead(NDISocketHandle socket, char* reply, int numberOfBytesToRead, bool isBinary, int* outErrorCode);

/*! \ingroup NDISocket
Sleep the socket
*/
ndicapiExport bool ndiSocketSleep(NDISocketHandle socket, int milliseconds);

#ifdef __cplusplus
}
#endif

#endif