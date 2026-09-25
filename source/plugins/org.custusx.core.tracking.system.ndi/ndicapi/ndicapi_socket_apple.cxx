/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.md for details.
=========================================================Plus=header=end*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>

//----------------------------------------------------------------------------
ndicapiExport bool ndiSocketOpen(const char* hostname, int port, NDISocketHandle& outSocket)
{
  NDISocketHandle sock = socket(AF_INET, SOCK_STREAM, 0);

  // Eliminate windows 0.2 second delay sending (buffering) data.
  int on = 1;
  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on)))
  {
    return false;
  }

  struct hostent* hp;
  hp = gethostbyname(hostname);
  if (!hp)
  {
    unsigned long addr = inet_addr(hostname);
    hp = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
  }

  if (!hp)
  {
    return false;
  }

  struct sockaddr_in name;
  name.sin_family = AF_INET;
  memcpy(&name.sin_addr, hp->h_addr, hp->h_length);
  name.sin_port = htons(port);

  int r = connect(sock, reinterpret_cast<sockaddr*>(&name), sizeof(name));

  if (r < 0)
  {
    shutdown(sock, 2);
    close(sock);
    return false;
  }

  outSocket = sock;
  return true;
}

//----------------------------------------------------------------------------
ndicapiExport void ndiSocketClose(NDISocketHandle socket)
{
  shutdown(socket, 2);
  close(socket);
}

//----------------------------------------------------------------------------
ndicapiExport bool ndiSocketFlush(NDISocketHandle socket, int flushtype)
{
  return true;
}

//----------------------------------------------------------------------------
ndicapiExport bool ndiSocketTimeout(NDISocketHandle socket, int timeoutMs)
{
  if (timeoutMs > 0)
  {
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char*) & (timeoutMs), sizeof(timeoutMs));
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
ndicapiExport int ndiSocketWrite(NDISocketHandle socket, const char* data, int length)
{
  if (length == 0)
  {
    // nothing to send.
    return 0;
  }

  const char* buffer = reinterpret_cast<const char*>(data);
  int total = 0;
  do
  {
    int flags;

    // On unix boxes if the client disconnects and the server attempts
    // to send data through the socket then the application crashes
    // due to SIGPIPE signal. Disable the signal to prevent crash.
#if defined(MSG_NOSIGNAL)
    flags = MSG_NOSIGNAL;
#else
#if defined(SO_NOSIGPIPE) // Mac OS X
    int set = 1;
    setsockopt(socket, SOL_SOCKET, SO_NOSIGPIPE, (void*)&set, sizeof(int));
#endif
    flags = 0;
#endif
    int n = send(socket, buffer + total, length - total, flags);
    if (n < 0)
    {
      return -1;
    }
    total += n;
  }
  while (total < length);
  return total;
}

//----------------------------------------------------------------------------
ndicapiExport int ndiSocketRead(NDISocketHandle socket, char* reply, int numberOfBytesToRead, bool isBinary, int* outErrorCode)
{
  int totalNumberOfBytesRead = 0;
  int totalNumberOfBytesToRead = numberOfBytesToRead;
  int numberOfBytesRead;
  bool binarySizeCalculated = false;

  do
  {
    numberOfBytesRead = recv(socket, reply + totalNumberOfBytesRead, numberOfBytesToRead, 0);

    if (numberOfBytesRead < 1)
    {
      return -1;
    }

    totalNumberOfBytesRead += numberOfBytesRead;
    if (!isBinary && reply[totalNumberOfBytesRead - 1] == '\r'       /* done when carriage return received (ASCII) or when ERROR... received (binary)*/
        || isBinary && strncmp(reply, "ERROR", 5) == 0 && reply[totalNumberOfBytesRead - 1] == '\r')
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
ndicapiExport bool ndiSocketSleep(NDISocketHandle socket, int milliseconds)
{
#if _POSIX_C_SOURCE >= 199309L
  struct timespec ts;
  ts.tv_sec = milliseconds / 1000;
  ts.tv_nsec = (milliseconds % 1000) * 1000000;
  nanosleep(&ts, NULL);
#else
  usleep(milliseconds * 1000);
#endif
  return true;
}
