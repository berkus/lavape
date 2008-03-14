#ifndef __SYSTEM
#define __SYSTEM


#ifdef _DEBUG
   #include "crtdbg.h"
   #define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
   #define DEBUG_CLIENTBLOCK
#endif // _DEBUG

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif


#include <limits.h>
#include <ctype.h>
#include <stddef.h>
#include <new>
#include "qevent.h"


#ifdef WIN32
#ifdef DISCO_EXPORT
#define DISCO_DLL __declspec( dllexport )
#else
#define DISCO_DLL __declspec( dllimport )
#endif
#else
#define DISCO_DLL
#endif

#undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1


typedef void *address;
typedef unsigned char *UCP;
typedef unsigned char byte;
//typedef unsigned word;
typedef unsigned long       DWORD;
typedef unsigned short      WORD;

#ifndef WIN32
#define LOWORD(l)           ((WORD)(DWORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#endif

typedef void (*DPROC) ();


enum PutGetFlag {PUT,GET,DontPUT};
/*DontPUT implies PUT and is used internally by FIO procedures
  to express the absence of the default output attribute of form fields
  (which is present with PUT) within FIO procedures */


class DISCO_DLL SET {
public:

  unsigned long int bits;


  SET () {bits = 0;};
  SET (int arg1/* = -1*/, ...);

  SET (const unsigned long b) { bits = b; }


  inline void INCL (const unsigned member) {bits |= (1<<member);}

  inline void EXCL (const unsigned member) {bits &= ~(1<<member);}

  inline unsigned long BITS () const { return bits; }

  inline bool Contains (const unsigned member) const
  {
    return (bits & (1<<member) ? true : false);
  }

  inline friend bool operator== (const SET& s1,
                    const SET& s2)
  { return s1.bits == s2.bits; }

  inline friend bool operator!= (const SET& s1,
                    const SET& s2)
  { return s1.bits != s2.bits; }

  inline friend bool operator<= (const SET& s1,
                    const SET& s2)
  { return (s1.bits & s2.bits) == s1.bits; }

  inline friend bool operator>= (const SET& s1,
                    const SET& s2)
  { return (s1.bits & s2.bits) == s2.bits; }

  inline friend SET operator+ (const SET& s1,
               const SET& s2)
  /* result = union of sets s1 and s2 */
  { SET ret(s1.bits | s2.bits); return ret;}

  SET& operator+= (const SET& s)
  /* *this = union of sets *this and s */
  { bits = bits | s.bits; return *this; }

  inline friend SET operator- (const SET& s1,
               const SET& s2)
  /* result = difference of sets s1 and s2 */
  { class SET ret(s1.bits & ~s2.bits); return ret;}

  SET& operator-= (const SET& s)
  /* *this = difference of sets *this and s */
  { bits = bits & ~s.bits; return *this; }

  inline friend SET operator* (const SET& s1,
                     const SET& s2)
  /* result = intersection of sets s1 and s2 */
  { class SET ret(s1.bits & s2.bits); return ret;}

  SET& operator*= (const SET& s)
  /* *this = difference of sets *this and s */
  { bits = bits * s.bits; return *this; }

  void print();

};

#ifdef WIN32
extern "C" DISCO_DLL void bcopy (const unsigned char* const,unsigned char*,unsigned);
#endif

#define COPY(fromVariable,toVariable) \
  bcopy((unsigned char*)&fromVariable, \
        (unsigned char*)&toVariable, \
        sizeof(toVariable));

// User defined events:
#define UEV_Idle (QEvent::Type)QEvent::User
#define UEV_LavaEnd (QEvent::Type)(QEvent::User+1)
#define UEV_LavaShow (QEvent::Type)(QEvent::User+2)
#define UEV_LavaPE_SyncTree (QEvent::Type)(QEvent::User+3)
#define UEV_LavaPE_CalledView (QEvent::Type)(QEvent::User+4)
#define UEV_LavaMsgBox (QEvent::Type)(QEvent::User+5)
#define UEV_LavaPE_SyncForm (QEvent::Type)(QEvent::User+7)
#define UEV_LavaPE_SetLastHint (QEvent::Type)(QEvent::User+8)
#define UEV_OpenObject (QEvent::Type)(QEvent::User+9)
#define UEV_LavaPE_OnDrop (QEvent::Type)(QEvent::User+10)
#define UEV_LavaDump (QEvent::Type)(QEvent::User+11)
#define UEV_LavaStart (QEvent::Type)(QEvent::User+12)
#define UEV_LavaDebug (QEvent::Type)(QEvent::User+13)
#define UEV_LavaDebugRq (QEvent::Type)(QEvent::User+14)
#define UEV_LavaDebugW (QEvent::Type)(QEvent::User+15)
#define UEV_LavaPE_setSel (QEvent::Type)(QEvent::User+16)
#define UEV_WhatNext (QEvent::Type)(QEvent::User+17)
#define UEV_PMDumpOff (QEvent::Type)(QEvent::User+18)
#define UEV_Start (QEvent::Type)(QEvent::User+19)
#define UEV_Send (QEvent::Type)(QEvent::User+20)
#define UEV_Stop (QEvent::Type)(QEvent::User+21)
#define UEV_Close (QEvent::Type)(QEvent::User+22)
#define UEV_ShowMiniEdit (QEvent::Type)(QEvent::User+23)
#define UEV_NewHandler (QEvent::Type)(QEvent::User+24)
#define UEV_LavaPE_CloseDoc (QEvent::Type)(QEvent::User+25)
#define UEV_LavaGUIEvent (QEvent::Type)(QEvent::User+26)
#define UEV_AttachHandler (QEvent::Type)(QEvent::User+27)
#define UEV_DebugStop (QEvent::Type)(QEvent::User+28)
#define UEV_TabChange (QEvent::Type)(QEvent::User+29)

class DISCO_DLL CustomEvent : public  QEvent {
public:
  CustomEvent(int type, void *data = 0):QEvent(static_cast<QEvent::Type>(type)) {
    d = reinterpret_cast<QEventPrivate *>(data);
  }
  void *data() const {
    return d;
  }
  void setData(void* data) {
    d = reinterpret_cast<QEventPrivate *>(data);
  }
};

#endif
