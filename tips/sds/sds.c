#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "sds.h"
#include "zmalloc.h"




sds sdsnewlen(const void *init, size_t initlen){

	struct sdshdr *sh;

	if(init){
		sh = zmalloc(sizeof(struct sdshdr) + initlen + 1);
	}
	else{
		sh = zcalloc(sizeof(struct sdshdr) + initlen + 1);
	}

	if(sh == NULL) return NULL;

	sh->len = initlen;
	sh->free = 0;

	if(init && initlen)
		memcpy(sh->buf, init, initlen);

	return (char *)sh->buf;
}

sds sdsnew(const char *init){
	size_t initlen = (init == NULL) ? 0 : strlen(init);
	return sdsnewlen(init, initlen);
}

sds sdsempty(void){
	return sdsnewlen("", 0);
}

sds sdsdup(const sds s){
	return sdsnewlen(s, sdslen(s));
}

void sdsfree(sds s){
	if(s == NULL) return;
	zfree(s- sizeof(struct sdshdr));
}

void sdsupdatelen(sds s){
	struct sdshdr *sh = (void*)(s - (sizeof(struct sdshdr)));
	int reallen = strlen(s);
	sh->free += (sh->len - reallen);
	sh->len = reallen;
}

void sdsclear(sds s){
	struct sdshdr *sh = (void*)(s - (sizeof(struct sdshdr)));

	sh->free += sh->len;
	sh->len = 0;

	sh->buf[0] = '\0';
}

sds sdsMakeRoomFor(sds s, size_t addlen){
	struct sdshdr *sh, *newsh;

	size_t free = sdsavail(s);

	size_t len, newlen;

	if(free >= addlen) return s;

	len = sdslen(s);
	sh = (void*)(s - (sizeof(struct sdshdr)));

	newlen = (len + addlen);

	if(newlen < SDS_MAX_PREALLOC)
		newlen *= 2;
	else
		newlen += SDS_MAX_PREALLOC;

	newsh = zrealloc(sh, sizeof(struct sdshdr) + newlen + 1);

	if(newsh == NULL) return NULL;

	newsh->free = newlen - len;

	return newsh->buf;
}

sds sdsRemoveFreeSpace(sds){
	struct sdshdr *sh = (void*) (s-(sizeof(struct sdshdr)));

	sh = zrealloc(sh, sizeof(struct sdshdr) + sh->len + 1);
	sh->free = 0;

	return (char *)sh->buf;
}

size_t sdsAllocSize(sds s){
	struct sdshdr *sh = (void*) (s-(sizeof(struct sdshdr)));

	return sizeof(*sh) + sh->len + sh->free + 1;
	//return sizeof(struct sdshdr) + sh->len + sh->free +1;
}

void sdsIncreLen(sds s, int incr){
	struct sdshdr *sh = (void*) (s-(sizeof(struct sdshdr)));

	assert(sh->free >= incr);

	sh->len += incr;
	sh->free -= incr;
	sh->buf[len] = '\0' 
}


sds sdsgrowzero(sds s, size_t len){
	struct sdshdr *sh = (void*)(s - (sizeof(struct sdshdr)));
	size_t totlen,curlen = sh->len;

	if(len <= curlen) return s;

	s = sdsMakeRoomFor(s, len - curlen);

	if(s == NULL) return NULL;

	sh = (void*)(s - (sizeof(struct sdshdr)));
	memset(s + curlen, 0, len - curlen + 1);

	totlen = sh->free + sh->len;
	sh->len = len;
	sh->free = totlen - sh->len;
}

sds sdscatlen(sds s, const void *t, size_t len){
	struct sdshdr *sh;

	size_t curlen = sdslen(s);

	s = sdsMakeRoomFor(s, len);

	if(s == NULL) return NULL;

	memcpy(s + curlen, t, len);

	sh = (void*) (s-(sizeof(struct sdshdr)));

	sh->len = curlen + len;
	sh->free = sh->free - len;

}

sds sdscat(sds s, const char *t){
	return sdscatlen(s, t, strlen(t));
}

sds sdscatsds(sds s, const sds t){
	return sdscatlen(s, t, sdslen(t));
}

sds sdscpylen(sds s, const char *t, size_t len){
	struct sdshdr *sh = (void*)(s - (sizeof(struct sdshdr)));

	size_t totlen = sh->free + sh->len;

	if(totlen < len){
		s = sdsMakeRoomFor(s, len - sh->len);
		if(s == NULL) return NULL;

		sh = (void*)(s - (sizeof(struct sdshdr)));
		totlen = sh->free + sh->len;
	}

	memcpy(s, t, len);

	s[len] = '\0';
	sh->len = len;
	sh->free = totlen - len;

	return s;
}

sds sdscpy(sds s, const char *t){
	return sdscpylen(s, t, strlen(t));
}

/*
 *将long long 类型转换为字符串
 */
#define SDS_LLSTR_SIZE 21
int sdsll2str(char *s, long long value){
	char *p, aux;
	unsigned long long v;
	size_t len;

	v = (value < 0) ? -value ： value;
	p = s;
	do{
		*p++ = '0' + (value%10);
		v /= 10;
	}while(v)

	len = p - s;
	*p = '\0';

	/*逆置字符串*/
	while(s < p){
		aux = *s;
		*s = *p;
		*p = aux;
		s++;
		p--;
	}
	return len;
}

int sdsull2str(char *s, unsigned long long v) {
    char *p, aux;
    size_t l;

    /* Generate the string representation, this method produces
     * an reversed string. */
    p = s;
    do {
        *p++ = '0'+(v%10);
        v /= 10;
    } while(v);

    /* Compute length and add null term. */
    l = p-s;
    *p = '\0';

    /* Reverse the string. */
    p--;
    while(s < p) {
        aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }
    return l;
}

sds sdsfromlonglong(long long value){
	char buf[SDS_LLSTR_SIZE];
	int len = sdsll2str(buf, value);

	return sdsnewlen(buf, len);
}

sds sdscatvprintf(sds s, const char *fmt, va_list ap){
	va_list cpy;
	char staticbuf[1024], *buf = staticbuf, *t;
	size_t buflen = strlen(fmt) * 2;

	if(buflen > sizeof(staticbuf)){
		buf = zmalloc(buflen);
		if(buf == NULL) return NULL;
	}else{
		buflen = sizeof(staticbuf);
	}

	while(1){
		buf[buflen - 2] = '\0';
		va_copy(cpy, ap);
		vsnprintf(buf, buflen, fmt, cpy);
		if(buf[buflen - 2] != '\0'){
			if(buf != staticbuf) zfree(buf);
			buflen *= 2;
			buf = zmalloc(buflen);
			if(buf == NULL) return NULL;
			continue;
		}
	}

	t = sdscat(s, buf);
	if(buf != staticbuf) zfree(buf);
	return t;
}

sds sdscatprintf(sds s, const char *fmt, ...){
	va_list ap;
	char *t;
	va_start(ap, fmt);

	t = sdscatvprintf(s, fmt, ap);
	va_end(ap);
	return t;
}

sds sdscatfmt(sds s, char const *fmt, ...){
	struct sdshdr *sh = (void*) (s-(sizeof(struct sdshdr)));
	size_t initlen = sdslen(s);
	const char *f = fmt;
	int i;
	va_list ap;

	va_start(ap, fmt);
	f = fmt;
	i = initlen;
	while(*f){
		char next, *str;
		size_t l;
		long long num;
		unsigned long long unum;

		if(sh->free == 0){
			s = sdsMakeRoomFor(s, 1);
			sh = (void*) (s-(sizeof(struct sdshdr)));
		}

		switch(*f){
		case '%':
			next = *(f + 1);
			f++;
			switch(next){
			case 's':
			case 'S':
				str = va_arg(ap, char*);
				l = (next == 's') ? strlen(str) : sdslen(str);
				if(sh->free < l){
					s = sdsMakeRoomFor(s, l);
					sh = (void*)(s-(sizeof(struct sdshdr)));
				}
				memcpy(s + i, str, l);
				sh->len += l;
				i += l;
				break;
			case 'i':
			case 'I':
				if(next == 'i')
					num = va_arg(ap, int);
				else
					num = va_arg(ap, long long);
                {
                    char buf[SDS_LLSTR_SIZE];
                    l = sdsll2str(buf,num);
                    if (sh->free < l) {
                        s = sdsMakeRoomFor(s,l);
                        sh = (void*) (s-(sizeof(struct sdshdr)));
                    }
                    memcpy(s+i,buf,l);
                    sh->len += l;
                    sh->free -= l;
                    i += l;
                }
                break;
            case 'u':
            case 'U':
            	if(next == 'u')
            		unum = va_arg(ap, unsigned int);
            	else
            		unum = va_arg(ap, unsigned long long);
                {
                    char buf[SDS_LLSTR_SIZE];
                    l = sdsull2str(buf,unum);
                    if (sh->free < l) {
                        s = sdsMakeRoomFor(s,l);
                        sh = (void*) (s-(sizeof(struct sdshdr)));
                    }
                    memcpy(s+i,buf,l);
                    sh->len += l;
                    sh->free -= l;
                    i += l;
                }
                break;
            default :
            	s[i++] = next;
            	sh->len += 1;
            	sh->free -= 1; 
            	break;   	           		    			
			}
            s[i++] = *f;
            sh->len += 1;
            sh->free -= 1;
            break;			
		}
        f++;
	}
	va_end(ap);

	s[i] = '\0';
    return s;
}