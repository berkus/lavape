// CLavaProgram.h : Schnittstelle der Klasse CCLavaProgram
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __LavaProgram
#define __LavaProgram


#include "LavaBaseDoc.h"
#include "Constructs.h"
#include "wx_obj.h"


class LAVAEXECS_DLL CLavaProgram : public CLavaBaseDoc
{

public:
  //DString PathName;

protected: // Nur aus Serialisierung erzeugen
  CLavaProgram();

  int getSectionNumber(CheckData& ckd, LavaDECL *classDECL, LavaDECL* baseDECL);
  bool CheckNamesp(CheckData& ckd, LavaDECL* nspDECL);
  bool CheckContext(CheckData& ckd, const CContext& con);
  bool CheckFuncImpl(CheckData& ckd, LavaDECL* funcDECL, LavaDECL* classFuncDECL);
  bool CheckSetAndGets(CheckData& ckd, LavaDECL* implDECL, LavaDECL* classDecl);
  bool CheckOneSetGet(CheckData& ckd, TypeFlag setgetflag, LavaDECL* implDECL, LavaDECL* propDecl);
  bool CheckVElems(CheckData& ckd, LavaDECL *classDECL, bool fromNsp, DWORD /*TAdapterFunc**/ funcAdapter);
  bool AddVElems(CheckData& ckd, LavaDECL *classDECL, LavaDECL* baseDECL);
  bool AddVBase(CheckData& ckd, LavaDECL *classDECL, LavaDECL* baseDECL);
  bool AttachPrivatElems(CheckData& ckd, LavaDECL* classDECL);
  bool CheckFuncInOut(CheckData& ckd, LavaDECL* funcDECL);
  bool CheckMenu(CheckData& ckd, LavaDECL* formDECL, LavaDECL* classDECL);
  bool nVirtualBase(CheckData& ckd, LavaDECL *classDECL, LavaDECL* conDECL);
  bool AddVirtualBase(CheckData& ckd, LavaDECL *classDECL, LavaDECL* conDECL, int posSect);
  bool AllocSectionTable(CheckData& ckd, LavaDECL *classDECL);

public:

  bool OnOpenProgram(DString pathName, bool imiExec, bool reDef, bool putErr);
  virtual ~CLavaProgram();
  virtual bool CheckImpl(CheckData& ckd, LavaDECL* classDECL);
  virtual int GetMemsSectionNumber(CheckData& ckd, LavaDECL *classDECL, LavaDECL* memDECL, bool putErr=true);
  virtual int GetSectionNumber(CheckData& ckd, const TID& classID, const TID& baseclassID);
  virtual int GetSectionNumber(CheckData& ckd, LavaDECL *classDECL, LavaDECL* baseDECL, bool putErr=true);
  virtual int GetVTSectionNumber(CheckData& ckd, const CContext& context, LavaDECL* paramDECL, bool& outer);
  LavaObjectPtr CastVInObj(CheckData& ckd, LavaObjectPtr obj, const CContext& callCtx, LavaDECL* ET_casted, LavaDECL* VT_f, int v0, bool outer);
  LavaObjectPtr CastVOutObj(CheckData& ckd, LavaObjectPtr obj, const CContext& callCtx, LavaDECL* VT_f, int v0, bool outer);
  LavaObjectPtr CastVObj(CheckData& ckd, LavaObjectPtr obj, LavaDECL* eType);
  int GetVStatCallSecN(CheckData& ckd, LavaObjectPtr obj, const CContext& lowCtx, LavaDECL* vType, int vSec, bool outer );
  virtual bool MakeFormVT(LavaDECL *decl, CheckData* pckd = 0);
  virtual bool MakeVElems(LavaDECL *classDECL, CheckData* pckd = 0);
  virtual bool CheckForm(CheckData& ckd, LavaDECL* formDECL, int l = 0);
  bool MakeSectionTable(CheckData& ckd, LavaDECL *implDECL);
  void InitBAdapter();
  bool LoadSyntax(DString& fn, SynDef*& sntx, bool reDef, bool putErr);

//  virtual bool IsOuterParam(paramDECL, typeDECL);
//  virtual int GetContextsSectionNumber(LavaDECL* classDECL, LavaDECL* baseDECL, bool outer);
//  virtual int GetVTDelta(LavaDECL* ET_call, LavaDECL* ET_casted, LavaDECL* FVT_f, int v0, bool outer);
//  virtual BOOL openFView(LavaDECL* decl, LavaVariablePtr in, LavaVariablePtr out) {return FALSE;}
//  virtual void OnClose( OLECLOSE dwCloseOption ); 


  virtual void LavaError(CheckData& ckd, bool setEx, LavaDECL *decl, QString *nresourceID, LavaDECL* refDECL = 0);
  void HCatch(CheckData& ckd);


};

extern LAVAEXECS_DLL bool ShowFuncEdit(CheckData& ckd, LavaVariablePtr stack);
extern LAVAEXECS_DLL bool ShowFuncFillIn(CheckData& ckd, LavaVariablePtr stack);
extern LAVAEXECS_DLL unsigned ExecuteLava(CLavaBaseDoc *doc );

extern LAVAEXECS_DLL void sigEnable();
#ifndef WIN32
extern LAVAEXECS_DLL void signalHandler(int sig_number, siginfo_t *info);
#endif


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio f�gt zus�tzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif
