#ifndef __Scanner
#define __Scanner


#include "Atoms.h"


struct SourcePosition {
  AtomType Atom;
  unsigned AtomLine, AtomCol, currentLine, currentCol, furtherBlanks;
  char currentChar, subsequentChar;
};


void Scanner_INIT ();


void NextAtom ();

/* Determines the next lexical atom and returns its type in variable
   Atom; if the atom is an identifier, number, or string constant then it
   returns additionally the corresponding character string in variable
   "StringBuffer" at position "StringBufferPos" */


void SkipUntilPPescape ();


void Check (AtomType at);


void CheckPush (AtomType at);


void SaveSourcePosition (SourcePosition& sp);


void RestoreSourcePosition (SourcePosition& sp);


#endif
