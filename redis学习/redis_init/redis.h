#ifndef __REDIS_H_
#define __REDIS_H_

#include <stdio.h>
#include <dict.h>

typedf struct redisDb
{
	dict *dict;			// 数据库键空间，保存着数据库中的所有键值对

	dict *expires;		// 键的过期时间，字典的键为键，字典的值为过期事件

	dict *blocking_keys;// 正处于阻塞状态的键

	dict *ready_keys;	// 可以解除阻塞的键

	int id;				// 数据库号码
};

struct redisServer{

	char *configfile; 	// 配置文件的绝对路径，要么就是NULL

	int hz;				// serverCron()每秒调用的次数

	redisDb *db;		//一个数组，保存着服务器
};

#endif