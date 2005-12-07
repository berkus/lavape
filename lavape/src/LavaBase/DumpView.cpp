/* LavaPE -- Lava Programming Environment
   Copyright (C) 2002 Fraunhofer-Gesellschaft
	 (http://www.sit.fraunhofer.de/english/)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#include "LavaBase_all.h"
#include "MACROS.h"

#include "LavaBaseDoc.h"
#include "BAdapter.h"
#include "DumpView.h"

#include "qapplication.h"
#include "qframe.h"
#include "qlistview.h"
#include "qheader.h"
#include "qpushbutton.h"
#include "qvbox.h"
#include "qlayout.h"


QString DDMakeClass::getValue0(const QString& stdLabel)
{
  QString str0;
  if (!isSection && myObject) {
    if (isCasted)
      str0 = " / " + QString(myObject[0][0].classDECL->FullName.c);
  }
  if (myObject)
    str0 = str0 + ")";
  str0 = str0 + "   ";
  str0 = stdLabel + str0;
  return str0;
}


QString DDMakeClass::getValue1()
{
  QString str1;
  int ii;
  if (!myObject) 
    return QString("0x00000000   ");
  else 
    if (!isSection) {
      str1 = QString("%1").arg((ulong)(myObject), 8,16);
      for (ii=0; str1[ii]==' '; ii++)
        str1[ii]='0';
      str1 = "0x" + str1;
      return str1;
    }
  return str1;
}

QString DDMakeClass::getValue2()
{
  QString str2;
  if (!myObject)
    return str2;
  if (myObject[0][0].classDECL == myDoc->DECLTab[Bitset] )
    str2 = QString("%1").arg(*(ulong*)(myObject+LSH), sizeof(unsigned) * 8,2);
  else if (myObject[0][0].classDECL == myDoc->DECLTab[B_Bool] )
    if (*(bool*)(myObject+LSH))
      str2 = QString("true");
    else
      str2 = QString("false");
  else if (myObject[0][0].classDECL == myDoc->DECLTab[Char]) {
    str2 = *(char*)(myObject+LSH);
    str2 = "'" + str2 + "'";
  }
  else if (myObject[0][0].classDECL == myDoc->DECLTab[Integer] )
    str2 = QString("%1").arg(*(int*)(myObject+LSH));
  else if (myObject[0][0].classDECL == myDoc->DECLTab[Float]  )
    str2 = QString("%1").arg(*(float*)(myObject+LSH), 0,'f');
  else if (myObject[0][0].classDECL == myDoc->DECLTab[Double] )
    str2 = QString("%1").arg(*(double*)(myObject+LSH), 0,'f');
  else if (myObject[0][0].classDECL == myDoc->DECLTab[VLString] ) {
    if ((QString*)(myObject+LSH)) {
      str2 = *(QString*)(myObject+LSH);
      str2 = '"' + str2 + '"';
    }
  }
  else if (myObject[0][1].classDECL == myDoc->DECLTab[Enumeration])
     str2 = *(QString*)(myObject + myObject[0][1].sectionOffset + LSH + 1);
  else if (myObject[0][0].classDECL == myDoc->DECLTab[B_Set]) {
    if (((CHAINX*)(myObject+LSH))->first == 0)
      str2 = "0x00000000";
    else
      return str2;
  }
  else if (myObject[0][0].classDECL == myDoc->DECLTab[B_Array]) {
    if (*(int*)(myObject+LSH) == 0)
      str2 = "0x00000000";
    else
      return str2;
  }
  else
    return str2; //no value
  if (str2.isNull())
    str2 = "0";
  return str2;
}



bool DDMakeClass::hasChildren()
{
  TAdapterFunc *funcAdapter;
  CheckData ckd;

  ckd.document = myDoc;
  if (!myObject || !myObject[0])
    return false;
  int ll = myObject[0][0].classDECL->SectionInfo2;
  if (myObject[0][0].classDECL->TypeFlags.Contains(isNative)) {
    funcAdapter = GetAdapterTable(ckd, myObject[0][0].classDECL, myObject[0][0].classDECL);
    if (funcAdapter)
      ll = ll - (unsigned)*funcAdapter[0]; //- native part of section
  }
  if (isSection)
    return ll > 0;
  else
    return (ll > 0) || (myObject[0][0].nSections > 2)
                       && (myObject[0][1].classDECL != myDoc->DECLTab[Enumeration]);
}

void DDMakeClass::makeChildren()

{
  LavaObjectPtr sectionPtr, newStackFrame[SFH+2];
  LavaVariablePtr memPtr;
  LavaDECL *secClassDECL, *attrDecl, *attrType;
  DDItem *childItem=0;
  int iSect, ll, llast;
  QString label;
  bool hasChain=false;
  TAdapterFunc *funcAdapter;
  CheckData ckd;

  ckd.document = myDoc;
  if (!myItem->childrenDrawn) {
    if (myObject && myObject[0]) {
      if (isSection) { //put the members
        secClassDECL =  myObject[0][0].classDECL;
        sectionPtr = myObject;
        llast = secClassDECL->SectionInfo2 + LSH;
        ll = LSH;
        if (secClassDECL->TypeFlags.Contains(isNative)) { 
          funcAdapter = GetAdapterTable(ckd, secClassDECL, secClassDECL);
          if (funcAdapter)
            //llast = llast - (unsigned)*funcAdapter[0]; //- native part of section
            ll = LSH + (unsigned)*funcAdapter[0]; //- members behind native part of section 
        }
        for (/*ll = LSH*/; ll < llast; ll++) {
          memPtr = (LavaVariablePtr)(sectionPtr + ll);
          attrDecl = myObject[0][0].attrDesc[ll-LSH].attrDECL;
          attrType = myDoc->IDTable.GetDECL(attrDecl->RefID, attrDecl->inINCL);
          label = QString(attrDecl->LocalName.c) + "   (" + QString(attrType->FullName.c);
          childItem = myItem->createChild(new DDMakeClass, childItem, myDoc, *memPtr, label, false, attrDecl->ParentDECL->DeclType == Impl);
        }//for (ll...
        if (secClassDECL->TypeFlags.Contains(isNative)) { 
          if (secClassDECL == myDoc->DECLTab[B_Set]) 
            makeChildren();
          else if (myObject[0][0].classDECL == myDoc->DECLTab[B_Array]) 
            makeChildren();
          else {
            funcAdapter = GetAdapterTable(ckd, secClassDECL, secClassDECL);
            if (funcAdapter && funcAdapter[adapterPos_DD]) //section has DD function
              makeChildren();
          }
        }
      }//isSection
      else { // put the members of section 0 and all other sections
        secClassDECL =  myObject[0][0].classDECL;
        sectionPtr = myObject;
        if (secClassDECL->DeclType == CompObjSpec) {
          label = "url-object";
          if (sectionPtr-LOH && ((RunTimeData*)(sectionPtr-LOH))->urlObj)
            label = label + "  (" + QString((((RunTimeData*)(sectionPtr-LOH))->urlObj)[0][0].classDECL->FullName.c);
          childItem = myItem->createChild(new DDMakeClass, childItem, myDoc, ((RunTimeData*)(sectionPtr-LOH))->urlObj, label, false, false);            
        }
        llast = secClassDECL->SectionInfo2 + LSH;
        ll = LSH;
        if (secClassDECL->TypeFlags.Contains(isNative)) { 
          funcAdapter = GetAdapterTable(ckd, secClassDECL, secClassDECL);
          if (funcAdapter)
            //llast = llast - (unsigned)*funcAdapter[0]; //- native part of section
            ll = LSH + (unsigned)*funcAdapter[0];
        }
        for (/*ll = LSH*/; ll < llast; ll++) {
          memPtr = (LavaVariablePtr)(sectionPtr + ll);
          attrDecl = myObject[0][0].attrDesc[ll-LSH].attrDECL;
          attrType = myDoc->IDTable.GetDECL(attrDecl->RefID, attrDecl->inINCL);
          label = QString(attrDecl->LocalName.c) + "   (" + QString(attrType->FullName.c);
          childItem = myItem->createChild(new DDMakeClass, childItem, myDoc, *memPtr, label, false, attrDecl->ParentDECL->DeclType == Impl);            
        }//for (ll...
        if (secClassDECL->TypeFlags.Contains(isNative)) { 
          //funcAdapter = GetAdapterTable(ckd, secClassDECL, secClassDECL);
          if (funcAdapter && funcAdapter[adapterPos_DD]) {//section has DD function
            newStackFrame[0] = 0;
            newStackFrame[1] = 0;
            newStackFrame[2] = 0;
            newStackFrame[SFH] = sectionPtr;
            funcAdapter[adapterPos_DD](ckd, newStackFrame);
            childItem = myItem->createChild((DDMakeClass*)(newStackFrame[SFH+1]), childItem, myDoc, sectionPtr, label, false, false);            
          }
        }
        for (iSect = 1; iSect < myObject[0]->nSections; iSect++) {
          if (myObject[0][iSect].SectionFlags.Contains(SectPrimary)) {
            secClassDECL = myObject[0][iSect].classDECL;
            if ((secClassDECL->inINCL != 1) 
              || secClassDECL != myDoc->DECLTab[B_Chain]
                && secClassDECL != myDoc->DECLTab[B_Object]) {
              sectionPtr = myObject - myObject[0][0].sectionOffset + myObject[0][iSect].sectionOffset;
              label = "(" + QString(secClassDECL->FullName.c);
              if (secClassDECL->TypeFlags.Contains(isNative)) {
                funcAdapter = GetAdapterTable(ckd, secClassDECL, myObject[0][0].classDECL);
                if (funcAdapter && funcAdapter[adapterPos_DD]) {//section has DD function
                  newStackFrame[0] = 0;
                  newStackFrame[1] = 0;
                  newStackFrame[2] = 0;
                  newStackFrame[SFH] = 0;
                  funcAdapter[adapterPos_DD](ckd, newStackFrame);
                  childItem = myItem->createChild(*(DDMakeClass**)(newStackFrame[SFH+1]+LSH), childItem, myDoc, sectionPtr, label);            
                }
                else
                  childItem = myItem->createChild( new DDMakeClass, childItem, myDoc, sectionPtr, label);
              }
              else
                childItem = myItem->createChild( new DDMakeClass, childItem, myDoc, sectionPtr, label);
            }
          }//primary
        }//for (iSect...
      }
    }
  }

}

bool DDArrayClass::hasChildren()
{
  return *(int*)(myObject+LSH) != 0;
}

void DDArrayClass::makeChildren()
{
  int arrayLen, ii, iSect;
  LavaObjectPtr object;
  LavaVariablePtr array = *(LavaVariablePtr*)(myObject+LSH+1);
  DDItem* childItem=0;
  QString label0, label;

  LavaDECL* elemType = myObject[0][0].vtypeDesc[0].fValue;
  arrayLen = *(int*)(myObject+LSH);
  label0 = "   (" + QString(elemType->FullName.c) + ")";
  for (ii = 0; ii < arrayLen; ii++) {
    label = "[" + QString("%1").arg(ii) + "]" + label0;
    object = array[ii];
    if (object) {
      object = object - object[0][0].sectionOffset;
      for (iSect = 0;
         (iSect < object[0]->nSections)
           && (!object[0][iSect].SectionFlags.Contains(SectPrimary) 
               || (object[0][iSect].classDECL != elemType));
         iSect++);
      if (iSect < object[0]->nSections)
        object = object- object[0][iSect].sectionOffset;
    }
    childItem = myItem->createChild( new DDMakeClass, childItem, myDoc, object, label, false, false);
  }
}


bool DDSetClass::hasChildren()
{
  return ((CHAINX*)(myObject+LSH))->first != 0;
}

QString DDSetClass::getValue0(const QString& stdLabel)
{
  int iSect;
  bool hasChain = false;
  LavaObjectPtr fullObj = myObject - myObject[0][0].sectionOffset;           
  for (iSect = 0; !hasChain && (iSect < fullObj[0]->nSections); iSect++) 
    hasChain = (fullObj[0][iSect].classDECL == myDoc->DECLTab[B_Chain]);
  if (hasChain) 
    return QString("(Chain)");
  else 
    return QString("(Set)");
}


void DDSetClass::makeChildren()
{
  CHE* che;
  LavaObjectPtr object;
  DDItem* childItem=0;
  QString label;
  int iSect;

  LavaDECL *elemType = myObject[0][0].vtypeDesc[0].fValue;
  label = "   (" + QString(elemType->FullName.c) + ")";
  for (che = (CHE*)((CHAINX*)(myObject+LSH))->first; che; che = (CHE*)che->successor) {
    object = (LavaObjectPtr)che->data;
    if (object) {
      object = object - object[0][0].sectionOffset;
      for (iSect = 0;
         (iSect < object[0]->nSections)
           && (!object[0][iSect].SectionFlags.Contains(SectPrimary) 
               || (object[0][iSect].classDECL != elemType));
         iSect++);
      if (iSect < object[0]->nSections)
        object = object- object[0][iSect].sectionOffset;
    }
    childItem = myItem->createChild( new DDMakeClass, childItem, myDoc, object, label, false, false);
  }
}

LavaDumpFrame::LavaDumpFrame( QWidget* parent, DumpEventData* data)
: QDialog(parent, "Dump", true, WType_TopLevel | WStyle_MinMax)
 //   : QMainWindow(parent,"Dump",WDestructiveClose)
{
  resize(200, 300);
  view = new DumpListView(this, data->doc, data->object, data->name);
  QPushButton* okButton = new QPushButton("Ok", this);
  QVBoxLayout* qvbox = new QVBoxLayout(this);
  qvbox->addWidget(view);
  qvbox->addWidget(okButton);
  QSize sz = size();
  int w = view->columnWidth(0) + view->columnWidth(1) + view->columnWidth(2) + 2*view->frameWidth();
  if (sz.width() < w) {
    sz.setWidth(w);
    resize(sz);
  }


  connect(okButton, SIGNAL(clicked()), this, SLOT(OnOK()));
//  fromThread = data->currentThread;
  returned = false;
  show();
}


LavaDumpFrame::~LavaDumpFrame()
{
  if (!returned && !wxTheApp->deletingMainFrame) {
    returned = true;
    //(*fromThread->pContExecEvent)--;
  }

}

void LavaDumpFrame::closeEvent(QCloseEvent *e)
{
  if (!returned) {
    returned = true;
    //(*fromThread->pContExecEvent)--;
  }
  QWidget::closeEvent(e);
}

void LavaDumpFrame::OnOK()
{
  if (!returned) {
    returned = true;
    QApplication::postEvent(this, new QCloseEvent());
    //(*fromThread->pContExecEvent)--;
  }
  QDialog::accept();
}

DDItem* DumpItem::createChild(DDMakeClass* dd, DDItem* afterItem, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName, bool isSec, bool isPriv)
{
  if (afterItem)
    return new DumpItem(dd, this, (DumpItem*)afterItem, doc, object, varName, isSec, isPriv);
  else
    return new DumpItem(dd, this, doc, object, varName, isSec, isPriv);
}

DumpItem::DumpItem(DDMakeClass* dd, DumpItem* parent, DumpItem* afterItem, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName, bool isSec, bool priv)
  :QListViewItem(parent, afterItem) 
{ 
  DD = dd;
  DD->myItem = this;
  DD->isSection = isSec;
  if (DD->isSection || !object)
    DD->myObject = object;
  else 
    DD->myObject = object - object[0][0].sectionOffset; 
  DD->isCasted = DD->myObject != object;
  isTop = false;
  isPriv = priv;
  DD->myDoc = doc;
  childrenDrawn = false;
  withChildren = DD->hasChildren();
  setExpandable(withChildren);
  setHeight(16);
  setMultiLinesEnabled(true);
  setText(0,DD->getValue0(varName));
  setText(1, DD->getValue1());
  setText(2, DD->getValue2());
  //setValues(this, 0);    
}

DumpItem::DumpItem(DDMakeClass* dd, DumpItem* parent, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName, bool isSec, bool priv)
  :QListViewItem(parent) 
{ 
  DD = dd;
  DD->myItem = this;
  DD->isSection = isSec;
  if (DD->isSection || !object)
    DD->myObject = object;
  else 
    DD->myObject = object - object[0][0].sectionOffset; 
  DD->isCasted = DD->myObject != object;
  isTop = false;
  isPriv = priv;
  DD->myDoc = doc;
  childrenDrawn = false;
  withChildren = DD->hasChildren();
  setExpandable(withChildren);
  setHeight(16);
  setMultiLinesEnabled(true);
  setText(0, DD->getValue0(varName));
  setText(1, DD->getValue1());
  setText(2, DD->getValue2());
}


DumpItem::DumpItem (DDMakeClass* dd, DumpListView* parent, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName)
  :QListViewItem(parent) 
{ 
  DD = dd;
  DD->myItem = this;
  DD->isSection = false;
  isPriv = false;
  if (DD->isSection || !object)
    DD->myObject = object;
  else
    DD->myObject = object - object[0][0].sectionOffset; 
  DD->isCasted = DD->myObject != object;
  isTop = true;
  DD->myDoc = doc;
  childrenDrawn = false;
  withChildren = DD->hasChildren();
  setExpandable(withChildren);
  setHeight(16);
  setMultiLinesEnabled(true);
  varName = varName + "   ";
  setText(0,varName);
  setText(1, DD->getValue1());
  setText(2, DD->getValue2());
}


DumpItem::~DumpItem()
{
}


void DumpItem::setOpen(bool O)
{
  if (O && withChildren) {
    DD->makeChildren();
    childrenDrawn = true;
    QListViewItem::setOpen(O);
  }
  else
    QListViewItem::setOpen(O);
}


void DumpItem::paintCell( QPainter * p, const QColorGroup & cg,
			       int column, int width, int align )
{
  if (!column && isPriv) {
    QColorGroup cgN ( cg);
    cgN.setColor(QColorGroup::Text,red);
    QListViewItem::paintCell( p, cgN, column, width, align );
  }
  else
    QListViewItem::paintCell( p, cg, column, width, align );
}



//**********************************************************************************


DumpListView::DumpListView(QWidget *parent,CLavaBaseDoc* doc, LavaObjectPtr object, QString varName)
:QListView(parent, "DumpListView")
{
  QString label;
  LavaObjectPtr myObject = object;

  myDoc = doc;
  setFocusPolicy(QWidget::StrongFocus);
  setSorting(-1);
  addColumn("varName (static type [/ runtime type])");
  addColumn("address");
  addColumn("value");
  setRootIsDecorated(true);
  header()->show();
  setSelectionMode(QListView::Single);
  setShowToolTips(true);
  if (myObject && myObject[0]) {
    myObject = myObject - myObject[0][0].sectionOffset; 
    label = varName + "   (" + QString(myObject[0][0].classDECL->FullName.c) + ")";
  }
  else
    label = varName;
  rootItem = new DumpItem(new DDMakeClass, this, myDoc, object, label);
  rootItem->setOpen(true);
}

/* Lava *******************************************************************************/

DebugItem::DebugItem(DDMakeClass* dd, DebugItem* parent, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName, bool isSec, bool priv, bool drawChildren)
{
  DD = dd;
  DD->myItem = this;
  DD->isSection = isSec;
  if (DD->isSection || !object)
    DD->myObject = object;
  else 
    DD->myObject = object - object[0][0].sectionOffset; 
  DD->isCasted = DD->myObject != object;
  isTop = false;
  isPriv = priv;
  isPrivate = priv;
  DD->myDoc = doc;
  childrenDrawn = false;
  withChildren = DD->hasChildren();
  HasChildren = withChildren;
  if (!DD->myObject && parent)
    varName = varName + ")";
  Column0 = DString(DD->getValue0(varName));
  Column1 = DString(DD->getValue1());
  Column2 = DString(DD->getValue2());
  if (drawChildren) {
    DD->makeChildren();
    childrenDrawn = true;
  }
}

DDItem* DebugItem::createChild(DDMakeClass* dd, DDItem* afterItem, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName, bool isSec, bool isPriv)
{
  DebugItem* it = new DebugItem(dd, this, doc, object, varName, isSec, isPriv);
  Children.Append(new CHE((DebugItem*)it));
  return it;
}

DebugItem* DebugItem::openObj (CHE* cheObj, CHEint* number)
{
  if (number->data)
    for (int ii = 0; ii < number->data; ii++) 
      cheObj = (CHE*)cheObj->successor;
  if (!((DebugItem*)cheObj->data)->childrenDrawn)
    ((DebugItem*)cheObj->data)->DD->makeChildren();
  if (number->successor) 
    return openObj((CHE*)((DebugItem*)cheObj->data)->Children.first, (CHEint*)number->successor);
  else
    return (DebugItem*)cheObj->data;
  
}
