/* We have slightly modified this file to adapt it to the needs of the Lava project */

/*  ----------------------------------------------------------------<Prolog>-
    Name:       sflsock.h
    Title:      TCP/IP, UDP/IP socket functions
    Package:    Standard Function Library (SFL)

    Written:    1996/02/03  iMatix SFL project team <sfl@imatix.com>
    Revised:    1999/09/20

    Synopsis:   Provides functions to create, read, and write TCP and UDP
                sockets.  Encapsulates system dependencies.  Tested under
                MS Winsock, UNIX (Linux, AIX, SunOs), OpenVMS.  Some of the
                code in this module was based on the book "Internetworking
                With TCP/IP Volume III: Client-Server Programming And
                Applications BSD Socket Version" by Douglas E. Comer and
                David L. Stevens, published 1993 by Prentice-Hall Inc.
                ISBN 0-13-020272-X.  Defines sock_t which you should use
                for all sockets.  If you need to call a native socket
                function, use a (SOCKET) cast on the sock_t handle.

    Copyright:  Copyright (c) 1996-2000 iMatix Corporation
    License:    This is free software; you can redistribute it and/or modify
                it under the terms of the SFL License Agreement as provided
                in the file LICENSE.TXT.  This software is distributed in
                the hope that it will be useful, but without any warranty.
 ------------------------------------------------------------------</Prolog>-*/

#ifndef SFLSOCK_INCLUDED               /*  Allow multiple inclusions        */
#define SFLSOCK_INCLUDED


#ifdef WIN32
#ifdef SFL_EXPORT
#define SFL_DLL __declspec( dllexport )
#else
#define SFL_DLL __declspec( dllimport )
#endif
#else
#define SFL_DLL
#endif

/*---------------------------------------------------------------------------
 *  Non-portable definitions - currently for Winsocks
 *  winsock.h is included in the prelude.h file.
 */

#if (defined (__WINDOWS__))
#   define sockerrno     winsock_last_error ()
#   undef INVALID_SOCKET                /*  MSVC defines it in winsock.h     */
#   if (defined (WIN32) && defined (_MSC_VER))
#       pragma comment (lib, "wsock32")
#   endif
#else
#   define sockerrno     errno          /*  Use sockerrno for error number   */
#   define SOCKET_ERROR  -1             /*  Error on socket function         */
    typedef int SOCKET;                 /*  Use (SOCKET) for system calls    */
#endif


/*---------------------------------------------------------------------------
 *  Standard TCP/IP constants
 */

#define DNS_PORT         53             /*  Domain Name server port          */


/*---------------------------------------------------------------------------
 *  Define the socket type 'sock_t' and various types and constants.
 */

typedef qbyte sock_t;                   /*  Use sock_t for all sockets       */

/*  Argument sizes are "int", on most systems, even though negative values
 *  do not make sense.  GNU libc 2 (aka Linux libc6) changed argument sizes
 *  to be "socklen_t", ie unsigned int, at least as of GNU libc 2.0.7.
 *  OpenVMS Dec C defines these as "unsigned int".
 */

#if !defined(__socklen_t_defined) && !defined(__SOCKLEN_T_DEFINED)
typedef int socklen_t;
#define __socklen_t_defined
#define __SOCKLEN_T_DEFINED
#endif

#if (defined (__GLIBC__) && (__GLIBC__ > 1))
typedef socklen_t  argsize_t;           /*  GNU libc: size arg for sock func */

#elif (defined (__VMS__) && !defined (vaxc))
typedef unsigned int argsize_t;         /*  OpenVMS: size arg for sock func  */

#else
typedef int argsize_t;                  /*  Traditional: size for sock func  */
#endif

#define INVALID_SOCKET   (sock_t) -1    /*  Invalid socket handle            */
#define SOCKET_LOOPBACK  0x7f000001L    /*  Loopback address 127.0.0.1       */


/*---------------------------------------------------------------------------
 *  Fake socket definitions
 *
 *  If the system does not support sockets, we'll fake it so that the code
 *  still works.  Under crippleware OS's this will work so that all devices
 *  are always ready for I/O.  Most of this code is adapted from the Linux
 *  time.h file.  We also define some useful structures.
 */

#if (!defined (DOES_SOCKETS))
#   define INADDR_ANY       0

    /*  If FAKE_SOCKETS is set, sflsock will fake basic socket i/o           */
#   define FAKE_SOCKETS     1

    /*  Number of descriptors that can fit in an `fd_set'.                   */
#   define FD_SETSIZE       256

    /*  It's easier to assume 8-bit bytes than to get CHAR_BIT.              */
#   define NFDBITS          (sizeof (unsigned long int) * 8)
#   define __FDELT(d)       ((d) / NFDBITS)
#   define __FDMASK(d)      (1 << ((d) % NFDBITS))
#   if (!defined (__DJGPP__))
#   define FD_ZERO(set)     ((void) memset((void *) (set), 0, sizeof(fd_set)))
#   define FD_SET(d,set)    ((set)->__bits[__FDELT(d)] |=  __FDMASK(d))
#   define FD_CLR(d,set)    ((set)->__bits[__FDELT(d)] &= ~__FDMASK(d))
#   define FD_ISSET(d,set)  ((set)->__bits[__FDELT(d)] &   __FDMASK(d))
#   endif

    /*  Fake a select() function that just returns 1                         */
#   define select(n,rf,wf,xf,t)         1

    /*  Fake the inet conversion functions                                   */
#   define inet_addr(x)     1
#   define inet_ntoa(x)     "127.0.0.1"

#if (!defined (__DJGPP__))
    /*  Define the fd_set structure for select()                             */
    typedef struct {
        qbyte __bits [(FD_SETSIZE + (NFDBITS - 1)) / NFDBITS];
    } fd_set;

    /*  Define the timeval structure for select()                            */
    struct timeval {
        long  tv_sec;                   /*  Seconds                          */
        long  tv_usec;                  /*  Microseconds                     */
    };
#endif

    /*  Define the generic socket address structure                          */
    struct sockaddr {
        dbyte sa_family;                /*  Type of address                  */
        char  sa_data [14];             /*  Value of address (filename)      */
    };

    /*  Define the internet socket address structure                         */
    struct in_addr {
        qbyte s_addr;
    };
    struct sockaddr_in {
        dbyte          sin_family;      /*  Type of address                  */
        dbyte          sin_port;        /*  Protocol port number             */
        struct in_addr sin_addr;        /*  IP address                       */
        char           sin_zero [8];    /*  Unused - should be zero          */
    };
#   define AF_INET          0

    /*  Define net-to-host conversion macros                                 */
#   if (defined (__MSDOS__) && !defined (__DJGPP__))
#       define ntohs(x) (((x) & 0xff00U) >> 8) +                             \
                        (((x) & 0x00ffU) << 8)
#       define ntohl(x) (((x) & 0xff000000UL) >> 24) +                       \
                        (((x) & 0x00ff0000UL) >> 8 ) +                       \
                        (((x) & 0x0000ff00UL) << 8 ) +                       \
                        (((x) & 0x000000ffUL) << 24)
#       define htons(x) ntohs(x)
#       define htonl(x) ntohl(x)
#   else
#       define ntohs(x) (x)
#       define ntohl(x) (x)
#       define htons(x) (x)
#       define htonl(x) (x)
#   endif                               /*  msdos                            */
#endif                                  /*  Define fake sockets              */


/*---------------------------------------------------------------------------
 *  Definitions for DJGPP and other compilers which don't have net support
 *  in their header libraries. Expand as required.
 */

#if (defined (__DJGPP__) || defined (__VMS__) || defined (__UTYPE_BEOS))
struct protoent {                       /*  Protocol entry                   */
    char  *p_name;
    char **p_aliases;
    int    p_proto;
};
struct protoent *getprotobynumber (int);
struct protoent *getprotobyname   (const char *);
struct servent  *getservbyport    (int, const char *);
#endif
#if (defined (__DJGPP__))
struct servent {                        /*  Server entry                     */
    char  *s_name;
    char **s_aliases;
    int    s_port;
    char  *s_proto;
};
#endif

/*---------------------------------------------------------------------------
 *  Required definitions
 *
 *  These are symbols used by calling programs: if the compiler does not
 *  define them, we do it.
 */

#if (!defined (EWOULDBLOCK))            /*  BSD tends to use EWOULDBLOCK     */
#   if (defined (__WINDOWS__))
#       define EWOULDBLOCK  WSAEWOULDBLOCK
#   else
#       define EWOULDBLOCK  -1
#   endif
#endif

#if (!defined (EINPROGRESS))            /*  Return code for asynch I/O       */
#   if (defined (__WINDOWS__))
#       define EINPROGRESS  WSAEINPROGRESS
#   else
#       define EINPROGRESS  EWOULDBLOCK
#   endif
#endif

#if (!defined (EAGAIN))                 /*  Return code for asynch I/O       */
#   define EAGAIN           EWOULDBLOCK
#endif

#if (!defined (EPIPE))                  /*  Return code for closed socket    */
#   define EPIPE            -1
#endif

#if (!defined (ECONNRESET))             /*  Return code for closed socket    */
#   if (defined (__WINDOWS__))
#       define ECONNRESET   WSAECONNRESET
#   else
#       define ECONNRESET   EPIPE
#   endif
#endif

#if (defined (__UTYPE_HPUX))
#   define FD_SETTYPE (int *)           /*  Some systems use the older       */
#else                                   /*    select() format                */
#   define FD_SETTYPE                   /*  Most use fd_set's                */
#endif

#if (!defined (MAXHOSTNAMELEN))         /*  Some guys don't define this      */
#   define MAXHOSTNAMELEN   256         /*    constant                       */
#endif
#if (!defined (INADDR_NONE))            /*  Some guys don't define this      */
#   define INADDR_NONE      -1          /*    constant                       */
#endif


/*---------------------------------------------------------------------------
 *  Error values returned by connect_error(). These reflect the last problem
 *  detected by one of the passive/connect connection functions.
 */

#define IP_NOERROR          0           /*  No errors                        */
#define IP_NOSOCKETS        1           /*  Sockets not supported            */
#define IP_BADHOST          2           /*  Host not known                   */
#define IP_BADSERVICE       3           /*  Service or port not known        */
#define IP_BADPROTOCOL      4           /*  Invalid protocol specified       */
#define IP_SOCKETERROR      5           /*  Error creating socket            */
#define IP_CONNECTERROR     6           /*  Error making connection          */
#define IP_BINDERROR        7           /*  Error binding socket             */
#define IP_LISTENERROR      8           /*  Error preparing to listen        */

#ifdef WIN32

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
 *  The ip_passive address is used for passive socket connections.  Initially
 *  this is set to INADDR_ANY.  It is used for one passive socket creation
 *  and reset to INADDR_ANY thereafter.
 */
 
SFL_DLL qbyte ip_passive;


/*---------------------------------------------------------------------------
 *  The ip_portbase is added to the port number when creating a service;
 *  you can set this variable before calling passive_TCP or passive_UDP.
 */

SFL_DLL int ip_portbase;


/*---------------------------------------------------------------------------
 *  The ip_nonblock flag determines whether sockets are blocking or not.
 *  Under Windows sockets are never blocking.  Under UNIX they may be, or
 *  not.  For portability to Windows, this flag is set to TRUE by default.
 *  The main consequence of this is that after a connect_xxx() call you may
 *  need to perform a select() on the socket for writing to determine when
 *  the connection has suceeded.
 */

SFL_DLL Bool
    ip_nonblock;

/*---------------------------------------------------------------------------
 *  The connect_errlist table provides messages for the connect_error()
 *  values.
 */

SFL_DLL char
    *connect_errlist[];

SFL_DLL int
    ip_sockets;                         /*  Number of open sockets           */

#ifdef __cplusplus
}
#endif
#else //GNU

/*---------------------------------------------------------------------------
 *  The ip_passive address is used for passive socket connections.  Initially
 *  this is set to INADDR_ANY.  It is used for one passive socket creation
 *  and reset to INADDR_ANY thereafter.
 */
 
#ifdef __cplusplus
extern "C" 
#endif
qbyte ip_passive;

/*---------------------------------------------------------------------------
 *  The ip_portbase is added to the port number when creating a service;
 *  you can set this variable before calling passive_TCP or passive_UDP.
 */

#ifdef __cplusplus
extern "C" 
#endif
int ip_portbase;


/*---------------------------------------------------------------------------
 *  The ip_nonblock flag determines whether sockets are blocking or not.
 *  Under Windows sockets are never blocking.  Under UNIX they may be, or
 *  not.  For portability to Windows, this flag is set to TRUE by default.
 *  The main consequence of this is that after a connect_xxx() call you may
 *  need to perform a select() on the socket for writing to determine when
 *  the connection has suceeded.
 */

#ifdef __cplusplus
extern "C" 
#endif
Bool ip_nonblock;

/*---------------------------------------------------------------------------
 *  The connect_errlist table provides messages for the connect_error()
 *  values.
 */

#ifdef __cplusplus
extern "C" 
#endif
char *connect_errlist [20];

#ifdef __cplusplus
extern "C" 
#endif
int ip_sockets;                         /*  Number of open sockets           */

#endif


#ifdef __cplusplus
extern "C"  {
#endif

/*---------------------------------------------------------------------------
 *  Function prototypes
 */


SFL_DLL int    sock_init            (void);
SFL_DLL int    sock_term            (void);
SFL_DLL sock_t passive_TCP          (const char *service, int queue);
SFL_DLL sock_t passive_UDP          (const char *service);
SFL_DLL sock_t passive_socket       (const char *service, const char *protocol,
                                     int queue);
SFL_DLL sock_t connect_TCP          (const char *host, const char *service);
SFL_DLL sock_t connect_UDP          (const char *host, const char *service);
SFL_DLL sock_t connect_TCP_fast     (const struct sockaddr_in *sin);
SFL_DLL sock_t connect_UDP_fast     (const struct sockaddr_in *sin);
SFL_DLL sock_t connect_socket       (const char *host, const char *service, const char
                             *protocol, const struct sockaddr_in *sin,
                             int retry_max, int retry_delay);
SFL_DLL int    connect_to_peer      (sock_t handle,
                             const struct sockaddr_in *sin);
SFL_DLL int    connect_error        (void);
SFL_DLL sock_t accept_socket        (sock_t master);
SFL_DLL sock_t create_socket        (const char *protocol);
SFL_DLL int    address_end_point    (const char *host, const char *service, const
                             char *protocol, struct sockaddr_in *sin);
SFL_DLL int    get_sock_addr        (sock_t handle, struct sockaddr_in *sin,
                             char *name, int namesize);
SFL_DLL int    get_peer_addr        (sock_t handle, struct sockaddr_in *sin,
                             char *name, int namesize);
SFL_DLL void   build_sockaddr       (struct sockaddr_in *sin, qbyte host, dbyte port);
SFL_DLL char  *socket_localaddr     (sock_t handle);
SFL_DLL char  *socket_peeraddr      (sock_t handle);
SFL_DLL Bool   socket_is_alive      (sock_t handle);
SFL_DLL int    socket_error         (sock_t handle);
SFL_DLL int    socket_nodelay       (sock_t handle);
SFL_DLL int    read_TCP             (sock_t handle, void *buffer, size_t length);
SFL_DLL int    write_TCP            (sock_t handle, const void *buffer, size_t length);
SFL_DLL int    read_UDP             (sock_t handle, void *buffer, size_t length,
                             const struct sockaddr_in *sin);
SFL_DLL int    write_UDP            (sock_t handle, const void *buffer, size_t length,
                             const struct sockaddr_in *sin);
SFL_DLL int    close_socket         (sock_t handle);
SFL_DLL int    sock_select          (int nfds, fd_set *readfds, fd_set *writefds,
                             fd_set *errorfds, struct timeval *timeout);
SFL_DLL char  *get_hostname         (void);
SFL_DLL qbyte  get_hostaddr         (void);
SFL_DLL qbyte *get_hostaddrs        (void);
SFL_DLL const  char *sockmsg        (void);
SFL_DLL Bool   socket_is_permitted  (const char *address, const char *mask);
SFL_DLL char  *sock_ntoa            (qbyte address);
SFL_DLL char  *get_host_file        (void);
SFL_DLL int    get_name_server      (struct sockaddr_in *ns_address, int ns_max);

#if (defined (__WINDOWS__))
SFL_DLL int    winsock_last_error   (void);
#endif

#ifdef __cplusplus
}
#endif

/*  Macros for compatibility with previous versions                          */
#define socket_hostaddr(handle) socket_peeraddr (handle)


#endif                                  /*  Include sflsock.h                */
