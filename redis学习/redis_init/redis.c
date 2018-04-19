#include "redis.h"


/*=============================== Function declariton =======================*/
void setCommand(redisClient *c);
void getCommand(redisClient *c);
/*================================= Globals ================================= */

/* Global vars */
struct redisServer server;

struct redisCommand redisCommandTable[] = {
	{ "get", getCommand, 2, "r", 0, NULL, 1, 1, 1, 0, 0},
	{ "set", setCommand, -3, "wm", 0, NULL, 1, 1, 1, 0, 0},
};

/*================================ Dict ===================================== */


/*================================== Commands ================================ */

/*
 * 根据 redis.c 文件顶部的命令列表，创建命令表
 */
void populateCommandTable(void){
	int j;

	// 命令的数量
	int numcommands = sizeof(redisCommandTable) / sizeof(struct redisCommand);

	for(j = 0; j < numcommands; j++){

		//指定命令
		struct redisCommand *c = redisCommandTable + j;

		char *f = c->sflags;

		while(*f != '\0'){
			switch(f){
				case 'w': c->flags |= REDIS_CMD_WRITE; break;
				case 'r': c->flags |= REDIS_CMD_READONLY; break;
				case 'm': c->flags |= REDIS_CMD_DENYOOM; break;
				case 'a': c->flags |= REDIS_CMD_ADMIN; break;
				case 'p': c->flags |= REDIS_CMD_PUBSUB; break;
				case 's': c->flags |= REDIS_CMD_NOSCRIPT; break;
				case 'R': c->flags |= REDIS_CMD_RANDOM; break;
				case 'S': c->flags |= REDIS_CMD_SORT_FOR_SCRIPT; break;
				case 'l': c->flags |= REDIS_CMD_LOADING; break;
				case 't': c->flags |= REDIS_CMD_STALE; break;
				case 'M': c->flags |= REDIS_CMD_SKIP_MONITOR; break;
				case 'k': c->flags |= REDIS_CMD_ASKING; break;
				default:
					break;
			}
			f++;
		}

		// 将命令关联到命令表
		retval1 = dictAdd(server.commands, sdsnew(c->name), c);

		/*
		 * 将命令也关联到原始命令表
		 *
		 * 原始命令表不会受 redis.conf 中命令改名的影响
		 */
		retval2 = dictAdd(server.orig_commands, sdsnew(c->name), c);
	}
}


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
	server.commands = dictCreate(&commandTableDictType, NULL);
	server.orig_commands = dictCreate(&commandTableDictType, NULL);
	populateCommandTable();  // 安装命令处理函数

}


int main(int argc, char *argv[]){
	//初始化服务器
	initServerConfig();
	return 0;
}