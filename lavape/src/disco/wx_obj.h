#ifndef wxb_objh
#define wxb_objh

/*
 * File:  wx_obj.h
 * Purpose: Top level object for wxWidgets
 * Author:  Julian Smart
 * Created: 1993
 * Updated:
 * Copyright: (c) 1993, AIAI, University of Edinburgh
  Last change:  JS   25 May 97   11:16 am
 */

/* sccsid[] = "@(#)wx_obj.h 1.2 5/9/94" */


#include "SYSTEM.h"
#include "qstring.h"


#ifdef IN_CPROTO
typedef       void    *wxObject ;
#else
class wxObject;

#ifdef GNUWIN32
#ifdef GetClassName
#undef GetClassName
#endif
#endif

class wxClassInfo;
/*
 * Dynamic object system declarations
 */
typedef wxObject * (*wxObjectConstructorFn) (void);

class DISCO_DLL wxClassInfo
{
 public:
   QString className;
   QString baseClassName1;
   QString baseClassName2;
   int objectSize;

   wxObjectConstructorFn objectConstructor;

   // Pointers to base wxClassInfos: set in InitializeClasses
   // called from wx_main.cc
   wxClassInfo *baseInfo1;
   wxClassInfo *baseInfo2;
   wxClassInfo(char *cName, char *baseName1, char *baseName2, int sz, wxObjectConstructorFn fn);

   wxObject *CreateObject(void);
   inline QString GetClassName(void) { return className; }
   inline QString GetBaseClassName1(void) { return baseClassName1; }
   inline QString GetBaseClassName2(void) { return baseClassName2; }
   inline int GetSize(void) { return objectSize; }
   static wxClassInfo *FindClass(char *c);

   // Initializes parent pointers for fast searching.
   static void InitializeClasses(void);
};

DISCO_DLL wxObject *wxCreateDynamicObject(char *name);

#define DECLARE_DYNAMIC_CLASS(name) \
public:\
	static wxClassInfo class##name;\
  wxClassInfo *GetWxClassInfo() \
   { return &name::class##name; }

#define DECLARE_ABSTRACT_CLASS(name) DECLARE_DYNAMIC_CLASS(name)
#define DECLARE_CLASS(name) DECLARE_DYNAMIC_CLASS(name)

////// for concrete classes
// Single inheritance with one base class

#define IMPLEMENT_DYNAMIC_CLASS(name, basename) \
wxObject *wxConstructorFor##name(void) \
   { return new name; }\
 wxClassInfo name::class##name(#name, #basename, NULL, sizeof(name), wxConstructorFor##name);

// Multiple inheritance with two base classes
#define IMPLEMENT_DYNAMIC_CLASS2(name, basename1, basename2) \
wxObject *wxConstructorFor##name(void) \
   { return new name; }\
 wxClassInfo name::class##name(#name, #basename1, #basename2, sizeof(name), wxConstructorFor##name);

////// for abstract classes
// Single inheritance with one base class

#define IMPLEMENT_ABSTRACT_CLASS(name, basename) \
 wxClassInfo name::class##name(#name, #basename, NULL, sizeof(name), NULL);

// Multiple inheritance with two base classes
#define IMPLEMENT_ABSTRACT_CLASS2(name, basename1, basename2) \
 wxClassInfo name::class##name(#name, #basename1, #basename2, sizeof(name), NULL);

#define IMPLEMENT_CLASS IMPLEMENT_ABSTRACT_CLASS
#define IMPLEMENT_CLASS2 IMPLEMENT_ABSTRACT_CLASS2

#define CLASSINFO(name) (&name::class##name)

class DISCO_DLL wxObject
{
 protected:
  int refCount;

 public:
#if USE_OLD_TYPE_SYSTEM
  WXTYPE __type;
#endif

  // This is equivalent to using the macro DECLARE_ABSTRACT_CLASS

  static wxClassInfo classwxObject;
  wxObject(void);
  virtual ~wxObject(void);
  virtual wxClassInfo *GetWxClassInfo(void) { return &classwxObject; }

  // Reference counting
  inline int GetRefCount(void) { return refCount; }
  inline void SetRefCount(int ref) { refCount = ref; }
  inline int Ref(void) { refCount ++; return refCount; }
  inline int UnRef(void) { refCount --; return refCount; }
};

extern DISCO_DLL QHash<QString,wxClassInfo*> *classDict;

#endif // IN_CPROTO
#endif // wx_objh

