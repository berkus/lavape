#ifndef __SYSTEM
#define __SYSTEM

#include <limits.h>
#include <ctype.h>
#include <stddef.h>
#include <new>


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


typedef unsigned char *address;
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
//#endif

#endif
