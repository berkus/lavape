#ifdef __GNUC__
#pragma interface
#endif

#ifndef __TreeSrch
#define __TreeSrch


#include "SYSTEM.h"
#include "DString.h"
#include "Syntax.h"
#include "GUIProgBase.h"

class LAVAGUI_DLL TreeSrchCLASS {

public:
  CGUIProgBase *GUIProg;
  ~TreeSrchCLASS() {}
  CHEFormNode *itertrp, *ititrp, *optrp;
  void INIT (CGUIProgBase *guiPr);
  CHEFormNode* FirstNode ();
  void NextNodeNotIGNORED (CHEFormNode*& chFrmNd, CHEFormNode*& syo);
  void NextNodeNotIGNOREDorEll (CHEFormNode*& chFrmNd, CHEFormNode*& syo);
//  CHEFormNode* IterationPtr ();
//  bool IsPoppedDown (CHEFormNode* trp);
  CHEFormNode* NthUnprotected (unsigned n);
  CHEFormNode* NextUnprotected (CHEFormNode* trp, CHEFormNode* frmPtr = 0);
  CHEFormNode* NUnpInSubtree (CHEFormNode* trp);
  CHEFormNode* PrevUnprotected (CHEFormNode* trp);
  CHEFormNode* LastUnprotected (CHEFormNode* trp);
  //void finalTagFieldType (LavaDECL* tft, LavaDECL*& ftft);

private:

  void nextUnprotected (CHEFormNode* trp, CHEFormNode* fmno, bool& begin, bool& found, CHEFormNode*& trpn);
  void nUnpInSubtree (CHEFormNode* trp, CHEFormNode* fmno, bool& begin, bool& found, CHEFormNode*& trpn);
  void prevUnprotected (CHEFormNode* trp, CHEFormNode* fmno, bool& begin, bool& found, CHEFormNode*& trpn);
  void nthUnprotected (CHEFormNode* fmno, bool& found, CHEFormNode*& trpn, unsigned n, unsigned& i);
  void lastUnprotected (CHEFormNode* fmno, bool& found, CHEFormNode*& trpn);

};


#endif
