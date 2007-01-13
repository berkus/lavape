#ifndef __EmptyForm
#define __EmptyForm


#include "SYSTEM.h"
#include "Syntax.h"
#include "LavaBaseDoc.h"
#include "SynIO.h"
#include "STR.h"
#include "GUIProgBase.h"


class LAVAGUI_DLL LavaFormCLASS {

public:
  CGUIProgBase *GUIProg;
  ~LavaFormCLASS() {}
  void INIT (CGUIProgBase *guiPr);
  void MakeForm (LavaDECL* decl, LavaVariablePtr resultPtr, CHEFormNode *&empForm);
  void PartialForm ( LavaDECL* FormDecl, CHEFormNode *&fNode, bool allowHandler);

  bool AllocResultObj(LavaDECL *syn, LavaVariablePtr resultObjPtr, bool emptyOpt = false);
  void AllocFNode (CHEFormNode *&formNode, LavaDECL *syn, LavaVariablePtr resultObjPtr);
  void CreateEllipsis (CHEFormNode *&fNode, LavaDECL *syn, bool allowHandler);
  void DeleteForm (CHEFormNode *formNode);
  void DeletePopups(CHEFormNode *object_first);
  void DeleteWindows(CHEFormNode *object_first, bool redraw, bool finalRelease=false);
  bool IsIteration (CHEFormNode*& trp);
  bool IsAdmissibleIteration (CHEFormNode* trp, bool& equal, unsigned count);
  void SetIoFlags(CHEFormNode* upNode, CHEFormNode* subNode);
  bool OnOK(CHEFormNode *object_first);
  LavaDECL* GetFinalVType(LavaDECL* decl);
  LavaDECL* FinalFormDECL(LavaDECL* decl);
  LavaDECL* GetFinalIterType(LavaDECL* decl);
  CHEFormNode* BrowseForm(CHEFormNode *formNode, LavaDECL* formDECL);
  LavaVariablePtr NewLavaVarPtr(LavaObjectPtr val);
  bool setDefaultValue (CHEFormNode *resultFNode);

  bool Silent;
  bool emptyInsertion;

private:

  LavaObjectPtr newPtr[1025];
  LavaVariablePtr actPtr;
  int PtrCount;

  bool emptyForm, hasWidgets;
  unsigned level;
  DString ellipsis, blank, selCode;
  DString blank2;
  DString blank4, questionMarks;
  CHEFormNode *upNode;
  LavaDECL *currentIteration;

  TAnnotation *inheritAnnotation (LavaDECL *DECLptr);

  void partialForm (LavaDECL* parDECL, //for supporting the inherited classes 
                    LavaDECL* FormDecl, /*pure input*/
                    LavaVariablePtr resultObjPtr,
                    SynFlags defaultIOflags,
                    CHEFormNode *&resultFNode,
                    bool nowField, 
                    bool allowHandler,
                    TIDs *classChain
                    );
  void exprList ( CHE *DECLCHptrAny,
                 SynFlags defaultIOflags,
                 CHEFormNode *resultFNode, bool nowField);
  void memberList (LavaDECL* parDECL, 
                 LavaVariablePtr resultObjPtr,
                 SynFlags defaultIOflags,
                 CHEFormNode *resultFNode,
                 bool nowField,
                 bool allowHandler,
                 TIDs *classChain
                 );
  bool IterForm(CHEFormNode* resultFNode,
                LavaDECL* FormDecl,
                SynFlags& defaultIOflags,
                bool allowHandler,
                TIDs *classChain
                );

  LavaVariablePtr GetMemberVarPtr(LavaVariablePtr objPtr, LavaDECL* fieldDECL, int sectionNumber);
  void ReduceClassChain(TIDs* classChain);

};


#endif
