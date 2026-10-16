/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.md for details.
=========================================================Plus=header=end*/

// This file contains the platform-dependent portions of the source code
// that talk to the socket.  All these methods
// are of the form ndiSocketXX().

#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ndicapi_socket.h"

// time out period in milliseconds
#define TIMEOUT_PERIOD_MS 500

#ifdef _WIN32
  #include "ndicapi_socket_win32.cxx"
#elif defined(unix) || defined(__unix__) || defined(__linux__)
  #include "ndicapi_socket_unix.cxx"
#elif defined(__APPLE__)
  #include "ndicapi_socket_apple.cxx"
#endif