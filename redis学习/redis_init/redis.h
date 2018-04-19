#ifndef __REDIS_H_
#define __REDIS_H_

#include <stdio.h>
#include <dict.h>
#include "ea.h"
#include "adlist.h"

/*协议和I/O相关的定义*/
#define REDIS_REPLY_CHUNK_BYTES	(16*1024) /* 16k 输出缓存*/
#define REDIS_MAX_CLIENTS 10000 /* 最大所支持的用户数目 */

/*服务器配置*/
#define REDIS_DEFAULT_HZ        10
#define REDIS_SERVERPORT		6379 /* TCP port */
#define REDIS_TCP_BACKLOG       511     /* TCP listen backlog */ 
#define REDIS_BINDADDR_MAX		16
#define REDIS_IP_STR_LEN INET6_ADDRSTRLEN
#define REDIS_DEFAULT_DBNUM     16
#define REDIS_DEFAULT_TCP_KEEPALIVE 0

/* 命令标志 */
#define REDIS_CMD_WRITE 1                   /* "w" flag */
#define REDIS_CMD_READONLY 2                /* "r" flag */
#define REDIS_CMD_DENYOOM 4                 /* "m" flag */
#define REDIS_CMD_NOT_USED_1 8              /* no longer used flag */
#define REDIS_CMD_ADMIN 16                  /* "a" flag */
#define REDIS_CMD_PUBSUB 32                 /* "p" flag */
#define REDIS_CMD_NOSCRIPT  64              /* "s" flag */
#define REDIS_CMD_RANDOM 128                /* "R" flag */
#define REDIS_CMD_SORT_FOR_SCRIPT 256       /* "S" flag */
#define REDIS_CMD_LOADING 512               /* "l" flag */
#define REDIS_CMD_STALE 1024                /* "t" flag */
#define REDIS_CMD_SKIP_MONITOR 2048         /* "M" flag */
#define REDIS_CMD_ASKING 4096               /* "k" flag */

#define REDIS_LRU_BITS 24

typedef struct redisObject{
	unsigned type : 4;	// 类型

	unsigned encoding : 4; //编码

	unsigned lru : REDIS_LRU_BITS; // 对象最后一次被访问的时间

	int refcount;	// 引用计数
 
	void *ptr;		// 指向实际值的指针
}robj;

typedef struct redisDb
{
	dict *dict;			// 数据库键空间，保存着数据库中的所有键值对

	dict *expires;		// 键的过期时间，字典的键为键，字典的值为过期事件

	dict *blocking_keys;// 正处于阻塞状态的键

	dict *ready_keys;	// 可以解除阻塞的键

	int id;				// 数据库号码
}redisDb;


typedef struct redisClient
{
	int fd;		// 套接字描述符

	redisDb *db; // 当前正在使用的数据库

	int dictid;	// 当前正在使用的数据库id

	robj *name; // 客户端的名字

	sds querbuf; // 查询缓冲区

	size_t querbuf_peak; // 查询缓冲区长度峰值

	int argc; // 参数总量

	robj ** argc; // 参数对象数组

	strcut redisCommand * cmd, *lastcmd;	// 记录被客户执行的命令

	int reqtype;	// 请求类型，是内联命令还是多条命令

	int multibulklen;	//剩余未读去的命令内容数量

	long bulklen; // 命令内容的长度

	list *reply;	// 回复链表

	unsigned long reply_bytes;	//回复链表中对象的总大小

	int bufpos;	//回复偏移量

	char buf[REDIS_RE]
}redisClient;

struct redisServer{

	char *configfile; 	// 配置文件的绝对路径，要么就是NULL

	int hz;				// serverCron()每秒调用的次数

	redisDb *db;		// 一个数组，保存着服务器

	dict *commands;		// 命令表（受到rename配置选项的作用）

	dict *orig_commands;// Command table before command renaming.

	aeEventLoop *el;	// 事件状态

	int shutdown_asap;	// 关闭服务器的标识

	int port;			// TCP 监听端口

	int tcp_backlog;	// TCP listen() backlog

	char *bindaddr[REDIS_BINDADDR_MAX];  // ip 地址

	int bindaddr_count; // 地址的数量

	int ipfd[REDIS_BINDADDR_MAX]; 		 // TCP 描述符

	int ipfd_count;		// 已使用了的描述符的数目

	list *clients;		// 一个链表，保存了所有的客户端状态结构

	list *clients_to_close; // 保存了所有待关闭的客户端

	redisClient *current_client; // 服务器当前服务的客户端，仅用于崩溃报告

	char neterr[ANET_ERR_LEN];	// 用于记录网络错误

	int tcpkeepalive;	// 是否开启 SO_KEEPALIVE选项
	int dbnum;			// 数据库的总数目

	int maxclients;		//最大并发数量
};


typedef void redisCommandProc(redisClient *c);
typedef int *redisGetKeyProc(struct redisCommand *cmd, robj **argv, int argc, int *numkeys);
/*
 * redisCommand redis命令
 */
struct redisCommand{
	
	char *name;		// 命令名称

	redisCommandProc *proc;		// 实现函数

	int arity;		// 参数个数

	char *sflags;	//字符串表示的FLAG

	redisGetKeyProc *getkeys_proc;	//从命令中判断命令的键参数。在redis集群转向时使用。

	int firstkey;
	int lastkey;
	int keystep;

	// 统计信息
	// microseconds 记录了命令执行耗费的总毫微秒数
	// calls 是命令被执行的总次数
	long long microseconds, calls;
};
#endif