/*******************************************************************************
 * 文件位置: TPList.h
 * 文件说明: 通用型链表API 的定义
 * 创建日期: 2012年02月24日
 * 创建人        : 吴耀泉 
 * 修改日期:
 * 修改说明:
 * 修改日期:
 ******************************************************************************/

#ifndef _LIST_H__
#define _LIST_H__


#undef  MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#undef  MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

typedef struct _TPList  TPList;

struct _TPList
{
  void * data;
  TPList *next;
  TPList *prev;
};

typedef const void *tpconstpointer;

typedef int32            (*TPCompareFunc)         (tpconstpointer  a,
                                                 tpconstpointer  b);
typedef int32            (*TPCompareDataFunc)     (tpconstpointer  a,
                                                 tpconstpointer  b,
                         tppointer       user_data);
typedef void            (*TPFunc)                (tppointer       data,
                                                 tppointer       user_data);

#ifdef __cplusplus
extern "C" {
#endif
                                                 
/* Doubly linked lists
 */
/*
  void     tp_list_push_allocator (GAllocator       *allocator);
  void     tp_list_pop_allocator  (void);
*/
/**
 * 函 数 tp_list_alloc() 功能描述：申请链表
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_alloc          (void);
/**
 * 函 数 tp_list_free() 功能描述：释放链表
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
void     tp_list_free           (TPList            *list);
void     tp_list_free_1         (TPList            *list);
/**
 * 函 数 tp_list_append() 功能描述：后插入链表
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_append         (TPList            *list,
                void *          data);
/**
 * 函 数 tp_list_prepend() 功能描述：前插入链表
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_prepend        (TPList            *list,
                void *          data);
/**
 * 函 数 tp_list_insert() 功能描述：插入链表
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_insert         (TPList            *list,
                void *          data,
                int32              position);
TPList*   tp_list_insert_sorted  (TPList            *list,
                void *          data,
                TPCompareFunc      func);
/**
 * 函 数 tp_list_insert_before() 功能描述：插入链表
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_insert_before  (TPList            *list,
                TPList            *sibling,
                void *          data);
TPList*   tp_list_concat         (TPList            *list1,
                TPList            *list2);
/**
 * 函 数 tp_list_remove() 功能描述：删除相同数据的链表
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_remove         (TPList            *list,
                tpconstpointer     data);
/**
 * 函 数 tp_list_remove_all() 功能描述：删除相同数据的链表
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_remove_all     (TPList            *list,
                tpconstpointer     data);
/**
 * 函 数 _tp_list_remove_link() 功能描述：删除子链表
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_remove_link    (TPList            *list,
                TPList            *llink);
/**
 * 函 数 tp_list_delete_link() 功能描述：删除子链表
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_delete_link    (TPList            *list,
                TPList            *link_);
/**
 * 函 数 tp_list_reverse() 功能描述：颠倒链表
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_reverse        (TPList            *list);
/**
 * 函 数 tp_list_copy() 功能描述：拷贝链表
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_copy           (TPList            *list);
/**
 * 函 数 tp_list_nth() 功能描述：获得指定序号的链表
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_nth            (TPList            *list,
                uint32             n);
/**
 * 函 数 tp_list_nth_data() 功能描述：获得指定序号的链表的前续节点
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_nth_prev       (TPList            *list,
                uint32             n);
/**
 * 函 数 tp_list_find() 功能描述：查询节点所在链表
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_find           (TPList            *list,
                tpconstpointer     data);
/**
 * 函 数 tp_list_find_custom() 功能描述：自定义查找
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
 TPList*   tp_list_find_custom    (TPList            *list,
                tpconstpointer     data,
                TPCompareFunc      func);
/**
 * 函 数 tp_list_position() 功能描述：链表序号
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
int32     tp_list_position       (TPList            *list,
                TPList            *llink);
/**
 * 函 数 tp_list_index() 功能描述：节点索引
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
int32     tp_list_index          (TPList            *list,
                tpconstpointer     data);
/**
 * 函 数 tp_list_last() 功能描述：链表尾指针
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_last           (TPList            *list);
/**
 * 函 数 tp_list_first() 功能描述：链表首指针
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_first          (TPList            *list);
/**
 * 函 数 tp_list_length() 功能描述：链表长度
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
uint32    tp_list_length         (TPList            *list);
/**
 * 函 数 tp_list_foreach() 功能描述：轮询节点处理
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
void     tp_list_foreach        (TPList            *list,
                TPFunc             func,
                void *          user_data);
/**
 * 函 数 tp_list_sort_with_data() 功能描述：排序
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 根据用户提供的数据表较函数排序
 */
 TPList*   tp_list_sort           (TPList            *list,
                TPCompareFunc      compare_func);
TPList*   tp_list_sort_with_data (TPList            *list,
                TPCompareDataFunc  compare_func,
                void *          user_data);
/**
 * 函 数 tp_list_nth_data() 功能描述：获得指定序号的链表的数据
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
 * 
 */
void    *tp_list_nth_data       (TPList            *list,
                uint32             n);


#ifdef __cplusplus
}
#endif

#define tp_list_previous(list)  ((list) ? (((TPList *)(list))->prev) : NULL)
#define tp_list_next(list)  ((list) ? (((TPList *)(list))->next) : NULL)


typedef TPList TPSList;
#define tp_slist_prepend tp_list_prepend
#define tp_slist_free tp_list_free
#define tp_slist_free_1  tp_list_free_1
#define tp_slist_reverse tp_list_reverse
#define tp_slist_next tp_list_next
#define tp_slist_foreach tp_list_foreach
#define tp_slist_length tp_list_length

/* to be continued...*/

#endif /* __G_LIST_H__ */
