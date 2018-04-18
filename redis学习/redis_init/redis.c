#include "redis.h"

struct redisServer server;

struct redisCommand redisCommandTable[] = {
	{ "get", getCommand, 2, "r", 0, NULL, 1, 1, 1, 0, 0},
	{ "set", setCommand, -3, "wm", 0, NULL, 1, 1, 1, 0, 0},
};

void initServerConfig(){
	int j;

	//服务器状态
	server.hz = REDIS_DEFAULT_HZ;
	server.port = REDIS_SERVERPORT;
	server.tcp_backlog = REDIS_TCP_BACKLOG;
	server.bindaddr_count = 0;

	server.maxclients = REDIS_MAX_CLIENTS;
	server.ipfd_count = 0;
	server.dbnum = REDIS_DEFAULT_DBNUM;
	server.tcpkeepalive = REDIS_DEFAULT_TCP_KEEPALIVE;


}

/*================================== Commands ================================ */

/*
 * 根据 redis.c 文件顶部的命令列表，创建命令表
 */
void populateCommandTable(void){
	int j;

	// 命令的数量
	int num
}

int main(int argc, char *argv[]){
	//初始化服务器
	initServerConfig();
	return 0;
}