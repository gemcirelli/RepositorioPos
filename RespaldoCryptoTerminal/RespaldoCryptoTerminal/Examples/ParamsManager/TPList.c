/*******************************************************************************
 * �ļ�λ��: TPList.c
 * �ļ�˵��: i�o��ia��i��ʵ��,֧���̰߳�ȫ
 * �������u: 2012��02��25��
 * ������        : ��ҫȪ 
 * �޸����u:
 * �޸�˵��:
 * �޸����u:
 ******************************************************************************/


#include "TPLib.h"
#include "TPList.h"
#include "TPTypedef.h"


#define _tp_list_alloc tp_list_alloc

/**
 * �� �� tp_list_alloc() ����������e���a��i
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList* tp_list_alloc (void)
{
    TPList *list;
    #ifndef TP_LIBS
    list = tp_new0 (TPList, 1);
    #else
    tp_new0 (sizeof(TPList)*1, (void**)&list);
    #endif
    return list;
}

/**
 * �� �� tp_list_free() �����������i��a��i
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
void tp_list_free (TPList *list)
{
    TPList *last;
  
    while (list != PNULL)
    {
        last = list;
        list = list->next;
        #ifndef TP_LIBS
        free(last);
        #else
        tp_free ((void**)&last);
        #endif  
    }
}

#define _tp_list_free_1 tp_list_free_1

/**
 * �� �� tp_list_free() �����������i��a��i
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
void tp_list_free_1 (TPList *list)
{
    #ifndef TP_LIBS
    free(list);
    #else
    tp_free ((void**)&list);
    #endif  
}

/**
 * �� �� tp_list_append() �����������o����a��i
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList* tp_list_append (TPList  *list,
           void *    data)
{
    TPList *new_list;
    TPList *last;
  
    new_list = _tp_list_alloc ();
    if(new_list == NULL)
        return list;
    
    new_list->data = data;
  
    if (list != PNULL)
    {
        last = tp_list_last (list);
        /* g_assert (last != NULL); */
        last->next = new_list;
        new_list->prev = last;

        return list;
    }
    else
    {
        return new_list;
    }
}

/**
 * �� �� tp_list_prepend() ����������ǰ����a��i
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList* tp_list_prepend (TPList  *list,
        void *  data)
{
    TPList *new_list;
  
    new_list = _tp_list_alloc ();
    new_list->data = data;
  
    if (list != PNULL)
    {
        if (list->prev)
        {
            list->prev->next = new_list;
            new_list->prev = list->prev;
        }
        list->prev = new_list;
        new_list->next = list;
    }
  
    return new_list;
}

/**
 * �� �� tp_list_insert() ��������������a��i
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList* tp_list_insert (TPList  *list,
           void *    data,
           int32     position)
{
    TPList *new_list;
    TPList *tmp_list;
  
    if (position < 0)
        return tp_list_append (list, data);
    else
        if (position == 0)
            return tp_list_prepend (list, data);
  
    tmp_list = tp_list_nth (list, position);
    if (!tmp_list)
        return tp_list_append (list, data);
  
    new_list = _tp_list_alloc ();
    new_list->data = data;
  
    if (tmp_list->prev)
    {
        tmp_list->prev->next = new_list;
        new_list->prev = tmp_list->prev;
    }
    new_list->next = tmp_list;
    tmp_list->prev = new_list;

    if (tmp_list == list)
        return new_list;
    else
        return list;
}

/**
 * �� �� tp_list_insert_before() ��������������a��i
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList* tp_list_insert_before (TPList   *list,
              TPList   *sibling,
              void * data)
{
    if (!list)
    {
        list = tp_list_alloc ();
        if(list != NULL)
        {
            list->data = data;
            tp_return_val_if_fail (sibling == NULL, list);
        }
        return list;
    }
    else if (sibling != PNULL)
    {
        TPList *node;

        node = tp_list_alloc ();
        node->data = data;
        if (sibling->prev)
        {
            node->prev = sibling->prev;
            node->prev->next = node;
            node->next = sibling;
            sibling->prev = node;
            return list;
        }
        else
        {
            node->next = sibling;
            sibling->prev = node;
            tp_return_val_if_fail (sibling == list, node);
            return node;
        }
    }
    else
    {
        TPList *last;

        last = list;
        while (last->next != PNULL)
            last = last->next;

        last->next = tp_list_alloc ();
        last->next->data = data;
        last->next->prev = last;

        return list;
    }
}


TPList *tp_list_concat (TPList *list1, TPList *list2)
{
    TPList *tmp_list;
  
    if (list2 != PNULL)
    {
        tmp_list = tp_list_last (list1);
        if (tmp_list != PNULL)
            tmp_list->next = list2;
        else
            list1 = list2;
        list2->prev = tmp_list;
    }
  
    return list1;
}

/**
 * �� �� tp_list_remove() ����������e�����i����ݵ�a��i
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*
tp_list_remove (TPList       *list,
           tpconstpointer  data)
{
    TPList *tmp;
  
    tmp = list;
    while (tmp != PNULL)
    {
        if (tmp->data != data)
            tmp = tmp->next;
        else
        {
            if (tmp->prev != PNULL)
                tmp->prev->next = tmp->next;
            if (tmp->next != PNULL)
                tmp->next->prev = tmp->prev;

            if (list == tmp)
                list = list->next;
      
            _tp_list_free_1 (tmp);
      
            break;
        }
    }
    return list;
}
/**
 * �� �� tp_list_remove_all() ����������e�����i����ݵ�a��i
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList* tp_list_remove_all (TPList  *list,
           tpconstpointer data)
{
    TPList *tmp = list;

    while (tmp != PNULL)
    {
        if (tmp->data != data)
            tmp = tmp->next;
        else
        {
            TPList *next = tmp->next;

            if (tmp->prev != PNULL)
                tmp->prev->next = next;
            else
                list = next;
            if (next != PNULL)
                next->prev = tmp->prev;

            _tp_list_free_1 (tmp);
            tmp = next;
        }
    }
    return list;
}
/**
 * �� �� _tp_list_remove_link() ����������e����oa��i
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
static __inline TPList*
_tp_list_remove_link (TPList *list,
             TPList *link)
{
    if (link != PNULL)
    {
        if (link->prev != PNULL)
            link->prev->next = link->next;
        if (link->next != PNULL)
            link->next->prev = link->prev;
      
        if (link == list)
            list = list->next;
      
        link->next = NULL;
        link->prev = NULL;
    }
  
    return list;
}
/**
 * �� �� tp_list_remove_link() ����������e����oa��i
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*
tp_list_remove_link (TPList *list,
            TPList *link)
{
    return _tp_list_remove_link (list, link);
}

/**
 * �� �� tp_list_delete_link() ����������e����oa��i
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*
tp_list_delete_link (TPList *list,
            TPList *link)
{
    list = _tp_list_remove_link (list, link);
    _tp_list_free_1 (link);

    return list;
}
/**
 * �� �� tp_list_copy() ��������������a��i
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*
tp_list_copy (TPList *list)
{
    TPList *new_list = NULL;

    if (list != PNULL)
    {
        TPList *last;

        new_list = _tp_list_alloc ();
        new_list->data = list->data;
        last = new_list;
        list = list->next;
        while (list != PNULL)
        {
            last->next = _tp_list_alloc ();
            last->next->prev = last;
            last = last->next;
            last->data = list->data;
            list = list->next;
        }
    }

    return new_list;
}

/**
 * �� �� tp_list_reverse() ��������������a��i
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*
tp_list_reverse (TPList *list)
{
    TPList *last;

    last = NULL;
    while (list != PNULL)
    {
        last = list;
        list = last->next;
        last->next = last->prev;
        last->prev = list;
    }
  
    return last;
}
/**
 * �� �� tp_list_nth() �����������n��ָ����ŵ�a��i
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*
tp_list_nth (TPList *list,
        uint32  n)
{
    while ((n-- > 0) && list)
        list = list->next;
  
    return list;
}

/**
 * �� �� tp_list_nth_data() �����������n��ָ����ŵ�a��i��ǰ���u��
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList* tp_list_nth_prev (TPList *list,
         uint32  n)
{
    while ((n-- > 0) && list)
        list = list->prev;
  
    return list;
}

/**
 * �� �� tp_list_nth_data() �����������n��ָ����ŵ�a��i������
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
void *
tp_list_nth_data (TPList     *list,
         uint32      n)
{
    while ((n-- > 0) && list != PNULL)
        list = list->next;
  
    return list ? list->data : NULL;
}

/**
 * �� �� tp_list_find() �����������en��u�����ua��i
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList* tp_list_find (TPList         *list,
         tpconstpointer  data)
{
    while (list != PNULL)
    {
        if (list->data == data)
            break;
        list = list->next;
    }
  
    return list;
}

/**
 * �� �� tp_list_find_custom() �����������Զ���e��
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
 TPList*
tp_list_find_custom (TPList         *list,
            tpconstpointer  data,
            TPCompareFunc   func)
{
    tp_return_val_if_fail (func != NULL, list);

    while (list != PNULL)
    {
        if (! func (list->data, data))
            return list;
        list = list->next;
    }

    return NULL;
}

/**
 * �� �� tp_list_position() ����������a��i���
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
int32 tp_list_position (TPList *list,
         TPList *link)
{
    int32 i;

    i = 0;
    while (list != PNULL)
    {
        if (list == link)
            return i;
        i++;
        list = list->next;
    }

    return -1;
}
/**
 * �� �� tp_list_index() �����������u������
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
int32 tp_list_index (TPList         *list,
          tpconstpointer  data)
{
    int32 i;

    i = 0;
    while (list != PNULL)
    {
        if (list->data == data)
            return i;
        i++;
        list = list->next;
    }

    return -1;
}

/**
 * �� �� tp_list_last() ����������a��iβָ��
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList* tp_list_last (TPList *list)
{
    if (list != PNULL)
    {
        while (list->next != PNULL)
            list = list->next;
    }
  
    return list;
}
/**
 * �� �� tp_list_first() ����������a��i��ָ��
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList* tp_list_first (TPList *list)
{
    if (list != PNULL)
    {
        while (list->prev != PNULL)
            list = list->prev;
    }
  
    return list;
}
/**
 * �� �� tp_list_length() ����������a��i����
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
uint32 tp_list_length (TPList *list)
{
    uint32 length;
  
    length = 0;
    while (list != PNULL)
    {
        length++;
        list = list->next;
    }
  
    return length;
}
/**
 * �� �� tp_list_foreach() ������������n��u�㴦�i
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
void
tp_list_foreach (TPList  *list,
        TPFunc    func,
        void *  user_data)
{
    while (list != PNULL)
    {
        TPList *next = list->next;
        (*func) (list->data, user_data);
        list = next;
    }
}

/**
 * �� �� tp_list_insert_sorted() ������������������
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList*
tp_list_insert_sorted (TPList        *list,
                      void *      data,
                      TPCompareFunc  func)
{
    TPList *tmp_list = list;
    TPList *new_list;
    int32 cmp;

    tp_return_val_if_fail (func != NULL, list);
  
    if (!list) 
    {
        new_list = _tp_list_alloc ();
        new_list->data = data;
        return new_list;
    }
  
    cmp = (*func) (data, tmp_list->data);
  
    while ((tmp_list->next) && (cmp > 0))
    {
        tmp_list = tmp_list->next;
        cmp = (*func) (data, tmp_list->data);
    }

    new_list = _tp_list_alloc ();
    new_list->data = data;

    if ((!tmp_list->next) && (cmp > 0))
    {
        tmp_list->next = new_list;
        new_list->prev = tmp_list;
        return list;
    }
   
    if (tmp_list->prev)
    {
    tmp_list->prev->next = new_list;
    new_list->prev = tmp_list->prev;
    }
    new_list->next = tmp_list;
    tmp_list->prev = new_list;
 
    if (tmp_list == list)
        return new_list;
    else
        return list;
}
/**
 * �� �� tp_list_sort_merge() �����������e������
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
static TPList *
tp_list_sort_merge (TPList     *l1, 
           TPList     *l2,
           TPFunc     compare_func,
           tpboolean  use_data,
           void *  user_data)
{
    TPList list, *l, *lprev;
    int32 cmp;

    l = &list; 
    lprev = NULL;

    while (l1 && l2)
    {
        if (use_data)
            cmp = ((TPCompareDataFunc) compare_func) (l1->data, l2->data, user_data);
        else
            cmp = ((TPCompareFunc) compare_func) (l1->data, l2->data);

        if (cmp <= 0)
        {
            l->next = l1;
            l = l->next;
            l->prev = lprev; 
            lprev = l;
            l1 = l1->next;
        } 
        else 
        {
            l->next = l2;
            l = l->next;
            l->prev = lprev; 
            lprev = l;
            l2 = l2->next;
        }
    }
    l->next = l1 ? l1 : l2;
    l->next->prev = l;

    return list.next;
}


static TPList* 
tp_list_sort_real (TPList    *list,
          TPFunc     compare_func,
          tpboolean  use_data,
          void *  user_data)
{
    TPList *l1, *l2;
  
    if (!list) 
        return NULL;
    if (!list->next) 
        return list;
  
    l1 = list; 
    l2 = list->next;

    while ((l2 = l2->next) != NULL)
    {
        if ((l2 = l2->next) == NULL) 
            break;
        l1 = l1->next;
    }
    l2 = l1->next; 
    l1->next = NULL; 

    return tp_list_sort_merge (tp_list_sort_real (list, compare_func, use_data, user_data),
                tp_list_sort_real (l2, compare_func, use_data, user_data),
                compare_func,
                use_data,
                user_data);
}
/**
 * �� �� tp_list_sort_with_data() ��������������
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * ����oû��a�������ݱi�Ϻ�������
 */
TPList *
tp_list_sort (TPList        *list,
         TPCompareFunc  compare_func)
{
    return tp_list_sort_real (list, (TPFunc) compare_func, FALSE, NULL);                
}
/**
 * �� �� tp_list_sort_with_data() ������������ouoû���������
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 * 
 */
TPList *
tp_list_sort_with_data (TPList            *list,
               TPCompareDataFunc  compare_func,
               void *          user_data)
{
    return tp_list_sort_real (list, (TPFunc) compare_func, TRUE, user_data);
}


