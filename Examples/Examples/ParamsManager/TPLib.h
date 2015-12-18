/*******************************************************************************
 * 文件位置: TPLib.h
 * 文件说明: 不用平台的移植接口
 * 创建日期: 2012年02月23日
 * 创建人        : 吴耀泉 
 * 修改日期:
 * 修改说明:
 * 修改日期:
 ******************************************************************************/

#ifndef TPLIB_H
#define TPLIB_H


#include <stdlib.h>
#include <stdio.h>
//#include <unistd.h>
//#include <errno.h>
//#include <sys/types.h>
//#include <stdarg.h>
#include <string.h>

#define TP_LIBS
#ifndef TP_LIBS
/* integer types */
typedef  uint64_t uint64;
typedef  int64_t int64;
typedef  uint16_t uint16;
typedef  uint32_t uint32;
typedef  signed short int16;
typedef  int32_t int32;
typedef  unsigned int uint;
typedef  int int;
typedef  char char;
typedef  unsigned char uchar;
typedef  unsigned char uint8;
typedef  void* pointer;
typedef int boolean;
typedef double double;
typedef int size;
typedef ssize_t ssize;

#define TRUE 1
#define FALSE 0

#else
#include "TPTypedef.h"

typedef  void* tppointer;
typedef int tpboolean;

#endif

#ifdef __cplusplus
#define TP_BEGIN_DECLS extern"C"{
#define TP_END_DECLS }
#else
#define TP_BEGIN_DECLS 
#define TP_END_DECLS 
#endif



typedef char TPError;
TPError *tp_error_new(int32 dummy1, int32 dummy2, const char* errormsg,...);
void    tp_set_error(TPError **err,int32 dummy1,int32 dummy2,const char *format,...);

/*misc*/
#define tp_return_if_fail(expr) if (!(expr)) {printf("%s:%i- assertion" #expr "failed\n",__FILE__,__LINE__); return;}
#define tp_return_val_if_fail(expr,ret) if (!(expr)) {printf("%s:%i- assertion #expr failed\n",__FILE__,__LINE__); return (ret);}
#define tp_assert(expr) if (!(expr)) printf("%s:%i- assertion #expr failed\n",__FILE__,__LINE__)
#define tp_assert_not_reached() printf("%s:%i- assert not reached failed\n",__FILE__,__LINE__)


//#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>


/* memory allocation functions */
#ifndef TP_LIBS

#define tp_malloc(sz)  malloc(sz)

static __inline void * tp_malloc0(int sz)
{
    void *p=malloc(sz);
    memset(p,0,sz);
    return p;
}

#define tp_alloca(s)    alloca(s)
#define tp_new(type,count)   (type *)tp_malloc(sizeof(type)*(count))
#define tp_new0(type, count)    (type *)tp_malloc0(sizeof(type)*(count))
#define tp_realloc(p,sz) realloc((p),(sz))
#define tp_free(p) free(p)

#else

#include "TPTypedef.h"
#include "TPBase.h"
#define tp_malloc(sz)  malloc(sz)

static __inline void * tp_malloc0(uint32 sz, void** mem_pp)
{
/*
    void *p=malloc(sz);
    memset(p,0,sz);
    return p;
*/
    TP_AllocMemory(sz, mem_pp);
    if((uint32)(*mem_pp)>0)
        memset(*mem_pp, 0, sz); 
    return *mem_pp;
}

#define tp_alloca(s)    alloca(s)
//#define tp_new(sz, mem_pp)   tp_malloc(sz, mem_pp)
#define tp_new0(sz, mem_pp)  tp_malloc0(sz, mem_pp)
//#define tp_realloc(p,sz) realloc((p),(sz))
#define tp_free(mem_pp) TP_FreeMemory(mem_pp)


//String

static __inline void*tp_strdup(const char* srcdata, void** mem_pp)
{
    if(!srcdata)
        return NULL;
    
    tp_new0(strlen((char *)srcdata)+1, mem_pp);
    
    if(*mem_pp)
    {
        strncpy(*mem_pp, srcdata, strlen((char *)srcdata));
        return *mem_pp;
    }
    else
    {
        return NULL;
    }

}

#endif

#endif

