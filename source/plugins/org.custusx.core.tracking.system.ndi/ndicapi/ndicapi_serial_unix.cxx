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


// This file contains the platform-dependent portions of the source code
// that talk to the serial port.  All these methods
// are of the form ndiSerialXX().

#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>

//----------------------------------------------------------------------------
// Some static variables to keep track of which ports are open, so that
// we can restore the comm parameters (baud rate etc) when they are closed.
// Restoring the comm parameters is just part of being a good neighbor.

#define NDI_MAX_SAVE_STATE 4
static int ndi_open_handles[4] = { -1, -1, -1, -1 };

static struct termios ndi_save_termios[4];

//----------------------------------------------------------------------------
ndicapiExport int ndiSerialOpen(const char* device)
{
  static struct flock fl = { F_WRLCK, 0, 0, 0 }; /* for file locking */
  static struct flock fu = { F_UNLCK, 0, 0, 0 }; /* for file unlocking */
  int serial_port;
  struct termios t;
  int i;

  /* port is readable/writable and is (for now) non-blocking */
  serial_port = open(device, O_RDWR | O_NOCTTY | O_NDELAY);

  if (serial_port == -1)
  {
    return -1;             /* bail out on error */
  }

  /* restore blocking now that the port is open (we just didn't want */
  /* the port to block while we were trying to open it) */
  fcntl(serial_port, F_SETFL, 0);

  /* get exclusive lock on the serial port */
  /* on many unices, this has no effect for device files */
  if (fcntl(serial_port, F_SETLK, &fl))
  {
    close(serial_port);
    return -1;
  }

  /* get I/O information */
  if (tcgetattr(serial_port, &t) == -1)
  {
    fcntl(serial_port, F_SETLK, &fu);
    close(serial_port);
    return -1;
  }

  /* save the serial port state so that it can be restored when
     the serial port is closed in ndiSerialClose() */
  for (i = 0; i < NDI_MAX_SAVE_STATE; i++)
  {
    if (ndi_open_handles[i] == serial_port || ndi_open_handles[i] == -1)
    {
      ndi_open_handles[i] = serial_port;
      tcgetattr(serial_port, &ndi_save_termios[i]);
      break;
    }
  }

  /* clear everything specific to terminals */
  t.c_lflag = 0;
  t.c_iflag = 0;
  t.c_oflag = 0;

  t.c_cc[VMIN] = 0;                    /* use constant, not interval timeout */
  t.c_cc[VTIME] = TIMEOUT_PERIOD / 100; /* wait for 5 secs max */

  if (tcsetattr(serial_port, TCSANOW, &t) == -1) /* set I/O information */
  {
    if (i < NDI_MAX_SAVE_STATE)   /* if we saved the state, forget the state */
    {
      ndi_open_handles[i] = -1;
    }
    fcntl(serial_port, F_SETLK, &fu);
    close(serial_port);
    return -1;
  }

  tcflush(serial_port, TCIOFLUSH);        /* flush the buffers for good luck */

  return serial_port;
}

//----------------------------------------------------------------------------
ndicapiExport void ndiSerialClose(int serial_port)
{
  static struct flock fu = { F_UNLCK, 0, 0, 0 }; /* for file unlocking */
  int i;

  /* restore the comm port state to from before it was opened */
  for (i = 0; i < NDI_MAX_SAVE_STATE; i++)
  {
    if (ndi_open_handles[i] == serial_port && ndi_open_handles[i] != -1)
    {
      tcsetattr(serial_port, TCSANOW, &ndi_save_termios[i]);
      ndi_open_handles[i] = -1;
      break;
    }
  }

  /* release our lock on the serial port */
  fcntl(serial_port, F_SETLK, &fu);

  close(serial_port);
}

//----------------------------------------------------------------------------
ndicapiExport int ndiSerialCheckDSR(int serial_port)
{
#if defined(linux) || defined(__linux__)
  int bits;
  /* get the bits to see if DSR is set (i.e. if device is connected) */
  if (ioctl(serial_port, TIOCMGET, &bits) >= 0)
  {
    return ((bits & TIOCM_DSR) != 0);
  }
#endif
  /* if called failed for any reason, return success for robustness */
  return 1;
}

//----------------------------------------------------------------------------
ndicapiExport int ndiSerialBreak(int serial_port)
{
  tcflush(serial_port, TCIOFLUSH);    /* clear input/output buffers */
  tcsendbreak(serial_port, 0);        /* send the break */

  return 0;
}

//----------------------------------------------------------------------------
ndicapiExport int ndiSerialFlush(int serial_port, int buffers)
{
  int flushtype = TCIOFLUSH;

  if (buffers == NDI_IFLUSH)
  {
    flushtype = TCIFLUSH;
  }
  else if (buffers == NDI_OFLUSH)
  {
    flushtype = TCOFLUSH;
  }

  tcflush(serial_port, flushtype);    /* clear input/output buffers */

  return 0;
}

//----------------------------------------------------------------------------
ndicapiExport int ndiSerialComm(int serial_port, int baud, const char mode[4], int handshake)
{
  struct termios t;
  int newbaud;

#if defined(linux) || defined(__linux__)
  switch (baud)
  {
    case 9600:
      newbaud = B9600;
      break;
    case 14400:
      return -1;
    case 19200:
      newbaud = B19200;
      break;
    case 38400:
      newbaud = B38400;
      break;
    case 57600:
      newbaud = B57600;
      break;
    case 115200:
      newbaud = B115200;
      break;
    case 230400:
      newbaud = B230400;
      break;
    default:
      return -1;
  }
#elif defined(sgi) && defined(__NEW_MAX_BAUD)
  switch (baud)
  {
    case 9600:
      newbaud = 9600;
      break;
    case 14400:
      return -1;
    case 19200:
      newbaud = 19200;
      break;
    case 38400:
      newbaud = 38400;
      break;
    case 57600:
      newbaud = 57600;
      break;
    case 115200:
      newbaud = 115200;
      break;
    default:
      return -1;
  }
#else
  switch (baud)
  {
    case 9600:
      newbaud = B9600;
      break;
    case 14400:
      return -1;
    case 19200:
      newbaud = B19200;
      break;
    case 38400:
      newbaud = B38400;
      break;
    case 57600:
      return -1;
    case 115200:
      return -1;
    default:
      return -1;
  }
#endif

  tcgetattr(serial_port, &t);         /* get I/O information */
  t.c_cflag &= ~CSIZE;                /* clear flags */

#if defined(linux) || defined(__linux__)
  t.c_cflag &= ~CBAUD;
  t.c_cflag |= newbaud;                /* set baud rate */
#elif defined(sgi) && defined(__NEW_MAX_BAUD)
  t.c_ospeed = newbaud;
#else
  t.c_cflag &= ~CBAUD;
  t.c_cflag |= newbaud;                /* set baud rate */
#endif

  if (mode[0] == '8')                   /* set data bits */
  {
    t.c_cflag |= CS8;
  }
  else if (mode[0] == '7')
  {
    t.c_cflag |= CS7;
  }
  else
  {
    return -1;
  }

  if (mode[1] == 'N')                   /* set parity */
  {
    t.c_cflag &= ~PARENB;
    t.c_cflag &= ~PARODD;
  }
  else if (mode[1] == 'O')
  {
    t.c_cflag |= PARENB;
    t.c_cflag |= PARODD;
  }
  else if (mode[1] == 'E')
  {
    t.c_cflag |= PARENB;
    t.c_cflag &= ~PARODD;
  }
  else
  {
    return -1;
  }

  if (mode[2] == '1')                    /* set stop bits */
  {
    t.c_cflag &= ~CSTOPB;
  }
  else if (mode[2] == '2')
  {
    t.c_cflag |= CSTOPB;
  }
  else
  {
    return -1;
  }

  if (handshake)
  {
#ifdef sgi
    t.c_cflag |= CNEW_RTSCTS;       /* enable hardware handshake */
#else
    t.c_cflag |= CRTSCTS;
#endif
  }
  else
  {
#ifdef sgi
    t.c_cflag &= ~CNEW_RTSCTS;          /* turn off hardware handshake */
#else
    t.c_cflag &= ~CRTSCTS;
#endif
  }

  return tcsetattr(serial_port, TCSADRAIN, &t); /* set I/O information */
}

//----------------------------------------------------------------------------
ndicapiExport int ndiSerialTimeout(int serial_port, int milliseconds)
{
  struct termios t;

  if (tcgetattr(serial_port, &t) == -1)
  {
    return -1;
  }

  t.c_cc[VMIN] = 0;                  /* use constant, not interval timout */
  t.c_cc[VTIME] = milliseconds / 100; /* wait time is in 10ths of a second */

  if (tcsetattr(serial_port, TCSANOW, &t) == -1)
  {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
ndicapiExport int ndiSerialWrite(int serial_port, const char* text, int n)
{
  int i = 0;
  int m;

  while (n > 0)
  {
    if ((m = write(serial_port, &text[i], n)) == -1)
    {
      if (errno == EAGAIN)   /* system canceled us, retry */
      {
        m = 0;
      }
      else
      {
        return -1;  /* IO error occurred */
      }
    }

    n -= m;  /* n is number of chars left to write */
    i += m;  /* i is the number of chars written */
  }

  return i;  /* return the number of characters written */
}

//----------------------------------------------------------------------------
ndicapiExport int ndiSerialRead(int serial_port, char* reply, int numberOfBytesToRead, bool isBinary, int* errorCode)
{
  int totalNumberOfBytesRead = 0;
  int totalNumberOfBytesToRead = numberOfBytesToRead;
  int numberOfBytesRead;
  bool binarySizeCalculated = false;

  do
  {
    if ((numberOfBytesRead = read(serial_port, &reply[totalNumberOfBytesRead], numberOfBytesToRead)) == -1)
    {
      if (errno == EAGAIN) /* canceled, so retry */
      {
        numberOfBytesRead = 0;
      }
      else
      {
        return -1; /* IO error occurred */
      }
    }
    else if (numberOfBytesRead == 0)   /* no characters read, must have timed out */
    {
      return 0;
    }

    totalNumberOfBytesRead += numberOfBytesRead;
    if ((!isBinary && reply[totalNumberOfBytesRead - 1] == '\r')      /* done when carriage return received (ASCII) or when ERROR... received (binary)*/
        || (isBinary && strncmp(reply, "ERROR", 5) == 0 && reply[totalNumberOfBytesRead - 1] == '\r'))
    {
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
ndicapiExport int ndiSerialSleep(int serial_port, int milliseconds)
{
#ifdef USE_NANOSLEEP
  struct timespec sleep_time, dummy;
  sleep_time.tv_sec = milliseconds / 1000;
  sleep_time.tv_nsec = (milliseconds - sleep_time.tv_sec * 1000) * 1000000;
  nanosleep(&sleep_time, &dummy);
#else /* use usleep instead */
  /* some unices like IRIX can't usleep for more than 1 second,
     so break usleep into 500 millisecond chunks */
  while (milliseconds > 500)
  {
    usleep(500000);
    milliseconds -= 500;
  }
  usleep(milliseconds * 1000);
#endif

  return 0;
}
