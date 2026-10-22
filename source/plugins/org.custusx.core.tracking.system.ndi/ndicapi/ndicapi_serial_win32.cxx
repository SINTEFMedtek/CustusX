/*=======================================================================

Copyright (c) 2000-2005 Atamai, Inc.

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
form, must retain the above copyright notice, this license,
the following disclaimer, and any notices that refer to this
license and/or the following disclaimer.

2) Redistribution in binary form must include the above copyright
notice, a copy of this license and the following disclaimer
in the documentation or with other materials provided with the
distribution.

3) Modified copies of the source code must be clearly marked as such,
and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=======================================================================*/


// This file contains the Windows portions of the source code
// that talk to the serial port.  All these methods
// are of the form ndiSerialXX().

#include <windows.h>
#include <winbase.h>
#include <sys/timeb.h>

// USB versions of NDI tracking can communicate at baud rate 921600 but is not defined in WinBase.h
#ifndef CBR_921600
  #define CBR_921600 921600
#endif

#include "ndicapi.h"

//----------------------------------------------------------------------------
// Some static variables to keep track of which ports are open, so that
// we can restore the comm parameters (baud rate etc) when they are closed.
// Restoring the comm parameters is just part of being a good neighbor.

#define NDI_MAX_SAVE_STATE 4
static HANDLE ndi_open_handles[4] = { INVALID_HANDLE_VALUE,
                                      INVALID_HANDLE_VALUE,
                                      INVALID_HANDLE_VALUE,
                                      INVALID_HANDLE_VALUE
                                    };

static COMMTIMEOUTS ndi_save_timeouts[4];
static DCB ndi_save_dcb[4];

//----------------------------------------------------------------------------
ndicapiExport HANDLE ndiSerialOpen(const char* device)
{
  static COMMTIMEOUTS default_ctmo = { MAXDWORD, MAXDWORD,
                                       TIMEOUT_PERIOD,
                                       2,
                                       TIMEOUT_PERIOD
                                     };
  HANDLE serial_port;
  DCB comm_settings;
  int i;

  serial_port = CreateFile(device, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (serial_port == INVALID_HANDLE_VALUE)
  {
    return INVALID_HANDLE_VALUE;
  }

  // Save the serial port state so that it can be restored when
  //   the serial port is closed in ndiSerialClose()
  for (i = 0; i < NDI_MAX_SAVE_STATE; i++)
  {
    if (ndi_open_handles[i] == serial_port || ndi_open_handles[i] == INVALID_HANDLE_VALUE)
    {
      ndi_open_handles[i] = serial_port;
      GetCommTimeouts(serial_port, &ndi_save_timeouts[i]);
      GetCommState(serial_port, &ndi_save_dcb[i]);
      break;
    }
  }

  if (SetupComm(serial_port, 1600, 1600) == FALSE) /* set buffer size */
  {
    if (i < NDI_MAX_SAVE_STATE)   /* if we saved the state, forget the state */
    {
      ndi_open_handles[i] = INVALID_HANDLE_VALUE;
    }
    CloseHandle(serial_port);
    return INVALID_HANDLE_VALUE;
  }

  if (GetCommState(serial_port, &comm_settings) == FALSE)
  {
    if (i < NDI_MAX_SAVE_STATE)   /* if we saved the state, forget the state */
    {
      ndi_open_handles[i] = INVALID_HANDLE_VALUE;
    }
    CloseHandle(serial_port);
    return INVALID_HANDLE_VALUE;
  }

  comm_settings.fOutX = FALSE;             /* no S/W handshake */
  comm_settings.fInX = FALSE;
  comm_settings.fAbortOnError = FALSE;     /* don't need to clear errors */
  comm_settings.fOutxDsrFlow = FALSE;      /* no modem-style flow stuff*/
  comm_settings.fDtrControl = DTR_CONTROL_ENABLE;

  if (SetCommState(serial_port, &comm_settings) == FALSE)
  {
    if (i < NDI_MAX_SAVE_STATE)   /* if we saved the state, forget the state */
    {
      ndi_open_handles[i] = INVALID_HANDLE_VALUE;
    }
    CloseHandle(serial_port);
    return INVALID_HANDLE_VALUE;
  }

  if (SetCommTimeouts(serial_port, &default_ctmo) == FALSE)
  {
    SetCommState(serial_port, &comm_settings);
    if (i < NDI_MAX_SAVE_STATE)   /* if we saved the state, forget the state */
    {
      SetCommState(serial_port, &ndi_save_dcb[i]);
      ndi_open_handles[i] = INVALID_HANDLE_VALUE;
    }
    CloseHandle(serial_port);
    return INVALID_HANDLE_VALUE;
  }

  return serial_port;
}

//----------------------------------------------------------------------------
ndicapiExport void ndiSerialClose(HANDLE serial_port)
{
  int i;

  /* restore the comm port state to from before it was opened */
  for (i = 0; i < NDI_MAX_SAVE_STATE; i++)
  {
    if (ndi_open_handles[i] == serial_port &&
        ndi_open_handles[i] != INVALID_HANDLE_VALUE)
    {
      SetCommTimeouts(serial_port, &ndi_save_timeouts[i]);
      SetCommState(serial_port, &ndi_save_dcb[i]);
      ndi_open_handles[i] = INVALID_HANDLE_VALUE;
      break;
    }
  }

  CloseHandle(serial_port);
}

//----------------------------------------------------------------------------
ndicapiExport int ndiSerialCheckDSR(HANDLE serial_port)
{
  DWORD bits;
  /* get the bits to see if DSR is set (i.e. if device is connected) */
  GetCommModemStatus(serial_port, &bits);
  return ((bits & MS_DSR_ON) != 0);
}

//----------------------------------------------------------------------------
ndicapiExport int ndiSerialBreak(HANDLE serial_port)
{
  DWORD dumb;

  ClearCommError(serial_port, &dumb, NULL);     /* clear error */
  PurgeComm(serial_port, PURGE_TXCLEAR | PURGE_RXCLEAR); /* clear buffers */

  SetCommBreak(serial_port);
  Sleep(300);                            /* hold break for 0.3 seconds */
  ClearCommBreak(serial_port);

  return 0;
}

//----------------------------------------------------------------------------
ndicapiExport int ndiSerialFlush(HANDLE serial_port, int buffers)
{
  DWORD dumb;
  DWORD flushtype = PURGE_TXCLEAR | PURGE_RXCLEAR;

  if (buffers == NDI_IFLUSH)
  {
    flushtype = PURGE_RXCLEAR;
  }
  else if (buffers == NDI_OFLUSH)
  {
    flushtype = PURGE_TXCLEAR;
  }

  ClearCommError(serial_port, &dumb, NULL);     /* clear error */
  PurgeComm(serial_port, flushtype);            /* clear buffers */

  return 0;
}

//----------------------------------------------------------------------------
ndicapiExport int ndiSerialComm(HANDLE serial_port, int baud, const char* mode,
                                int handshake)
{
  DCB comm_settings;
  int newbaud;
  switch (baud)
  {
    case 9600:
      newbaud = CBR_9600;
      break;
    case 14400:
      newbaud = CBR_14400;
      break;
    case 19200:
      newbaud = CBR_19200;
      break;
    case 38400:
      newbaud = CBR_38400;
      break;
    case 57600:
      newbaud = CBR_57600;
      break;
    case 115200:
      newbaud = CBR_115200;
      break;
    case 921600:
      newbaud = CBR_921600;
      break;
    case 1228739:
      newbaud = CBR_19200; //19.2k is aliased to 1.2Mbit in the Window's version of the NDI USB virtual com port driver
      break;
    case 230400:
      newbaud = 230400;
      break;
    default:
      return -1;
  }

  GetCommState(serial_port, &comm_settings);

  comm_settings.BaudRate = newbaud;     /* speed */

  if (handshake)                        /* set handshaking */
  {
    comm_settings.fOutxCtsFlow = TRUE;       /* on */
    comm_settings.fRtsControl = RTS_CONTROL_HANDSHAKE;
  }
  else
  {
    comm_settings.fOutxCtsFlow = FALSE;       /* off */
    comm_settings.fRtsControl = RTS_CONTROL_DISABLE;
  }

  if (mode[0] == '8')                   /* data bits */
  {
    comm_settings.ByteSize = 8;
  }
  else if (mode[0] == '7')
  {
    comm_settings.ByteSize = 7;
  }
  else
  {
    return -1;
  }

  if (mode[1] == 'N')                   /* set parity */
  {
    comm_settings.Parity = NOPARITY;
  }
  else if (mode[1] == 'O')
  {
    comm_settings.Parity = ODDPARITY;
  }
  else if (mode[1] == 'E')
  {
    comm_settings.Parity = EVENPARITY;
  }
  else
  {
    return -1;
  }

  if (mode[2] == '1')                    /* set stop bits */
  {
    comm_settings.StopBits = ONESTOPBIT;
  }
  else if (mode[2] == '2')
  {
    comm_settings.StopBits = TWOSTOPBITS;
  }
  else
  {
    return -1;
  }

  int result = SetCommState(serial_port, &comm_settings);

  return result > 0 ? 0 : -1;
}

//----------------------------------------------------------------------------
ndicapiExport int ndiSerialTimeout(HANDLE serial_port, int milliseconds)
{
  COMMTIMEOUTS ctmo;

  if (GetCommTimeouts(serial_port, &ctmo) == FALSE)
  {
    return -1;
  }

  ctmo.ReadIntervalTimeout = MAXDWORD;
  ctmo.ReadTotalTimeoutMultiplier = MAXDWORD;
  ctmo.ReadTotalTimeoutConstant = milliseconds;
  ctmo.WriteTotalTimeoutConstant = milliseconds;

  if (SetCommTimeouts(serial_port, &ctmo) == FALSE)
  {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
ndicapiExport int ndiSerialWrite(HANDLE serial_port, const char* text, int n)
{
  DWORD m, dumb;
  int i = 0;

  while (n > 0)
  {
    if (WriteFile(serial_port, &text[i], n, &m, NULL) == FALSE)
    {
      if (GetLastError() == ERROR_OPERATION_ABORTED)  /* system canceled us */
      {
        ClearCommError(serial_port, &dumb, NULL); /* so clear error and retry */
      }
      else
      {
        return -1;  /* IO error occurred */
      }
    }
    else if (m == 0)   /* no characters written, must have timed out */
    {
      return i;
    }

    n -= m;  /* n is number of chars left to write */
    i += m;  /* i is the number of chars written */
  }

  return i;  /* return the number of characters written */
}

//----------------------------------------------------------------------------
ndicapiExport int ndiSerialRead(HANDLE serial_port, char* reply, int numberOfBytesToRead, bool isBinary, int* errorCode)
{
  int totalNumberOfBytesRead = 0;
  int totalNumberOfBytesToRead = numberOfBytesToRead;
  DWORD numberOfBytesRead;
  bool binarySizeCalculated = false;

  do
  {
    if (ReadFile(serial_port, &reply[totalNumberOfBytesRead], numberOfBytesToRead, &numberOfBytesRead, NULL) == FALSE)
    {
      if (GetLastError() == ERROR_OPERATION_ABORTED)  /* canceled */
      {
        DWORD dummyVariable;
        ClearCommError(serial_port, &dummyVariable, NULL); /* so clear error and retry */
      }
      else
      {
        return -1;  /* IO error occurred */
      }
    }
    else if (numberOfBytesRead == 0)   /* no characters read, must have timed out */
    {
      return 0;
    }

    totalNumberOfBytesRead += numberOfBytesRead;
    if (!isBinary && reply[totalNumberOfBytesRead - 1] == '\r'       /* done when carriage return received (ASCII) or when ERROR... received (binary)*/
        || isBinary && strncmp(reply, "ERROR", 5) == 0 && reply[totalNumberOfBytesRead - 1] == '\r')
    {
      if (strncmp(reply, "ERROR", 5) == 0)
      {
        unsigned long err = ndiHexToUnsignedLong(&reply[5], 2);
        if (errorCode != NULL)
        {
          *errorCode = static_cast<int>(err);
        }
      }
      break;
    }

    if (isBinary && !binarySizeCalculated && reply[0] == (char)0xc4 && reply[1] == (char)0xa5)
    {
      // recalculate n based on the reply length (reported from ndi device) and the amount of data received so far
      unsigned short size = ((unsigned char)reply[2] | (unsigned char)reply[3] << 8) + 8; // 8 bytes -> 2 for Start Sequence (a5c4), 2 for reply length, 2 for header CRC, 2 for CRC16
      totalNumberOfBytesToRead = size;
    }
  }
  while (totalNumberOfBytesRead != totalNumberOfBytesToRead);

  return totalNumberOfBytesRead;
}

//----------------------------------------------------------------------------
ndicapiExport int ndiSerialSleep(HANDLE serial_port, int milliseconds)
{
  Sleep(milliseconds);

  return 0;
}