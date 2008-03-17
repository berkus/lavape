#ifndef _LAVADOC
#define _LAVADOC


#include "LavaProgram.h"
#include "Constructs.h"
#include "qdatastream.h"
#include <QList>

#ifdef WIN32
  #include "qt_windows.h"
  #include "commdlg.h"
#endif


class CLavaDoc : public CLavaProgram 
{
public:
  FACTORY(CLavaDoc)
  CLavaDoc();
  virtual ~CLavaDoc();

  virtual bool IsModified();
  virtual void Serialize(CheckData& ckd, QDataStream& ar);
  virtual bool OnOpenDocument(const QString& lpszPathName); 
  virtual bool OnCloseDocument();
  virtual bool OnNewDocument();
  virtual bool OnSaveDocument(const QString& lpszPathName);
  virtual bool SelectLcom(bool emptyDoc);
	virtual void customEvent(QEvent *e);
  virtual bool SaveAs();
  virtual bool DeleteContents() ;

protected:
  void SerializeObj(CheckData& ckd, QDataStream& ar, LavaVariablePtr pObject, const QString& ldocName );
  bool Store(CheckData& ckd, ASN1tofromAr* cid, LavaObjectPtr object);
  bool Load(CheckData& ckd, ASN1tofromAr* cid, LavaVariablePtr pObject);
  void FullTab();
  void StoreChain(ASN1tofromAr* cid, LavaObjectPtr object);
  QString* LoadChain1(CheckData& ckd, ASN1tofromAr* cid, LavaObjectPtr object);
  void LoadChain2(CheckData& ckd, LavaObjectPtr object);
  void StoreArray(ASN1tofromAr* cid, LavaObjectPtr object);
  QString* LoadArray1(ASN1tofromAr* cid, LavaObjectPtr object);
  void LoadArray2(CheckData& ckd, LavaObjectPtr object);
  bool OnEmptyObj(const DString& lcomName, const DString& linkName);

public:
  LavaObjectPtr *ObjTab;
  int MaxTab;
  int ActTab;
  DString ObjectPathName;
  DString NameToStore;
  bool firstStore;
  bool newLdoc;
  virtual LavaObjectPtr OpenObject(CheckData& ckd, LavaObjectPtr urlObj);
  virtual bool SaveObject(CheckData& ckd, LavaObjectPtr object);
//  void CalcObjName(const DString& FileName);
  void LObjectError(CheckData& ckd, const QString& ldocName, const QString& lcomName, QString* nresourceID, int moreText = 0, const DString* text = 0, const TID* id = 0);
  bool ExecuteLavaObject();

private:
	Q_OBJECT

};


#endif
