#ifdef __GNUC__
#pragma interface
#endif

#ifndef __ElemCode
#define __ElemCode


/**********************************************************************/
/*                                                                    */
/*                    #includes/types/constants                       */
/*              required by the public class members:                 */
/*                                                                    */
/**********************************************************************/


#include "Atoms.h"

#include "IntCode.h"


/**********************************************************************/
/*         initialization proc. (the former "module body"):           */
/**********************************************************************/
void EC_INIT ();

extern DString designator;
extern unsigned sourceLine, sourceCol;


/* **********************************************************************/
/* elementary procedures for indentation and code generation: */


void incIndent ();
/* increment indentation */


void decIndent ();
/* decrement indentation */


void indentHere (unsigned& savedInd);
/* set new indentation column before the current code position */


void restoreIndent (unsigned savedInd);


void writeIndent ();


void writeLn ();


void code (const char *text);
/* put constant text code */


inline void code (const DString& str)
{ code(str.c); }


void codeNl (const char *text);
/* put constant text code and then new line */


void nlCode (const char *text);
/* put new line and then constant text code */


void codeCard (unsigned c);
/* put CARDINAL */


void codeVariable (TStringBufferPos sbp);
/* put string data from StringBuffer */


void codeAtom (TreeNodePtr node);
/* put atom value */


void right (unsigned steps);

void down ();

void up ();

void flushAddressStack ();


void appendIdToDesignator (TStringBufferPos sbp);
/* append identifier to designator */


void appendIndexToDesignator (TStringBufferPos sbp);
/* append array index to designator */


void restoreDesignator (unsigned savedDesLength);
/* restore the original designator */


void copyText (TreeNodePtr upToNode,
         bool lastCharToo);
/* copy non-preprocessor statements to output file */


void skipText (TreeNodePtr upToNode);
/* skip pre-processor macro or module statement 
   up to the concluding semicolon */


void skipIncludeMacro (TreeNodePtr hFileNode);
/* used to skip $INCLUDE macro up to and including the concluding '"'
   of the include file name */

#endif
