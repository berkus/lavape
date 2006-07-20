#ifndef __DSynIO
#define __DSynIO
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "qstring.h"
#include "Syntax.h"


struct SynDef {
  QString PathName; //only used if document is embedded, else empty string
  int Release;
  int FreeID;
  int FreeINCL;
  SyntaxDefinition SynDefTree;
  address /*TIDTable*/ IDTable;
};


class LAVABASE_DLL SynIOCLASS {

public:

  void INIT ();
  void EXIT ();

  bool Done;
  bool Silent;
  bool fromOldStyle;
  /* error messages are printed if and as long as Silent is false
     (= default) */

  //LavaDECL** TypeInsert(SynDef *syntax, unsigned it, LavaDECL *decl, const QString& ModuleName);
  //CHEProduction*  TypeRemove(SynDef *&syntax, unsigned it);
  //void ChangeProduction(SynDef *syntax, const QString& oldName, LavaDECL *newDECL);
  //void FillInArrays(SynDef *syntax, int prevNb);

  int ReadSynDef (const QString& fileName, SynDef *&syntax, ASN1* strgCid = 0);
  //returns -1 : read failed, 0: read ok, write ok, 1: read ok, write failed

  void InitSyntax(SynDef *syntax, QString name);

  bool WriteSynDef (const QString& fileName,
                    SyntaxDefinition &synDef,
                    int freeID,
                    int freeINCL,
                    int release,
                    ASN1* strgCid = 0
                    );

  bool WriteSynDef (const QString& fileName,
                    SynDef *syntax,
                    ASN1* strgCid = 0);


  void DeleteSynDef (SynDef *&syntax);

/**********************************************************************/


private:

  void error (QString errorMsg);
};

extern LAVABASE_DLL SynIOCLASS SynIO;


#endif
