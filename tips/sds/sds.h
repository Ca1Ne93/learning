#ifndef __SDS_H
#define __SDS_H

/*
 * 最大预分配长度
 */
#define SDS_MAX_PREALLOC (1024*1024)

#include <sys/types.h>
#include <stdarg.h>


typedef char *sds;

struct sdshdr{
	
	//buf中已占用空间的长度 
	int len;

    //buf中未使用空间的长度
	int free;
	
	//存放数据
	char buf[];
};

/*
 *
 */
static inline size_t sdslen(const sds s){
	struct sdshdr *sh = (void*)(s - (sizeof(struct sdshdr)));
	return sh->len;
}

/*
 *
 */
static inline size_t sdsavail(const sds s){
	struct sdshdr *sh = (void*)(s - (sizeof(struct sdshdr)));
	return sh->free;
}

sds sdsnewlen(const void *init, size_t initlen);
sds sdsnew(const char *init);
sds sdsempty(void);
size_t sdslen(const sds s);
sds sdsdup(const sds s);
size_t sdsavail(const sds s);
sds sdsgrowzeor(sds s, size_t len);