#ifdef __GNUC__
#pragma interface
#endif


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
  bool Silent;
  bool emptyInsertion;
  void MakeForm (LavaDECL* decl, LavaVariablePtr resultPtr, CHEFormNode *&empForm);

  void PartialForm ( LavaDECL* FormDecl, CHEFormNode *&fNode);
  /* on input, fNode contains the address of the formNode node to which
     the produced partial form is to be linked;
     on output, fNode contains the address of the first formNode node
     of the produced partial form */

  bool AllocResultObj(LavaDECL *syn, LavaVariablePtr resultObjPtr, bool emptyOpt = false);

  void AllocFNode (CHEFormNode *&formNode, LavaDECL *syn, LavaVariablePtr resultObjPtr);
//  void UpdateFieldWidget (CHEFormNode *chFrmNd);
  void CreateEllipsis (CHEFormNode *&fNode, LavaDECL *syn);
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

private:

  LavaObjectPtr newPtr[1025];
  LavaVariablePtr actPtr;
  int PtrCount;

  bool emptyForm, hasWidgets;
  unsigned level;
  /* current depth of nesting of recursive partialForm invocations;
     used to avoid infinite recursion caused by recursive syntactic
     types with "Optional" flag */
  DString ellipsis, blank, selCode;
  DString blank2;
  DString blank4, questionMarks;
  CHEFormNode *upNode; // , *fieldNode;
  LavaDECL *currentIteration;

  TAnnotation *inheritAnnotation (LavaDECL *DECLptr);
  bool setDefaultValue (CHEFormNode *resultFNode);

  void partialForm (LavaDECL* parDECL, //for supporting the inherited classes 
                    LavaDECL* FormDecl, /*pure input*/
                    LavaVariablePtr resultObjPtr,
                    SynFlags defaultIOflags,
                    CHEFormNode *&resultFNode,
                    bool nowField, 
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
                 TIDs *classChain
                 );
  bool IterForm(CHEFormNode* resultFNode,
                LavaDECL* FormDecl,
                SynFlags& defaultIOflags,
                TIDs *classChain
                );

  LavaVariablePtr GetMemberVarPtr(LavaVariablePtr objPtr, LavaDECL* fieldDECL, int sectionNumber);
  void ReduceClassChain(TIDs* classChain);

};


#endif
