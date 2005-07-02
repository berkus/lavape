#ifndef __DISCO_MACROS
#define __DISCO_MACROS


//#ifndef max
#define lmax(a,b)            (((a) > (b)) ? (a) : (b))
//#endif

//#ifndef min
#define lmin(a,b)            (((a) < (b)) ? (a) : (b))
//#endif


#ifndef WIN32
#define LOWORD(l)           ((WORD)(DWORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#endif


#ifndef NULL
#define NULL 0
#endif


#ifdef WIN32
#define strcasecmp stricmp
#else
#define O_BINARY 0
#endif

#define EVEN(arg) (!(arg & 1))
#define ODD(arg) (arg & 1)

#define ABS(x) (x >= 0 ? x : -x)

#define CAP(c) ((islower(c)) ? (c | ' ') : c)

#define TRUNC(x) (unsigned(ABS(x)))


#define COPY(fromVariable,toVariable) \
  bcopy((unsigned char*)&fromVariable, \
        (unsigned char*)&toVariable, \
        sizeof(toVariable));

#endif
