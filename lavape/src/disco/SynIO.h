#ifdef __GNUC__
#pragma interface
#endif

// SynIO.h : interface of the SynIOCLASS class
//
/////////////////////////////////////////////////////////////////////////////


#ifndef __DSynIO
#define __DSynIO



#include "DString.h"

#include "Syntax.h"


struct SynDef {
  DString PathName; //only used if document is embedded, else empty string
  int Release;
  int FreeID;
  int FreeINCL;
  SyntaxDefinition SynDefTree;
  address /*TIDTable*/ IDTable;
};


class DISCO_DLL SynIOCLASS {

public:

  void INIT ();


  bool Done;
  bool Silent;
  bool fromOldStyle;
  /* error messages are printed if and as long as Silent is false
     (= default) */

  //LavaDECL** TypeInsert(SynDef *syntax, unsigned it, LavaDECL *decl, const DString& ModuleName);
  //CHEProduction*  TypeRemove(SynDef *&syntax, unsigned it);
  //void ChangeProduction(SynDef *syntax, const DString& oldName, LavaDECL *newDECL);
  //void FillInArrays(SynDef *syntax, int prevNb);

  int ReadSynDef (DString& fileName, SynDef *&syntax, ASN1* strgCid = 0);
  //returns -1 : read failed, 0: read ok, write ok, 1: read ok, write failed

  void InitSyntax(SynDef *syntax, DString name);

  bool WriteSynDef (const DString& fileName,
                    SyntaxDefinition &synDef,
                    int freeID,
                    int freeINCL,
                    int release,
                    ASN1* strgCid = 0
                    );

  bool WriteSynDef (const DString& fileName,
                    SynDef *syntax,
                    ASN1* strgCid = 0);


  void DeleteSynDef (SynDef *&syntax);

/**********************************************************************/


private:

  void error (DString errorMsg);
};

extern DISCO_DLL SynIOCLASS SynIO;


#endif
