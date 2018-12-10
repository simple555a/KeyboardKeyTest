/*
xgb 2017.2.25 将本类改为单例类
*/

#ifndef OPCCLIENT_H
#define OPCCLIENT_H
#include <iostream>
#include "opcerror.h"
#include "opccomn.h"
#include "OpcEnum.h"
#include "Opcda.h"
#include "copctransaction.h"
#include "wldef.h"
#include <QObject>
#include <QDebug>
#include "windows.h"
/**
 * Copyright 2016 Chn.Captain (chn.captain@gmail.com)
 */

#define SYNCVALUE  0 //同步读取
#define ASYNCVALUE 1 //异步读取
#define UPDATERATE 10 //异步的刷新速率 10ms

//#define ITEM_MAX_NUM 1000


#define FILEGROUP "group.ini"
#define FILETAG   "tag.ini"

class COPCDataCallback;
class ItemDef
{
public:
  QString	name;
  DWORD         accessRights;
  VARIANT	value;
  WORD		quality;
  OPCHANDLE	hServerHandle;
  OPCHANDLE	hClientHandle;
  FILETIME	time;
  VARTYPE	type;
  ItemDef()
  {
    type = VT_EMPTY;
    quality = 0;
    hServerHandle = 0;
    hClientHandle = 0;

    //ZeroMemory(&time, sizeof(time));
  }
  ItemDef( const ItemDef& it )
  {
    name =		it.name;
    value =		it.value;
    quality =		it.quality;
    hServerHandle =     it.hServerHandle;
    hClientHandle =     it.hClientHandle;
    time =		it.time;
    type =		it.type;
  }
};

class OPCClient:public QObject
{
  Q_OBJECT
public:
    explicit OPCClient();
    ~OPCClient();
    static OPCClient *m_pInstance;
public:
    static OPCClient *GetInstance()
    {
         if(m_pInstance == NULL){
            m_pInstance = new OPCClient();
         }
         return m_pInstance;
    }
  COPCTransaction *m_COPCTransaction;

  bool isWriteAble;

  bool isConnected();
  HRESULT Connect(QString s);
  void DisConnect();
  HRESULT AddGroup(QString n, DWORD update_rate , int async=1);
  QStringList AddItems(QStringList names);


  bool SyncReadOPCItem( DWORD rHandle,VARIANT *saveValue);
  bool SyncReadOPCItemS(QString &itemName,VARIANT *saveValue);
  bool ReadItem(ItemDef *item);

  bool WriteValueS(QString &n,VARENUM writeType, QString &v );
  bool WriteValue( DWORD cHandle, FILETIME &time, VARIANT &value, WORD Quality );
  HRESULT WriteValue_Async(ItemDef * item);
  HRESULT WriteValue_Sync (ItemDef * item);

  HRESULT RemoveItems(QStringList inames);
  HRESULT RemoveAllItems();

  void dataCallback();
  QList<ItemDef*> *items;

  QString VariantString(VARIANT &pValue);
  QString QualityString(UINT qnr);
  QString ValueTypeString(const VARTYPE& v);
  void StringVariant(ItemDef* item, QString v);
  QString AccessTypeString(qint16 accessIdent);
  QString TimeString(FILETIME t);

  HRESULT hResult;
  HRESULT *pErrors;

  WString  m_ServerName;
  WString  m_GroupName;
  DWORD    m_UpdateRate;
  CLSID    m_clsid;
  DWORD	   m_Cookie;
  bool     m_Async;

  IConnectionPointContainer* _pIConnectionPointContainer;
  IConnectionPoint* _pIConnectionPoint;
  IOPCServer *_pIOPCServer;
  IOPCBrowseServerAddressSpace *_pIOpcNamespace;
  IOPCItemProperties *_pIOpcProperties;
  IOPCGroupStateMgt *_pIOPCGroupStateMgt;
  IOPCItemMgt *_pIOPCItemMgt;
  IOPCSyncIO * _pIOPCSyncIO;
  IOPCAsyncIO2 * _pIOPCAsyncIO2;
  IOPCDataCallback * _pIOPCDataCallback;
  COPCDataCallback *m_OPCDataCallback;

  OPCHANDLE  m_GroupHandle;
  OPCHANDLE  m_ServerHandle;
  void       ClearOPCITEMDEF( OPCITEMDEF *idef, int count = 1 );
  ItemDef*   getItemByName(QString s);
  ItemDef*   getItemByHandle(DWORD h);
  WString    qstr2Wstr(QString s);

};

#endif //OPCCLIENT_H
