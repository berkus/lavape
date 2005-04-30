#ifndef __String
#define __String
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "SYSTEM.h"

class DISCO_DLL DString {
protected:
  void init (const DString& from);

  void copy (const DString& from);

public:
  short unsigned l;  /* current length of string */
  short unsigned m;  /* maximum length of string */
  char *c;  /* pointer to actual string contents */



  DString ()
  { l = 0; m = 0; c = 0; }
  
  DString (const DString& str) { init(str); }

  DString& operator= (const DString& str);
  /* Assigns (= copies the contents of) str to *this */
  
  DString (const unsigned maxLen);
  
  DString (const char * const s);
  // '\0' terminates s
  
  DString (const unsigned char * const s);
  // '\0' terminates s
  
  DString (const char * const s, const unsigned length);
  // explicit length specification for s

  DString (const char ch, const unsigned count);
  /* Afterwards *this consists of count characters = ch */

  virtual void Destroy ()
  { if (c) delete [] c; l = 0; m = 0; c = 0; }
  
  inline unsigned Length () const { return l; }

  inline unsigned MaxLength () const { return m; }

  inline char& operator[] (const unsigned sub) const
    { return c[sub]; }

//  inline operator char* () { return c; }

//  inline operator unsigned char* () { return (address)c; }


  /*************************************************************************/


  void Append (const DString& str);
  /* Appends str to the end of *this */


  void CAPS ();
  /* capitalizes the letters of the entire string */


  friend DISCO_DLL int Compare (const DString& str1, const DString& str2);
  /* Compares str1 and str2 lexicographically and returns -1 if
    str1 < str2, 0 if str1 = str2, +1 if str1 > str2 */

  friend DISCO_DLL bool Equal (const DString& str1, const DString& str2);
  /* tests str1 and str2 more efficiently for equality
     (from right to left if lengths are equal) */

  DString& operator+= (const DString& app);
  /* *this = concatenation of *this and app */
  
  friend DISCO_DLL DString operator+ (const DString& str1,
           const DString& str2);
  /* result = concatenation of str1 and str2 */

  bool Contains (const DString& substr,
                    const unsigned start,
                    unsigned& loc) const;
  /* Returns true if *this contains substr (but not before "start")
     together with the location of the first character of substr in
     *this, else returns false */
  bool DString::ContainsNoCase (const DString& substr,
                            const unsigned start,
                            unsigned& location) const;


  bool Defined () const {return (c == 0 ? false : true); }


  void Delete (const unsigned start,
               const unsigned count);
  /* Deletes "count" characters from *this, beginning at "start" */


  void Insert (const DString& substr,
               const unsigned start);
  /* Inserts substr into *this beginning at this->c[start] */


  void Repeat (const char ch,
           const unsigned count);
  /* Afterwards *this consists of count characters = ch */


  void Replace (const DString& substr,
                const unsigned start);
  /* Replaces the portion of *this starting at *this[start] with substr */


  DString Substr (const unsigned start,
     const unsigned count) const;
  /* Copies a substring of count characters from *this, beginning
     at start; copies the whole remainder of str if count = 0 */

  bool StringMatch(const DString& str1, bool matchCase, bool wholeWord);


  void Array (const unsigned count, char *a) const;
  /* Copies MIN(count,this->l) characters from dynamic string *this to
     array a, all characters if count = 0; overflow of array a is ignored */


  void Expand (const unsigned by);


  void Reset (const unsigned length);
  /* adjusts *this to the specified maximum length;
     the current length is set := 0. */

  virtual ~DString () { Destroy(); }
  
};


inline DISCO_DLL bool operator== (const DString& str1,
             const DString& str2)
{ 
  if (Compare(str1,str2) == 0) return true;
  else return false;
}


inline DISCO_DLL bool operator!= (const DString& str1,
           const DString& str2)
{ 
  if (Compare(str1,str2) != 0) return true;
  else return false;
}


inline DISCO_DLL bool operator< (const DString& str1,
          const DString& str2)
{
  if (Compare(str1,str2) < 0) return true;
  else return false;
}


inline DISCO_DLL bool operator<= (const DString& str1,
             const DString& str2)
{ 
  if (Compare(str1,str2) <= 0) return true;
  else return false;
}


inline DISCO_DLL bool operator> (const DString& str1,
          const DString& str2)
{ 
  if (Compare(str1,str2) > 0) return true;
  else return false;
}


inline DISCO_DLL bool operator>= (const DString& str1,
             const DString& str2)
{   
  if (Compare(str1,str2) >= 0) return true;
  else return false;
}


struct DISCO_DLL Bitstring : public DString {
protected:

  void init (const Bitstring& from);

  void copy (const Bitstring& from);

public:

  Bitstring () : DString() {}
    
  Bitstring (const Bitstring& str) : DString() { init(str); }

  Bitstring& operator= (const Bitstring& str)
  { copy(str); return *this; }
  
  Bitstring (const unsigned maxLen);
 
  virtual ~Bitstring() { Destroy(); }

};


#endif
