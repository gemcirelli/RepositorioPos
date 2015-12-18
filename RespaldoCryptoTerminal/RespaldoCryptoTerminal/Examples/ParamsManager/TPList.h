/*******************************************************************************
 * �ļ�λ��: TPList.h
 * �ļ�˵��: ͨ��������API �Ķ���
 * ��������: 2012��02��24��
 * ������        : ��ҫȪ 
 * �޸�����:
 * �޸�˵��:
 * �޸�����:
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
 * �� �� tp_list_alloc() ������������������
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_alloc          (void);
/**
 * �� �� tp_list_free() �����������ͷ�����
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
void     tp_list_free           (TPList            *list);
void     tp_list_free_1         (TPList            *list);
/**
 * �� �� tp_list_append() �������������������
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_append         (TPList            *list,
                void *          data);
/**
 * �� �� tp_list_prepend() ����������ǰ��������
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_prepend        (TPList            *list,
                void *          data);
/**
 * �� �� tp_list_insert() ������������������
 *@return 
 * @author  ��ҫȪ
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
 * �� �� tp_list_insert_before() ������������������
 *@return 
 * @author  ��ҫȪ
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
 * �� �� tp_list_remove() ����������ɾ����ͬ���ݵ�����
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_remove         (TPList            *list,
                tpconstpointer     data);
/**
 * �� �� tp_list_remove_all() ����������ɾ����ͬ���ݵ�����
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_remove_all     (TPList            *list,
                tpconstpointer     data);
/**
 * �� �� _tp_list_remove_link() ����������ɾ��������
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_remove_link    (TPList            *list,
                TPList            *llink);
/**
 * �� �� tp_list_delete_link() ����������ɾ��������
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_delete_link    (TPList            *list,
                TPList            *link_);
/**
 * �� �� tp_list_reverse() �����������ߵ�����
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_reverse        (TPList            *list);
/**
 * �� �� tp_list_copy() ������������������
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_copy           (TPList            *list);
/**
 * �� �� tp_list_nth() �������������ָ����ŵ�����
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_nth            (TPList            *list,
                uint32             n);
/**
 * �� �� tp_list_nth_data() �������������ָ����ŵ������ǰ���ڵ�
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_nth_prev       (TPList            *list,
                uint32             n);
/**
 * �� �� tp_list_find() ������������ѯ�ڵ���������
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_find           (TPList            *list,
                tpconstpointer     data);
/**
 * �� �� tp_list_find_custom() �����������Զ������
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
 TPList*   tp_list_find_custom    (TPList            *list,
                tpconstpointer     data,
                TPCompareFunc      func);
/**
 * �� �� tp_list_position() �����������������
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
int32     tp_list_position       (TPList            *list,
                TPList            *llink);
/**
 * �� �� tp_list_index() �����������ڵ�����
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
int32     tp_list_index          (TPList            *list,
                tpconstpointer     data);
/**
 * �� �� tp_list_last() ��������������βָ��
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_last           (TPList            *list);
/**
 * �� �� tp_list_first() ����������������ָ��
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*   tp_list_first          (TPList            *list);
/**
 * �� �� tp_list_length() ����������������
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
uint32    tp_list_length         (TPList            *list);
/**
 * �� �� tp_list_foreach() ������������ѯ�ڵ㴦��
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
void     tp_list_foreach        (TPList            *list,
                TPFunc             func,
                void *          user_data);
/**
 * �� �� tp_list_sort_with_data() ��������������
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * �����û��ṩ�����ݱ�Ϻ�������
 */
 TPList*   tp_list_sort           (TPList            *list,
                TPCompareFunc      compare_func);
TPList*   tp_list_sort_with_data (TPList            *list,
                TPCompareDataFunc  compare_func,
                void *          user_data);
/**
 * �� �� tp_list_nth_data() �������������ָ����ŵ����������
 *@return 
 * @author  ��ҫȪ
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
