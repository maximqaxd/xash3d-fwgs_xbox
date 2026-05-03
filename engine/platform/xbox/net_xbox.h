/*
net_xbox.h - Xbox (nxdk/lwip) compatibility
Copyright (C) 2026 maximqad

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/
#ifndef NET_XBOX_H
#define NET_XBOX_H

#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/inet.h>
#include <errno.h>
#include <string.h>
#include <lwip/netif.h>
#include <nxdk/net.h>

#ifndef IPV6_MULTICAST_LOOP
#define IPV6_MULTICAST_LOOP 19
#endif

static inline int gethostname( char *name, size_t len )
{
	if( netif_default != NULL )
	{
		ip4addr_ntoa_r( netif_ip4_addr( netif_default ), name, (int)len );
	}
	else
	{
		strncpy( name, "127.0.0.1", len );
	}

	if( len > 0 )
		name[len - 1] = '\0';

	return 0;
}

#define WSAGetLastError()  errno
#define WSAEINTR           EINTR
#define WSAEBADF           EBADF
#define WSAEACCES          EACCES
#define WSAEFAULT          EFAULT
#define WSAEINVAL          EINVAL
#define WSAEMFILE          EMFILE
#define WSAEWOULDBLOCK     EWOULDBLOCK
#define WSAEINPROGRESS     EINPROGRESS
#define WSAEALREADY        EALREADY
#define WSAENOTSOCK        ENOTSOCK
#define WSAEDESTADDRREQ    EDESTADDRREQ
#define WSAEMSGSIZE        EMSGSIZE
#define WSAEPROTOTYPE      EPROTOTYPE
#define WSAENOPROTOOPT     ENOPROTOOPT
#define WSAEPROTONOSUPPORT EPROTONOSUPPORT
#define WSAESOCKTNOSUPPORT ESOCKTNOSUPPORT
#define WSAEOPNOTSUPP      EOPNOTSUPP
#define WSAEPFNOSUPPORT    EPFNOSUPPORT
#define WSAEAFNOSUPPORT    EAFNOSUPPORT
#define WSAEADDRINUSE      EADDRINUSE
#define WSAEADDRNOTAVAIL   EADDRNOTAVAIL
#define WSAENETDOWN        ENETDOWN
#define WSAENETUNREACH     ENETUNREACH
#define WSAENETRESET       ENETRESET
#define WSAECONNABORTED    ECONNABORTED
#define WSAECONNRESET      ECONNRESET
#define WSAENOBUFS         ENOBUFS
#define WSAEISCONN         EISCONN
#define WSAENOTCONN        ENOTCONN
#define WSAESHUTDOWN       ESHUTDOWN
#define WSAETOOMANYREFS    ETOOMANYREFS
#define WSAETIMEDOUT       ETIMEDOUT
#define WSAECONNREFUSED    ECONNREFUSED
#define WSAELOOP           ELOOP
#define WSAENAMETOOLONG    ENAMETOOLONG
#define WSAEHOSTDOWN       EHOSTDOWN

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

#ifndef closesocket
#define closesocket lwip_close
#endif

#ifndef ioctlsocket
#define ioctlsocket lwip_ioctl
#endif

#define SOCKET int
typedef int WSAsize_t;

#endif // NET_XBOX_H
