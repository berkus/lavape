#ifdef __GNUC__
#pragma interface
#endif


#ifndef __LavaStream
#define __LavaStream

#include "BAdapter.h"



#ifdef Q_WS_WIN
# define LAVASTREAM_DLL __declspec(dllexport)
#else
# define LAVASTREAM_DLL
#endif


extern LAVASTREAM_DLL TAdapterFunc TInAdapter [];
extern LAVASTREAM_DLL TAdapterFunc TOutAdapter [];
//extern LAVASTREAM_DLL TAdapterFunc TSpecAdapter [];


extern "C" LAVASTREAM_DLL TAdapterFunc* TextIStream();

extern "C" LAVASTREAM_DLL TAdapterFunc* TextOStream();

//extern "C" LAVASTREAM_DLL TAdapterFunc* TextStream();

extern LAVASTREAM_DLL TAdapterFunc DInAdapter [];
extern LAVASTREAM_DLL TAdapterFunc DOutAdapter [];
//extern LAVASTREAM_DLL TAdapterFunc DSpecAdapter [];


extern "C" LAVASTREAM_DLL TAdapterFunc* DataIStream();

extern "C" LAVASTREAM_DLL TAdapterFunc* DataOStream();

//extern "C" LAVASTREAM_DLL TAdapterFunc* DataStream();

extern LAVASTREAM_DLL QString ERR_OpenInFailed;
extern LAVASTREAM_DLL QString ERR_OpenOutFailed;
//extern LAVASTREAM_DLL QString ERR_OpenInOutFailed;


class LAVASTREAM_DLL DDStreamClass : public DDMakeClass
{
public:
  DDStreamClass() {}
  DDStreamClass(bool text) {isDStream = text;}
  bool isDStream;
  virtual bool hasChildren();
  virtual void makeChildren();
  QString value2;
  virtual QString getValue0(const QString& stdLabel);
  virtual QString getValue1();
  virtual QString getValue2();
};


#endif

