/*
 * Copyright (c) 2018, Billie Soong <nonkr@hotmail.com>
 * All rights reserved.
 *
 * This file is under MIT, see LICENSE for details.
 *
 * Author: Billie Soong <nonkr@hotmail.com>
 * Datetime: 2018/3/17 14:28
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"

Frlist_header *frlist_create(void)
{
    Frlist_header *list = (Frlist_header *) malloc(sizeof(Frlist_header));
    if (list == NULL)
        return NULL;

    list->size = 0;
    list->head = list->tail = NULL;
    return list;
}

void frlist_header_init(Frlist_header *p_st_hdr)
{
    if (!p_st_hdr)
        return;

    p_st_hdr->size = 0;
    p_st_hdr->head = p_st_hdr->tail = NULL;
}

unsigned int frlist_size(const Frlist_header *p_st_hdr)
{
    if (p_st_hdr == NULL)
        return 0;

    return p_st_hdr->size;
}

void frlist_node_data_free(Frlist_node *p_st_node)
{
    if (p_st_node == NULL)
        return;
    if (p_st_node->data)
    {
        free(p_st_node->data);
        p_st_node->data = NULL;
    }

    if (p_st_node->p_key)
    {
        free(p_st_node->p_key);
        p_st_node->l_key      = 0;
        p_st_node->p_key      = NULL;
        p_st_node->ui_key_len = 0;
    }
}

Frlist_node *frlist_node_create(void *p_data)
{
    Frlist_node *p_node = (Frlist_node *) malloc(sizeof(Frlist_node));
    if (p_node == NULL)
        return NULL;

    memset(p_node, 0x00, sizeof(Frlist_node));
    p_node->data = p_data;
    return p_node;
}

int frlist_empty(Frlist_header *p_st_header)
{
    if (p_st_header == NULL)
        return 0;

    if (p_st_header->head == NULL)
        return 0;

    return 1;
}

void frlist_add_tail(Frlist_header *p_st_header, Frlist_node *p_node_add)
{
    if (p_st_header == NULL || p_node_add == NULL)
        return;

    p_node_add->next = NULL;

    if (p_st_header->head)
    {
        p_node_add->pre         = p_st_header->tail;
        p_st_header->tail->next = p_node_add;
        p_st_header->tail       = p_node_add;

    }
    else
    {
        p_node_add->pre   = NULL;
        p_st_header->head = p_st_header->tail = p_node_add;
    }

    p_st_header->size++;
}

void frlist_add_header(Frlist_header *p_st_header, Frlist_node *p_node_add)
{
    if (p_st_header == NULL || p_node_add == NULL)
        return;

    if (p_st_header->head)
    {
        p_st_header->head->pre = p_node_add;
        p_node_add->pre        = NULL;
        p_node_add->next       = p_st_header->head;
        p_st_header->head      = p_node_add;
    }
    else
    {
        p_node_add->pre   = NULL;
        p_node_add->next  = NULL;
        p_st_header->head = p_st_header->tail = p_node_add;
    }

    p_st_header->size++;
}

void frlist_merge_header_top(Frlist_header *p_st_hdr_dst, Frlist_header *p_st_hdr_src)
{
    if (p_st_hdr_dst == NULL || p_st_hdr_src == NULL || p_st_hdr_src->size == 0)
        return;

    if (p_st_hdr_dst->size == 0)
    {
        p_st_hdr_dst->head = p_st_hdr_src->head;
        p_st_hdr_dst->tail = p_st_hdr_src->tail;
        p_st_hdr_dst->size = p_st_hdr_src->size;
        goto END;
    }

    p_st_hdr_src->tail->next = p_st_hdr_dst->head;
    p_st_hdr_dst->head       = p_st_hdr_src->head;
    p_st_hdr_dst->size += p_st_hdr_src->size;

END:
    p_st_hdr_src->head = NULL;
    p_st_hdr_src->tail = NULL;
    p_st_hdr_src->size = 0;
}

void frlist_add_sort(Frlist_header *p_st_header, Frlist_node *p_node_add, list_node_compare pf_compare)
{
    Frlist_node *p_node_tmp, *p_node_pre = NULL;
    int         i_ret_fun;

    if (p_st_header == NULL || p_node_add == NULL)
        return;

    if (!pf_compare || p_st_header->size < 1)
    {
        frlist_add_tail(p_st_header, p_node_add);
        return;
    }

    p_node_tmp = p_st_header->head;
    while (p_node_tmp)
    {
        i_ret_fun      = pf_compare(p_node_add, p_node_tmp);
        if (i_ret_fun <= 0)
            break;
        else
            p_node_pre = p_node_tmp;

        p_node_tmp = p_node_tmp->next;
    }

    //数据插入头部
    if (p_node_pre == NULL)
    {
        p_node_tmp = p_st_header->head;
        p_st_header->head = p_node_add;
        p_node_add->pre   = NULL;
        p_node_add->next  = p_node_tmp;
        p_node_tmp->pre   = p_node_add;

        p_st_header->size++;
        return;
    }

    //数据插入中间或尾部
    p_node_tmp = p_node_pre->next;
    p_node_pre->next = p_node_add;

    p_node_add->pre     = p_node_pre;
    p_node_add->next    = p_node_tmp;
    //不是插入最后
    if (p_node_tmp)
        p_node_tmp->pre = p_node_add;

    p_st_header->size++;
}

Frlist_node *frlist_node_idx(const Frlist_header *p_st_header, int i_idx)
{
    int         i       = 0;
    Frlist_node *p_node = NULL;

    if (p_st_header == NULL)
        return NULL;

    p_node = p_st_header->head;
    if (i_idx < 0 || i_idx >= (int) p_st_header->size)
        return NULL;

    while (i < i_idx)
    {
        p_node = p_node->next;
        i++;
    }

    return p_node;
}

Frlist_node *frlist_find(Frlist_header *p_st_header, const void *p_data, list_node_find pf_find)
{
    Frlist_node *p_node_tmp;
    int         i_ret_fun;

    if (p_st_header == NULL || pf_find == NULL)
        return NULL;

    p_node_tmp = p_st_header->head;
    while (p_node_tmp)
    {
        i_ret_fun = pf_find(p_node_tmp, p_data);
        if (i_ret_fun == 0)
        {
            return p_node_tmp;
        }

        p_node_tmp = p_node_tmp->next;
    }

    return NULL;
}

void frlist_show(Frlist_header *p_st_header, char *p_outbuff, unsigned int ui_buff_size, list_node_show pf_show)
{
    Frlist_node *p_node_tmp;
    int         i_used = 0;

    if (p_st_header == NULL || pf_show == NULL)
        return;

    *p_outbuff = '\0';
    p_node_tmp = p_st_header->head;
    while (p_node_tmp && (ui_buff_size > i_used + 1))
    {
        pf_show(p_node_tmp, p_outbuff + i_used, ui_buff_size - i_used);
        i_used += strlen(p_outbuff + i_used);

        p_node_tmp = p_node_tmp->next;
    }
}

void frlist_show_all(Frlist_header *p_st_header, char **pp_outbuff, list_node_show_all pf_show)
{
    Frlist_node *p_node_tmp;
    char        *p_all = NULL;
    char        *p_tmp = NULL;
    int         i_used = 0, i_malloc = 1024;

    if (p_st_header == NULL || pf_show == NULL || pp_outbuff == NULL)
        return;

    p_all = (char *) malloc(i_malloc);
    if (p_all == NULL)
        return;
    p_all[0] = '\0';
    p_node_tmp = p_st_header->head;
    while (p_node_tmp)
    {
        pf_show(p_node_tmp, &p_tmp);
        if (p_tmp)
        {
            i_used += strlen(p_tmp);
            if (i_used >= i_malloc)
            {
                char *p_remalloc = NULL;

                if (i_used - i_malloc < 1024)
                    i_malloc += 1024;
                else
                    i_malloc += i_used + 1;
                p_remalloc       = (char *) malloc(i_malloc);
                if (p_remalloc == NULL)
                {
                    free(p_all);
                    fprintf(stderr, "malloc err\n");
                    return;
                }
                snprintf(p_remalloc, i_malloc, "%s%s", p_all, p_tmp);
                free(p_all);
                p_all = p_remalloc;
            }
            else
            {
                strcat(p_all, p_tmp);
            }
            free(p_tmp);
            p_tmp = NULL;
        }

        p_node_tmp = p_node_tmp->next;
    }
    *pp_outbuff = p_all;
}

void frlist_show_all2(Frlist_header *p_st_header, char **pp_outbuff, list_node_show pf_show)
{
    Frlist_node *p_node_tmp;
    char        *p_all        = NULL;
    char        arr_tmp[2048] = {'\0'};
    int         i_used        = 0, i_malloc = 1024;

    if (p_st_header == NULL || pf_show == NULL || pp_outbuff == NULL)
        return;

    p_all = (char *) malloc(i_malloc);
    if (p_all == NULL)
        return;
    p_all[0] = '\0';
    p_node_tmp = p_st_header->head;
    while (p_node_tmp)
    {
        pf_show(p_node_tmp, arr_tmp, sizeof(arr_tmp));

        i_used += strlen(arr_tmp);
        if (i_used >= i_malloc)
        {
            char *p_remalloc = NULL;

            if (i_used - i_malloc < 1024)
                i_malloc += 1024;
            else
                i_malloc += i_used + 1;
            p_remalloc       = (char *) malloc(i_malloc);
            if (p_remalloc == NULL)
            {
                free(p_all);
                fprintf(stderr, "malloc err\n");
                return;
            }
            snprintf(p_remalloc, i_malloc, "%s%s", p_all, arr_tmp);
            free(p_all);
            p_all = p_remalloc;
        }
        else
        {
            strcat(p_all, arr_tmp);
        }
        arr_tmp[0] = '\0';

        p_node_tmp = p_node_tmp->next;
    }
    *pp_outbuff = p_all;
}

int frlist_run(Frlist_header *p_st_header, void *p_data, list_node_run pf_run)
{
    Frlist_node *p_st_node_do, *p_st_node_tmp;
    int         i_ret_fun = 1;

    if (p_st_header == NULL || pf_run == NULL)
        return 1;

    p_st_node_do = p_st_header->head;
    while (p_st_node_do)
    {
        p_st_node_tmp = p_st_node_do->next;
        i_ret_fun     = pf_run(p_st_node_do, p_data);
        if (i_ret_fun == 0)
            break;
        p_st_node_do = p_st_node_tmp;
    }
    return i_ret_fun;
}

int frlist_shallow_copy(Frlist_header *p_st_hdr_dst, Frlist_header *p_st_hdr_src)
{
    Frlist_node *p_st_node_do, *p_st_node_tmp, *p_st_node_new;

    if (p_st_hdr_dst == NULL || p_st_hdr_src == NULL)
        return 1;

    p_st_node_do = p_st_hdr_src->head;
    while (p_st_node_do)
    {
        p_st_node_tmp = p_st_node_do->next;

        if (p_st_node_do->data)
        {
            p_st_node_new = frlist_node_create(p_st_node_do->data);
            if (p_st_node_new)
            {
                frlist_add_tail(p_st_hdr_dst, p_st_node_new);
            }
        }

        p_st_node_do = p_st_node_tmp;
    }
    return 0;
}

Frlist_node *frlist_remove_head(Frlist_header *p_st_header)
{
    Frlist_node *p_node = NULL;

    if (p_st_header == NULL)
        return NULL;

    if (p_st_header->head)
    {
        p_node = p_st_header->head;
        p_st_header->head = p_st_header->head->next;

        if (p_st_header->head == NULL)
            p_st_header->tail = NULL;
        else
        {
            //当前删除节点的下一个节点的pre设置为NULL
            p_st_header->head->pre = NULL;
        }

        p_node->next = NULL;
        p_st_header->size--;
    }

    return p_node;
}

/*******************************************************************************
*  FUNC     :  删除链表所有节点
*  ARGS     :  p_st_header (*Frlist_header)
            :  pf_free     (节点删除用函数指针)
*  RTN      :
*  NOTE     :  list 头不删除
*******************************************************************************/
void frlist_remove_all(Frlist_header *p_st_header, list_node_free pf_free)
{
    Frlist_node *p_node;

    if (p_st_header == NULL)
        return;

    while ((p_node = frlist_remove_head(p_st_header)))
    {
        if (pf_free)
            (*pf_free)(p_node);

        if (p_node->p_key)
            free(p_node->p_key);
        free(p_node);
    }

    p_st_header->size = 0;
    p_st_header->head = p_st_header->tail = NULL;
}


void frlist_remove_from_idx(Frlist_header *p_st_header, int i_idx, list_node_free pf_free)
{
    Frlist_node *p_node    = NULL, *p_node_free = NULL;
    int         i_free_idx = i_idx, i_pos = 0;

    if (p_st_header == NULL || p_st_header->size == 0 || p_st_header->size < i_idx + 1)
        return;

    if (i_free_idx < 0)
        i_free_idx = 0;

    //定位到需要删除的节点
    p_node = p_st_header->head;
    i_pos  = 0;
    while (i_pos < i_free_idx && p_node)
    {
        p_st_header->tail = p_node;
        p_node = p_node->next;
        i_pos++;
    }
    if (p_node == NULL)
        return;

    p_st_header->tail->next = NULL;
    if (i_free_idx == 0)
    {
        p_st_header->head = NULL;
        p_st_header->tail = NULL;
    }

    //释放节点
    while (p_node)
    {
        p_node_free = p_node;
        p_node      = p_node->next;

        if (pf_free)
            (*pf_free)(p_node_free);
        if (p_node_free->p_key)
            free(p_node_free->p_key);
        free(p_node_free);
        p_st_header->size -= 1;
    }
}

void frlist_node_free(Frlist_header *p_st_header, Frlist_node *p_node, int i_del_list, list_node_free pf_free)
{
    if ((i_del_list == 0 && (p_st_header == NULL || p_st_header->head == NULL))
        || p_node == NULL)
        return;

    //如果需要减少节点，表明该节点并未从list里删除
    if (i_del_list == 0)
    {
        //头节点
        if (p_st_header->head == p_node)
        {
            p_st_header->head     = p_node->next;
            if (p_st_header->tail == p_node)
                p_st_header->tail = NULL;
        }
            //尾节点
        else if (p_st_header->tail == p_node)
        {
            p_node->pre->next = NULL;
            p_st_header->tail = p_node->pre;
        }
        else /* 中间节点 */
        {
            p_node->pre->next = p_node->next;
            p_node->next->pre = p_node->pre;
        }

        p_st_header->size -= 1;
    }

    if (pf_free)
        (*pf_free)(p_node);
    if (p_node->p_key)
        free(p_node->p_key);
    free(p_node);
}

void frlist_node_free2(Frlist_node **pp_st_node)
{
    Frlist_node *p_st_node = NULL;

    if (pp_st_node == NULL || *pp_st_node == NULL)
        return;

    p_st_node = *pp_st_node;
    if (p_st_node->p_key)
    {
        free(p_st_node->p_key);
        //p_st_node->l_key = 0;
        //p_st_node->p_key = NULL;
        //p_st_node->ui_key_len = 0;
    }
    free(p_st_node);
    *pp_st_node = NULL;
}

void frlist_node_apart(Frlist_header *p_st_header, Frlist_node *p_st_node)
{
    if (p_st_header == NULL || p_st_header->head == NULL || p_st_node == NULL)
        return;

    //头节点
    if (p_st_header->head == p_st_node)
    {
        p_st_header->head     = p_st_node->next;
        if (p_st_header->tail == p_st_node)
            p_st_header->tail = NULL;
    }
        //尾节点
    else if (p_st_header->tail == p_st_node)
    {
        p_st_node->pre->next = NULL;
        p_st_header->tail    = p_st_node->pre;
    }
    else /* 中间节点 */
    {
        p_st_node->pre->next = p_st_node->next;
        p_st_node->next->pre = p_st_node->pre;
    }

    p_st_header->size -= 1;
}

void frlist_destroy(Frlist_header *p_st_header, list_node_free pf_free)
{
    if (p_st_header == NULL)
        return;

    frlist_remove_all(p_st_header, pf_free);
    free(p_st_header);
}
