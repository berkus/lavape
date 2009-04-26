#ifndef __DEBUGGER
#define __DEBUGGER

#include "ASN1File.h"
#include "Constructs.h"
#include "LavaAppBase.h"       // Hauptsymbole
#include "LavaBaseDoc.h"       // Hauptsymbole
#include "Syntax.h"       // Hauptsymbole
#include "docview.h"
#include "qstring.h"
#include <QProcess>
#include <QTcpSocket>
#include <QTcpServer>
#include "LavaProgram.h"

class LocalDebugVar : public VarAction {
public:
  LocalDebugVar(DbgStopData* data, CLavaBaseDoc* d)
  {dbgStopData = data; doc = d; }
  void run();
  virtual bool Action (CheckData &ckd, VarName *vn, TID &tid);
  DbgStopData* dbgStopData;
  CHEStackData* cheAct;
};

class LAVAEXECS_DLL CLavaDebugger : public CLavaDbgBase
{
public:
  CLavaDebugger();
  ~CLavaDebugger();

  virtual void customEvent(QEvent *ev);
  void initData(CLavaBaseDoc* d, CLavaThread *execThr);
  void resetData();
  QTcpServer *listenSocket;
  QTcpSocket *workSocket;
  ASN1InSock *get_cid;
  ASN1OutSock *put_cid;

  QString remoteIPAddress;
  quint16 remotePort;

  bool startedFromLavaPE;
  CLavaThread *m_execThread;

  DbgContType actContType;
  DbgStopData* dbgStopData;
  LocalDebugVar *varAction;
  CHAINANY /*ProgPoint*/ brkPnts;
  DbgMessage0 mSend, mReceive;

public slots:
  void start();
  void connectToClient();
  void connected();
  void receive();
  void send();
  void error(QAbstractSocket::SocketError socketError);
  void stop(DbgExitReason reason=otherError);

protected:
	//void run();
  void setBrkPnts();
  void addCalleeParams();

  Q_OBJECT
}; 

#endif
