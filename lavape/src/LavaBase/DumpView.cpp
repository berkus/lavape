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


#ifdef __GNUC__
#pragma implementation
#endif


#include "LavaBaseDoc.h"
#include "qapplication.h"
#include "qframe.h"
#include "qlistview.h"
#include "qheader.h"
#include "DumpView.h"
#include "qpushbutton.h"
#include "qvbox.h"
#include "qlayout.h"


void ObjItem::setValues(DumpItem* item, ObjDebugItem* data)
{
  QString str1, str2;
  int ii;
  if (!myObject) {
    if (item)
      item->setText(1, "0x00000000   ");
    else
      data->Column1 = DString("0x00000000   ");
    return;
  }
  else 
    if (!isSection) {
      str1 = QString("%1").arg((ulong)(myObject), 8,16);
      for (ii=0; str1[ii]==' '; ii++)
        str1[ii]='0';
      str1 = "0x" + str1;
      if (item)
        item->setText(1, str1);
      else
        data->Column1 = DString(str1);
    }
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
      return;
  }
  else if (myObject[0][0].classDECL == myDoc->DECLTab[B_Array]) {
    if (*(int*)(myObject+LSH) == 0)
      str2 = "0x00000000";
    else
      return;
  }
  else
    return; //no value
  if (str2.isNull())
    str2 = "0";
  if (item)
    item->setText(2, str2);
  else
    data->Column2 = DString(str2);
}


bool ObjItem::hasChildren()
{
  if (!myObject || !myObject[0])
    return false;
  if (isSection)
    if (myObject[0][0].classDECL == myDoc->DECLTab[B_Set])
      return ((CHAINX*)(myObject+LSH))->first != 0;
    else if (myObject[0][0].classDECL != myDoc->DECLTab[B_Array])//array
      return *(int*)(myObject+LSH) != 0;
    else
      return myObject[0][0].classDECL->SectionInfo2;
  else
    if (myObject[0][0].classDECL != myDoc->DECLTab[B_Set]
        && myObject[0][0].classDECL != myDoc->DECLTab[B_Array])
      return myObject
             && ((myObject[0][0].classDECL->inINCL != 1) 
                || myObject[0][0].classDECL != myDoc->DECLTab[Bitset] 
                  && myObject[0][0].classDECL != myDoc->DECLTab[B_Bool]   
                  && myObject[0][0].classDECL != myDoc->DECLTab[Char]    
                  && myObject[0][0].classDECL != myDoc->DECLTab[Integer] 
                  && myObject[0][0].classDECL != myDoc->DECLTab[Float]  
                  && myObject[0][0].classDECL != myDoc->DECLTab[Double] 
                  && myObject[0][0].classDECL != myDoc->DECLTab[VLString]  
                  && myObject[0][0].classDECL != myDoc->DECLTab[Enumeration]
                  && myObject[0][0].classDECL != myDoc->DECLTab[B_Object])
             && (myObject[0][0].classDECL->SectionInfo2
                 || (myObject[0][0].nSections > 2) && (myObject[0][1].classDECL != myDoc->DECLTab[Enumeration]));
    else
      if (myObject[0][0].classDECL == myDoc->DECLTab[B_Set])
        return ((CHAINX*)(myObject+LSH))->first != 0;
      else //array
        return *(int*)(myObject+LSH) != 0;
}

void ObjItem::makeChildren(DumpItem* item, ObjDebugItem* data)

{
  LavaObjectPtr sectionPtr;
  LavaVariablePtr memPtr;
  LavaDECL *secClassDECL, *attrDecl, *attrType;
  DWORD childItem=0;
  int iSect, iiSect, ll, llast;
  QString label;
  bool hasChain=false;

  if (!childrenDrawn) {
    if (myObject && myObject[0]) {
      if (isSection) { //put the members
        secClassDECL =  myObject[0][0].classDECL;
        sectionPtr = myObject;
        if (!secClassDECL->TypeFlags.Contains(isNative)) { 
          llast = secClassDECL->SectionInfo2 + LSH;
          for (ll = LSH; ll < llast; ll++) {
            memPtr = (LavaVariablePtr)(sectionPtr + ll);
            attrDecl = myObject[0][0].attrDesc[ll-LSH].attrDECL;
            attrType = myDoc->IDTable.GetDECL(attrDecl->RefID, attrDecl->inINCL);
            label = QString(attrDecl->LocalName.c) + "   (" + QString(attrType->FullName.c);
            if (item)
              if (childItem)
                childItem = (DWORD)new DumpItem(item, (DumpItem*)childItem, myDoc, *memPtr, label, false, attrDecl->ParentDECL->DeclType == Impl);
              else
                childItem = (DWORD)new DumpItem(item, myDoc, *memPtr, label, false, attrDecl->ParentDECL->DeclType == Impl);
            else {
              childItem = (DWORD)new ObjDebugItem(data, myDoc, *memPtr, label, false, attrDecl->ParentDECL->DeclType == Impl);
              data->Children.Append(new CHE((ObjDebugItem*)childItem));
            }
          }//for (ll...
        }//section not native
        else { //chain, set or array
          if (secClassDECL == myDoc->DECLTab[B_Set]) 
            makeSetChildren(item, data);
          else if (myObject[0][0].classDECL == myDoc->DECLTab[B_Array]) 
            makeArrayChildren(item, data);
        }
      }//isSection
      else { // put the members of section 0 and all other sections
        secClassDECL =  myObject[0][0].classDECL;
        sectionPtr = myObject;
        if (!secClassDECL->TypeFlags.Contains(isNative)) { 
          llast = secClassDECL->SectionInfo2 + LSH;
          for (ll = LSH; ll < llast; ll++) {
            memPtr = (LavaVariablePtr)(sectionPtr + ll);
            attrDecl = myObject[0][0].attrDesc[ll-LSH].attrDECL;
            attrType = myDoc->IDTable.GetDECL(attrDecl->RefID, attrDecl->inINCL);
            label = QString(attrDecl->LocalName.c) + "   (" + QString(attrType->FullName.c);
            if (item)
              if (childItem)
                childItem = (DWORD)new DumpItem(item, (DumpItem*)childItem, myDoc, *memPtr, label, false, attrDecl->ParentDECL->DeclType == Impl);
              else
                childItem = (DWORD)new DumpItem(item, myDoc, *memPtr, label, false, attrDecl->ParentDECL->DeclType == Impl);
            else {
              childItem = (DWORD)new ObjDebugItem(data, myDoc, *memPtr, label, false, attrDecl->ParentDECL->DeclType == Impl, false);
              data->Children.Append(new CHE((ObjDebugItem*)childItem));
            }
          }//for (ll...
        }//section not native
        for (iSect = 1; iSect < myObject[0]->nSections; iSect++) {
          if (myObject[0][iSect].SectionFlags.Contains(SectPrimary)) {
            secClassDECL = myObject[0][iSect].classDECL;
            if ((secClassDECL->inINCL != 1) 
              || secClassDECL != myDoc->DECLTab[Bitset] 
                && secClassDECL != myDoc->DECLTab[B_Bool]   
                && secClassDECL != myDoc->DECLTab[Char]    
                && secClassDECL != myDoc->DECLTab[Integer] 
                && secClassDECL != myDoc->DECLTab[Float]  
                && secClassDECL != myDoc->DECLTab[Double] 
                && secClassDECL != myDoc->DECLTab[VLString]  
                && secClassDECL != myDoc->DECLTab[Enumeration]
                && secClassDECL != myDoc->DECLTab[B_Set]
                && secClassDECL != myDoc->DECLTab[B_Chain]
                && secClassDECL != myDoc->DECLTab[B_Object]) {
              sectionPtr = myObject - myObject[0][0].sectionOffset + myObject[0][iSect].sectionOffset;
              label = "(" + QString(secClassDECL->FullName.c) + ")";
              if (item)
                if (childItem)
                  childItem = (DWORD)new DumpItem(item, (DumpItem*)childItem, myDoc, sectionPtr, label);
                else
                  childItem = (DWORD)new DumpItem(item, myDoc, sectionPtr, label);
              else {
                childItem = (DWORD)new ObjDebugItem(data, myDoc, sectionPtr, label, false, attrDecl->ParentDECL->DeclType == Impl);
                data->Children.Append(new CHE((ObjDebugItem*)childItem));
              }
            }
            else {
              if (secClassDECL == myDoc->DECLTab[B_Set]) {
                for (iiSect = 0; !hasChain && (iiSect < myObject[0]->nSections); iiSect++) 
                  hasChain = (myObject[0][iiSect].classDECL == myDoc->DECLTab[B_Chain]);
                if (hasChain) 
                  label = "(Chain)";
                else 
                  label = "(Set)";
                sectionPtr = myObject - myObject[0][0].sectionOffset + myObject[0][iSect].sectionOffset;
                if (item)
                  if (childItem)
                    childItem = (DWORD)(new DumpItem(item, (DumpItem*)childItem, myDoc, sectionPtr, label));
                  else
                    childItem = (DWORD)(new DumpItem(item, myDoc, sectionPtr, label));
                else {
                  childItem = (DWORD)new ObjDebugItem(data, myDoc, sectionPtr, label, false, attrDecl->ParentDECL->DeclType == Impl);
                  data->Children.Append(new CHE((ObjDebugItem*)childItem));
                }
              }
            }

          }//primary
        }//for (iSect...
      }
    }
  }

}


void ObjItem::makeArrayChildren(DumpItem* item, ObjDebugItem* data)
{
  int arrayLen, ii, iSect;
  LavaObjectPtr object;
  LavaVariablePtr array = *(LavaVariablePtr*)(myObject+LSH+1);
  DWORD childItem=0;
  QString label;

  LavaDECL* elemType = myObject[0][0].vtypeDesc[0].fValue;
  arrayLen = *(int*)(myObject+LSH);
  label = "   (" + QString(elemType->FullName.c) + ")";
  for (ii = 0; ii < arrayLen; ii++) {
    label = "[" + QString("%1").arg(ii) + "]" + label;
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
    if (item)
      if (childItem)
        childItem = (DWORD)new DumpItem(item, (DumpItem*)childItem, myDoc, object, label, false, false);
      else
        childItem = (DWORD)new DumpItem(item, myDoc, object, label, false, false);
     else {
      childItem = (DWORD)new ObjDebugItem(data, myDoc, object, label, false, false);
      data->Children.Append(new CHE((ObjDebugItem*)childItem));
    }
  }
}


void ObjItem::makeSetChildren(DumpItem* item, ObjDebugItem* data)
{
  CHE* che;
  LavaObjectPtr object;
  DWORD childItem=0;
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
    if (item)
      if (childItem)
        childItem = (DWORD)new DumpItem(item, (DumpItem*)childItem, myDoc, object, label, false, false);
      else
        childItem = (DWORD)new DumpItem(item, myDoc, object, label, false, false);
    else
      childItem = (DWORD)new ObjDebugItem(data, myDoc, object, label, false, false);
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
  fromThread = data->currentThread;
  returned = false;
  show();
}


LavaDumpFrame::~LavaDumpFrame()
{
  if (!returned && !wxTheApp->deletingMainFrame) {
    returned = true;
    (*fromThread->pContExecEvent)--;
  }

}

void LavaDumpFrame::closeEvent(QCloseEvent *e)
{
  if (!returned) {
    returned = true;
    (*fromThread->pContExecEvent)--;
  }
  QWidget::closeEvent(e);
}

void LavaDumpFrame::OnOK()
{
  if (!returned) {
    returned = true;
    QApplication::postEvent(this, new QCloseEvent());
    (*fromThread->pContExecEvent)--;
  }
  QDialog::accept();
}


DumpItem::DumpItem(DumpItem* parent, DumpItem* afterItem, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName, bool isSec, bool priv)
  :QListViewItem(parent, afterItem) 
{ 
  isSection = isSec;
  if (isSection || !object)
    myObject = object;
  else {
    myObject = object - object[0][0].sectionOffset; 
    if (myObject != object)
      varName = varName + " / " + QString(myObject[0][0].classDECL->FullName.c);
    varName = varName + ")";
  }
  isTop = false;
  isPriv = priv;
  myDoc = doc;
  childrenDrawn = false;
  withChildren = hasChildren();
  setExpandable(withChildren);
  setHeight(16);
  varName = varName + "   ";
  setText(0,varName);
  setValues(this, 0);    
}

DumpItem::DumpItem(DumpItem* parent, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName, bool isSec, bool priv)
  :QListViewItem(parent) 
{ 
  isSection = isSec;
  if (isSection || !object)
    myObject = object;
  else {
    myObject = object - object[0][0].sectionOffset; 
    if (myObject != object)
      varName = varName + " / " + QString(myObject[0][0].classDECL->FullName.c);
    varName = varName + ")";
  }
  isTop = false;
  isPriv = priv;
  myDoc = doc;
  childrenDrawn = false;
  withChildren = hasChildren();
  setExpandable(withChildren);
  setHeight(16);
  varName = varName + "   ";
  setText(0,varName);
  setValues(this, 0);    
}


DumpItem::DumpItem (DumpListView* parent, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName)
  :QListViewItem(parent) 
{ 
  isSection = false;
  isPriv = false;
  if (isSection || !object)
    myObject = object;
  else
    myObject = object - object[0][0].sectionOffset; 
  isTop = true;
  myDoc = doc;
  childrenDrawn = false;
  withChildren = hasChildren();
  setExpandable(withChildren);
  setHeight(16);
  varName = varName + "   ";
  setText(0,varName);
  setValues(this, 0);    
}


DumpItem::~DumpItem()
{
}


void DumpItem::setOpen(bool O)
{
  if (O && withChildren) {
    makeChildren(this, 0);
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
  addColumn("Address");
  addColumn("Value");
  setRootIsDecorated(true);
  header()->show();
  setSelectionMode(QListView::Single);
  if (myObject && myObject[0]) {
    myObject = myObject - myObject[0][0].sectionOffset; 
    label = varName + "   (" + QString(myObject[0][0].classDECL->FullName.c) + ")";
  }
  else
    label = varName;
  rootItem = new DumpItem(this, myDoc, object, label);
  rootItem->setOpen(true);
}

/* Lava *******************************************************************************/

ObjDebugItem::ObjDebugItem(ObjDebugItem* parent, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName, bool isSec, bool priv, bool drawChildren)
{
  isSection = isSec;
  if (isSection || !object)
    myObject = object;
  else {
    myObject = object - object[0][0].sectionOffset; 
    if (myObject != object)
      varName = varName + " / " + QString(myObject[0][0].classDECL->FullName.c);
    varName = varName + ")";
  }
  isTop = false;
  isPriv = priv;
  isPrivate = priv;
  myDoc = doc;
  childrenDrawn = false;
  withChildren = hasChildren();
  HasChildren = withChildren;
  varName = varName + "   ";
  Column0 = DString(varName);
  setValues(0, this);
  if (drawChildren) {
    makeChildren(0, this);
    childrenDrawn = true;
  }
}

ObjDebugItem* ObjDebugItem::openObj (CHE* cheObj, CHEint* number)
{
  if (number->data)
    for (int ii = 0; ii < number->data; ii++) 
      cheObj = (CHE*)cheObj->successor;
  if (!((ObjDebugItem*)cheObj->data)->childrenDrawn)
    ((ObjDebugItem*)cheObj->data)->makeChildren(0, (ObjDebugItem*)cheObj->data);
  if (number->successor) 
    return openObj((CHE*)((ObjDebugItem*)cheObj->data)->Children.first, (CHEint*)number->successor);
  else
    return (ObjDebugItem*)cheObj->data;
  
}
