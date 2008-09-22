#ifndef __SynIDTable
#define __SynIDTable


#include "BASEMACROS.h"
#include "DString.h"
#include "SynIO.h"
#include "docview.h"
#include "Syntax.h"
#include "qobject.h"
#include "qstring.h"


enum TIDType { noID, localID , globalID };


class LAVABASE_DLL  TIDEntry {
public:
  TIDType idType;
  LavaDECL ** pDECL; 
  int newID;
  TIDEntry() {newID = -1; idType = noID; pDECL=0;}
};

class LAVABASE_DLL TINCLTrans {
public:
  int nINCL;
  bool isValid;
  DString FileName;
  TINCLTrans() {isValid = false;}
};

class LAVABASE_DLL  TSimpleTable {
public:
  int maxID;
  int freeID;
  TIDEntry **SimpleIDTab;
  bool isValid;
  DString FileName;
  DString LinkName;  

  TINCLTrans *nINCLTrans; 
  //a referenced object (incl,id) in a SimpleTable has the reference (nINCLTrans[incl].nINCL,id) in the document
  int maxTrans; //the creation dimension of nINCLTrans
  TSimpleTable(int minimum=0);
  ~TSimpleTable();
  void AddID(LavaDECL ** pdecl);
  void NewID(LavaDECL ** pdecl);
  void AddLocalID(DWORD pvar, int id);
  int NewLocalID(DWORD pvar);
  TIDType DeleteID(int delID);
  void UndoDeleteID(int delID);
  void UndoDeleteLocalID(int delID);
};


//enum TTableUpdata {onUndoDeleteID, onDeleteID, onAddID, onNewID, onChange, onUndoChange, onCopy};
// defined in Syntax.ph

class LAVABASE_DLL  TIDTable {
public:
  int maxINCL;
  int freeINCL;
  DString DocName;
  DString DocDir;
  DString FileExtension;
  TSimpleTable **IDTab;
  SynDef * mySynDef;
  bool InCopy;
  bool inUpdateDoc;
  int BasicTypesID [30]; //BasicTypesID[ i] is the ID of the n-th basic type,
                         //   OwnID = BasicTypesID[(int)BType]
  TIDs ChangeTab; //this is used if a decl is copied from one doc to another doc, nID: old ID, nINCL: new ID
  SyntaxDefinition* ClipTree; 
  DString *ClipDocPathName;
  int useInINCL;
  bool inPattern;
  int DragINCL;
  int DropINCL;
  TIDTable* DropTable;
  bool hasRefToClipID;
  LavaDECL* lastImpl;
  LavaDECL* catchfuncDecl;
  TIDs implOfExToBase;
  wxDocument* DropImplDoc;
  bool inDragExToBase;
  bool inDropExToBase;

  bool inExToBase;
  CHAINX isAncChain;

  TIDTable();
  ~TIDTable();
  LavaDECL* GetDECL(const TID& id, int fromIncl = 0);
  LavaDECL* GetDECL(int incl, int id, int fromIncl = 0);
//  int CheckID(const TID& id, LavaDECL**& pdecl, int fromIncl = 0); //return value: -1:ungültig; 0:OK; >0:neue ID nach drag & drop
  DWORD GetVar(const TID& id, TIDType& idtype, int fromIncl = 0); 
  void SetVar(const TID& id, DWORD data);
  int GetINCL(const DString& otherSytaxName, const DString& otherDocDir);
  void NewID(LavaDECL ** pdecl);
  void AddID(LavaDECL ** pdecl, int incl=0);
  void AddLocalID(DWORD pp, int id, int incl=0);
  void NewLocalID(DWORD pp, int& id, bool execCopy=false);
  void AddINCL(const DString& name, int incl);
  int NewINCL(const DString& name);
  DString GetRelSynFileName(const TID& id);
  bool EQEQ(const TID& id1, int inINC1, const TID& id2, int inINC2);
  int AddSimpleSyntax(SynDef* isyntax, const DString& idocDir, bool reload=false, CHESimpleSyntax* ownSyn=0);
  void MakeTable(SynDef* syn, bool isstd = false);
  void FillBasicTypesID(CHESimpleSyntax* stdSyntax, bool isstd=false);
  void Down(LavaDECL *elDef,
            TTableUpdate onWhat, /*-2:undo delete, -1:delete, 0:add, 1:new, 2:change, 3:undo change*/
            int nINCL=0);
  bool Overrides(const TID& upId, int upinINCL, const TID& id, int inINCL, LavaDECL* conDECL);
  bool IsAn(LavaDECL *decl, const TID& id, int inINCL);
  bool IsAn(const TID& upId, int upinINCL, const TID& id, int inINCL); //with equals
  bool IsAnc(const TID& upId, int upinINCL, const TID& id, int inINCL, LavaDECL* conDECL=0, bool isI = false);//true :  id is an ancestor of upId, without equals
  bool IsAnc(LavaDECL *decl, const TID& id, int inINCL, LavaDECL* conDECL=0, bool isI = false, bool cheStart=true);
  bool HasVBase(LavaDECL *decl, const TID& vid, int inINCL);
  LavaDECL* GetFinalDef(const TID& id, int inINCL = 0);
  LavaDECL* GetFinalBasicType(const TID& id, int inINCL, LavaDECL* conDECL);
  bool Overrides(const TID& upId, int upinINCL, const TID& id, int inINCL);
  bool Overrides(LavaDECL* decl1, LavaDECL* decl2);
  QString*  CleanSupports(LavaDECL *decl, LavaDECL* contDECL);
  int InsertBaseClass(LavaDECL *decl, LavaDECL* newbasedecl, LavaDECL* contDECL, bool putBase);
  CHETVElem* FindElemInVT(LavaDECL* dropClassDecl, LavaDECL* dragClassDecl, LavaDECL* dragEl, int dragINCL);
  CHETVElem* FindSamePosInVT(CHETVElem *addElem, CHETVElem *ElStart, CHETVElem *ElLast);
  //  bool PatParamInContext(LavaDECL* patparamDECL, LavaDECL* decl);
  void GetPattern(LavaDECL* decl, CContext& context, bool start=true);
  void GetContextDECLs(LavaDECL* decl, CContext& context, bool start=true);
  bool okPatternLevel(LavaDECL* paramDECL);
  bool lowerOContext(LavaDECL* highDECL, LavaDECL *lowDECL, bool& sameContext);
  bool FindParamOfVal(LavaDECL *decl, LavaDECL *pat, LavaDECL*& paramDECL);
  bool virtualSelf(LavaDECL *decl, LavaDECL*& paramDECL);
  bool isValOfVirtual(LavaDECL *decl, LavaDECL* baseDECL=0);
  bool isValOfOtherVirtual(LavaDECL *decl, LavaDECL* vdecl);
  bool otherOContext(LavaDECL* decl1, LavaDECL *decl2);
  bool CheckValOfVirtual(LavaDECL* VTDecl, bool cor=true);
  //int GetnINCLinDoc(int inIncl, int nINCL);
  void SetAsName(int incl, const DString& newAsName, const DString& oldAsName, LavaDECL* topDECL );
  QString* SetImplDECLs(LavaDECL*& errDECL);
  TID FindImpl(TID id, int funcnID);
  void SetImplIDs(bool withTest);
  void RemoveImplID(LavaDECL* impl);
  void AddImplID(LavaDECL* impl);
  void DestroyTable();

  bool GetParamRefID(LavaDECL* decl, TID& ElID, SecondTFlag flag);
  bool GetParamID(LavaDECL* decl, TID& ElID, SecondTFlag flag);
  void CopiedToDoc(LavaDECL ** pnewDECL);
  void ChangeIDFromTab(int& id);
  bool ChangeFromTab(TID& id);
  void StartClipIDs(int dragINCL, int dropINCL, TIDTable* dropTable);
  void ChangeRefToClipID(TID& id);
  void ChangeRefsToClipIDs(LavaDECL* decl);
  void ChangeRefsToClipIDsApx();
  void PrepareClipTree(SyntaxDefinition* clipTree, DString* clipDocPathName) {ClipTree = clipTree; ClipDocPathName = clipDocPathName;};

  void Change(LavaDECL ** pnewDECL);
  void DownChange(LavaDECL ** pnewDECL);
  void UndoChange(LavaDECL ** poldDECL, LavaDECL* newDECL=NULL);
  void SetPatternStart(int overID, int newID);
  void ChangeLocalID(DWORD pvar, int id);
  void DeleteID(int delID);
  void UndoDeleteID(int delID);
  void UndoDeleteLocalID(int delID);

  void DeleteINCL(int nINCL);
  void UndoDeleteINCL(int nINCL);
  void RemoveFromInclTrans(int incl, const DString& fileName);
  int IsFileInherited(const DString& fileName);
  bool IsFileInherited(const DString& fileName, int incl);
  void SetInclTransValid(int incl, const DString& inclFile, int newINCL);
};

extern  LAVABASE_DLL DString pkt;
extern  LAVABASE_DLL DString kla;
extern  LAVABASE_DLL DString klz;
extern  LAVABASE_DLL DString komma;
extern  LAVABASE_DLL DString ddppkt;
extern  LAVABASE_DLL DString lthen;
extern  LAVABASE_DLL DString grthen;
extern  LAVABASE_DLL DString pktbsl;
extern  LAVABASE_DLL DString pktsl;
extern  LAVABASE_DLL DString pktpktsl;
extern  LAVABASE_DLL DString pktpktbsl;
extern  LAVABASE_DLL QString StdLava;
extern  LAVABASE_DLL QString StdLavaLog;
extern  LAVABASE_DLL QString ExeDir;
extern  LAVABASE_DLL QString RegDir;
extern  LAVABASE_DLL QString ComponentLinkDir;
extern  LAVABASE_DLL void InitGlobalStrings();

/////////////////////////////////////////////////////////////////////////////

#endif
