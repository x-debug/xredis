#pragma once
#include "all.h"
#include "xEventLoop.h"
#include "xTcpconnection.h"
#include "xBuffer.h"
#include "xTcpServer.h"
#include "xSds.h"
#include "xSession.h"
#include "xObject.h"
#include "xRdb.h"
#include "xPosix.h"
#include "xLog.h"
#include "xSocket.h"
#include "xReplication.h"
#include "xSentinel.h"

class xRedis : noncopyable
{
public:
	xRedis(const char * ip,int32_t port,int32_t threadCount,bool enbaledCluster = false,bool enabledSentinel = false);
	~xRedis();
	void init();
	void test(void * data);
	void handleTimeOut(void * data);
	void handleSalveRepliTimeOut(void * data);
	void handleSetExpire(void * data);
	
	void run();
	void connCallBack(const xTcpconnectionPtr& conn,void *data);
	bool deCodePacket(const xTcpconnectionPtr& conn,xBuffer *recvBuf,void  *data);

	void loadDataFromDisk();
	void flush();

	bool saveCommond(const std::deque <rObj*> & obj,xSession * session);
	bool pingCommond(const std::deque <rObj*> & obj,xSession * session);
	bool pongCommond(const std::deque <rObj*> & obj,xSession * session);
	bool ppingCommond(const std::deque <rObj*> & obj, xSession * session);
	bool ppongCommond(const std::deque <rObj*> & obj, xSession * session);

	bool flushdbCommond(const std::deque <rObj*> & obj,xSession * session);
	bool dbsizeCommond(const std::deque <rObj*> & obj,xSession * session);
	bool quitCommond(const std::deque <rObj*> & obj,xSession * session);
	bool delCommond(const std::deque <rObj*> & obj,xSession * session);
	bool setCommond(const std::deque <rObj*> & obj,xSession * session);
	bool getCommond(const std::deque <rObj*> & obj,xSession * session);
	bool hsetCommond(const std::deque <rObj*> & obj,xSession * session);
	bool hgetCommond(const std::deque <rObj*> & obj,xSession * session);
	bool hgetallCommond(const std::deque <rObj*> & obj,xSession * session);
	bool hlenCommond(const std::deque <rObj*> & obj,xSession * session);
	bool slaveofCommond(const std::deque <rObj*> & obj,xSession * session);
	bool syncCommond(const std::deque <rObj*> & obj,xSession * session);
	bool psyncCommond(const std::deque <rObj*> & obj,xSession * session);
	bool commandCommond(const std::deque <rObj*> & obj,xSession * session);
	bool clusterCommond(const std::deque <rObj*> & obj,xSession * session);
	bool authCommond(const std::deque <rObj*> & obj,xSession * session);
	bool configCommond(const std::deque <rObj*> & obj,xSession * session);
	bool infoCommond(const std::deque <rObj*> & obj,xSession * session);
	bool clientCommond(const std::deque <rObj*> & obj,xSession * session);
	bool echoCommond(const std::deque <rObj*> & obj,xSession * session);
	bool subscribeCommond(const std::deque <rObj*> & obj,xSession * session);
	bool publishCommond(const std::deque <rObj*> & obj,xSession * session);
	bool selectCommond(const std::deque <rObj*> & obj,xSession * session);
	bool hkeysCommond(const std::deque <rObj*> & obj,xSession * session);
	bool scardCommond(const std::deque <rObj*> & obj,xSession * session);
	bool saddCommond(const std::deque <rObj*> & obj,xSession * session);
	bool unsubscribeCommond(const std::deque <rObj*> & obj,xSession * session);
	bool zaddCommond(const std::deque <rObj*> & obj,xSession * session);
	bool zcardCommond(const std::deque <rObj*> & obj,xSession * session);
	bool zcountCommond(const std::deque <rObj*> & obj,xSession * session);
	bool zrangeCommond(const std::deque <rObj*> & obj,xSession * session);
	bool zrankCommond(const std::deque <rObj*> & obj,xSession * session);
	bool zrevrangeCommond(const std::deque <rObj*> & obj,xSession * session);
	bool keysCommond(const std::deque <rObj*> & obj,xSession * session);
	bool bgsaveCommond(const std::deque <rObj*> & obj,xSession * session);
	bool memoryCommond(const std::deque <rObj*> & obj,xSession * session);

	
	bool save(xSession * session);
	int removeCommond(rObj * obj,int &count);
	void clearCommond();
	void clearRepliState(int32_t sockfd);
	size_t getDbsize();

public:
	
	std::unordered_set<rObj*,Hash,EEqual>  unorderedmapCommonds;
	typedef std::function<bool (const std::deque<rObj*> &,xSession *)> commondFunction;
	std::unordered_map<rObj*,commondFunction,Hash,EEqual> handlerCommondMap;
	std::unordered_map<int32_t , std::shared_ptr<xSession>> sessions;

	typedef std::unordered_map<rObj*,rObj*,Hash,Equal> SetMap;
	typedef std::unordered_map<rObj*,std::unordered_map<rObj*,rObj*,Hash,Equal> ,Hash,Equal> HsetMap;
	typedef std::unordered_map<rObj*,std::unordered_set<rObj*,Hash,Equal>,Hash,Equal> Set;
	typedef std::unordered_map<rObj*,std::unordered_map<rObj *,rObj *,Hash,Equal>,Hash,Equal> SortSet;
	typedef std::unordered_map<rObj*,std::set<rSObj>,Hash,Equal> SSet;
	typedef std::unordered_map<rObj*,std::list<xTcpconnectionPtr>,Hash,Equal> PubSub;

	struct SetMapLock
	{		
		SetMap setMap;
		mutable MutexLock mutex;
	};

	struct HsetMapLock
	{
		HsetMap hsetMap;
		mutable MutexLock mutex;
	};

	struct SetLock
	{
		Set set;
		mutable MutexLock mutex;
	};

	struct SortSetLock
	{
		SortSet set;
		SSet sset;
		mutable MutexLock mutex;
	};

	struct PubSubLock
	{
		PubSub pubSub;
		mutable MutexLock mutex;
	};

	const static int kShards = 4096;
	std::array<SetMapLock, kShards> setMapShards;
	std::array<HsetMapLock, kShards> hsetMapShards;
	std::array<SetLock, kShards> setShards;
	std::array<SortSetLock,kShards> sortSetShards;
	std::array<PubSubLock, kShards> pubSubShards;

	xEventLoop loop;
	xTcpServer server;
	mutable MutexLock mutex;
	mutable MutexLock slaveMutex;
	mutable MutexLock expireMutex;
	std::string host;
	int32_t port;
	int32_t threadCount;
	std::string sentinelHost;
	std::atomic<int> sentinelPort;
	std::string  masterHost;
	std::atomic<int>    masterPort ;
	std::atomic<bool>  clusterEnabled;
	std::atomic<bool>  slaveEnabled;
	std::atomic<bool>  authEnabled;
	std::atomic<bool>  repliEnabled;
	std::atomic<bool>  sentinelEnabled;
	std::atomic<int>	salveCount;

	xBuffer		slaveCached;
	xReplication  repli;
	xSentinel	   senti;
	std::shared_ptr<std::thread > repliThreads;
	std::shared_ptr<std::thread > sentiThreads;
	std::map<int32_t,xTcpconnectionPtr> tcpconnMaps;
	std::map<int32_t,xTimer*> repliTimers;
	std::unordered_map<rObj*,xTimer*,Hash,Equal> expireTimers;
	xSocket socket;
	std::string password;
	std::atomic<int64_t >  count ;
	std::atomic<bool>	pingPong;
};


