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

#ifndef _LIST_H_
#define _LIST_H_

/*! @brief 链表节点 */
typedef struct __st_listnode
{
    struct __st_listnode *pre;
    struct __st_listnode *next;
    void                 *data;

    unsigned long ul_idx;      /*!< 普通链表时不用，保存当前节点在hash中的位置 */
    long          l_key;       /*!< 普通链表时不用，hash key */
    char          *p_key;                /*!< 普通链表时不用，hash key(string) */
    unsigned int  ui_key_len;  /*!< 普通链表时不用，hash key的长度 */
} Frlist_node;

/*! @brief 链表头 */
typedef struct __st_list
{
    unsigned int size;    /*!< 节点个数 */
    Frlist_node  *head;
    Frlist_node  *tail;
} Frlist_header;

/* list_destroy() 调用改函数，如果为NULL，则不调用 */
typedef void(*list_node_free)(Frlist_node *node);
/* 例子
void my_list_node_free(Frlist_node *node)
{
    free(node->data);
}
*/

/* 获取首节点(参数是Frlist_header) */
#define FRLIST_FIRST_NODE(hdr)      ((hdr)->head)
/* 获取首节点(参数是Frlist_header) */
#define FRLIST_LAST_NODE(hdr)       ((hdr)->tail)
//#define FRLIST_FIRST_NODE(hdr)    ((hdr)?(hdr)->head:NULL)
/* 获取下一个节点(参数是node) */
#define FRLIST_NEXT_NODE(n)     ((n)?(n)->next:NULL)

/* 获取节点数据 (参数是node) */
#define FRLIST_GET_NODE_DATA(n) (n?(n)->data:NULL)
/* 获取节点数据 (参数是Frlist_header) */
#define FRLIST_GET_FISRT_NODE_DATA(hdr) FRLIST_GET_NODE_DATA(FRLIST_FIRST_NODE(hdr))

/* 比较函数指针 (返回值 <=0; 终止运行) */
typedef int (*list_node_compare)(Frlist_node *node_add, Frlist_node *node);

/* 显示函数指针 */
typedef void(*list_node_show)(Frlist_node *node, char *p_outbuff, unsigned int ui_buff_size);

/* 显示全部函数指针 */
typedef void(*list_node_show_all)(Frlist_node *node, char **pp_outbuff);

/* 查询函数指针 (比较一致返回0:终止运行;其他返回1) */
typedef int (*list_node_find)(Frlist_node *node, const void *p_compare_data);

/* 运行函数指针 (返回值为0时, 终止运行) */
typedef int (*list_node_run)(Frlist_node *node, const void *p_data);

/*!
 * 链表头初始化(并生成内存空间)
 *
 * \return Frlist_header*   (normal)\n
 *         NULL             (error)
 *
 */
Frlist_header *frlist_create(void);

/*!
 * 链表初始化
 *
 * \param p_st_hdr (Frlist_header)
 *
 * \note 如果使用 frlist_create生成连表头，则不需要改函数
 *
 */
void frlist_header_init(Frlist_header *p_st_hdr);

/*!
 * 获取链表个数
 *
 * \param p_st_hdr (Frlist_header)
 *
 * \return 链表成员个数
 *
 */
unsigned int frlist_size(const Frlist_header *p_st_header);

/*!
 * 生成节点
 *
 * \param p_data (数据指针)
 *
 * \return 节点指针 (normal)\n
 *         NULL     (error)
 *
 */
Frlist_node *frlist_node_create(void *p_data);

/*!
 * 判断链表是否为空
 *
 * \param p_st_header (Frlist_header*)
 *
 * \return FR_YES:空; FR_NO:有节点
 *
 */
int frlist_empty(Frlist_header *p_st_header);

/*!
 * 增加节点到尾部
 *
 * \param *p_st_header (Frlist_header)
 * \param *p_node_add  (Frlist_node)
 *
 * \return FR_YES:空; FR_NO:有节点
 *
 */
void frlist_add_tail(Frlist_header *p_st_header, Frlist_node *p_node);

#define frlist_push(x, y) frlist_add_tail(x, y)

/*!
 * 增加节点到头部
 *
 * \param *p_st_header (Frlist_header)
 * \param *p_node_add  (Frlist_node)
 *
 */
void frlist_add_header(Frlist_header *p_st_header, Frlist_node *p_node_add);

/*!
 * 增加p_st_hdr_src链表到p_st_hdr_dst头部
 *
 * \param *p_st_hdr_dst (Frlist_header)
 * \param *p_st_hdr_src (Frlist_header)
 *
 */
void frlist_merge_header_top(Frlist_header *p_st_hdr_dst, Frlist_header *p_st_hdr_src);

/*!
 * 增加节点到顺序链表
 *
 * \param *p_st_header (Frlist_header)
 * \param *p_node_add  (Frlist_node)
 * \param *pf_compare  (比较函数指针)
 *
 */
void frlist_add_sort(Frlist_header *p_st_header, Frlist_node *p_node_add, list_node_compare pf_compare);

/*!
 * 显示节点内容
 *
 * \param *p_st_header (Frlist_header)
 * \param p_outbuff    (出力数据用空间)
 * \param ui_buff_size (出力空间大小)
 * \param pf_show      (显示数据用函数指针)
 *
 */
void frlist_show(Frlist_header *p_st_header, char *p_outbuff, unsigned int ui_buff_size, list_node_show pf_show);

/*!
 * 显示节点内容
 *
 * \param *p_st_header (Frlist_header)
 * \param pp_outbuff   (出力数据用空间)
 * \param pf_show      (显示数据用函数指针)
 *
 * \note pp_outbuff内存空间在该函数内部生成,需要在外部释放
 *
 */
void frlist_show_all(Frlist_header *p_st_header, char **pp_outbuff, list_node_show_all pf_show);

/*!
 * 显示节点内容 每个节点的长度分配为最大2048，超过2048请使用frlist_show_all
 *
 * \param *p_st_header (Frlist_header)
 * \param pp_outbuff   (出力数据用空间)
 * \param pf_show      (显示数据用函数指针)
 *
 * \note pp_outbuff内存空间在该函数内部生成,需要在外部释放
 *
 */
void frlist_show_all2(Frlist_header *p_st_header, char **pp_outbuff, list_node_show pf_show);

/*!
 * 对链表下所有节点进行函数调用
 *
 * \param *p_st_header (Frlist_header)
 * \param p_data       (函数指针用数据，如果没有设置为NULL)
 * \param pf_run       (运行用函数指针)
 *
 * \return 0:pf_run控制退出; 1:运行到最后
 *
 */
int frlist_run(Frlist_header *p_st_header, void *p_data, list_node_run pf_run);

/*!
 * 链表浅拷贝
 *
 * \param p_st_hdr_dst (Frlist_header *, p_st_hdr_src节点里的数据向该链表里追加)
 * \param p_st_hdr_src (Frlist_header *)
 *
 * \return FR_SUCC:成功\n
 *         FR_FAIL:失败
 *
 * \note node里的数据没有重新生成(节点重新生成)
 *
 */
int frlist_shallow_copy(Frlist_header *p_st_hdr_dst, Frlist_header *p_st_hdr_src);

/*!
 * 取指定位置的节点
 *
 * \param *p_st_header (Frlist_header)
 * \param i_idx        (节点号，从0开始)
 *
 * \return 节点指针 (normal)\n
 *         NULL     (error)
 *
 * \note 节点不脱链
 *
 */
Frlist_node *frlist_node_idx(const Frlist_header *p_st_header, int i_idx);

/*!
 * 查找指定节点
 *
 * \param *p_st_header (Frlist_header)
 * \param p_data       (查询数据)
 * \param pf_find      (查询数据用函数指针)
 *
 * \return 节点指针 (normal)\n
 *         NULL     (error)
 *
 * \note 节点不脱链
 *
 */
Frlist_node *frlist_find(Frlist_header *p_st_header, const void *p_data, list_node_find pf_find);

/*!
 * 获取头节点，并把该节点从列表里删除(同pop)
 *
 * \param *p_st_header (Frlist_header)
 *
 * \return 节点指针 (normal)\n
 *         NULL     (error)
 *
 * \note 节点脱链，链表个数减少(需要调用frlist_node_free进行资源释放)
 *
 */
Frlist_node *frlist_remove_head(Frlist_header *p_st_header);

#define frlist_pop(x) frlist_remove_head(x)

/*!
 * 从指定定位置开始释放
 *
 * \param p_st_header (*Frlist_header)
 * \param i_idx       (节点号，从0开始)
 * \param pf_free     (节点删除用函数指针)
 *
 */
void frlist_remove_from_idx(Frlist_header *p_st_header, int i_idx, list_node_free pf_free);

/*!
 * 删除所有节点，并把连表头也删除
 *
 * \param *p_st_header (Frlist_header)
 * \param pf_free      (节点删除用函数指针)
 *
 */
void frlist_remove_all(Frlist_header *p_st_header, list_node_free pf_free);

/*!
 * 删除指定节点
 *
 * \param p_st_header (*Frlist_header)
 * \param p_node      (*Frlist_node)
 * \param i_del_list  (FR_NO:节点数不减少, FR_YES:节点数减少)
 * \param pf_free     (节点删除用函数指针)
 *
 * \note frlist_pop/frlist_remove_head获取节点时,i_del_list = FR_NO\n
 *       其他i_dec_size = FR_YES
 *
 */
void frlist_node_free(Frlist_header *p_st_header, Frlist_node *p_node, int i_del_list, list_node_free pf_free);

/*!
 * 释放节点本身数据
 *
 * \param pp_st_node (Frlist_node **)
 *
 */
void frlist_node_free2(Frlist_node **pp_st_node);

/*!
 * 指定节点从链表上脱离
 *
 * \param p_st_header (*Frlist_header)
 * \param p_node      (*Frlist_node)
 *
 */
void frlist_node_apart(Frlist_header *p_st_header, Frlist_node *p_st_node);

/*!
 * 释放链表
 *
 * \param *p_st_header (Frlist_header)
 * \param pf_free      (节点删除用函数指针)
 *
 */
void frlist_destroy(Frlist_header *p_st_header, list_node_free pf_free);

/*!
 * 释放节点的data数据
 *
 * \param p_node (Frlist_node *)
 *
 */
void frlist_node_data_free(Frlist_node *p_st_node);

#endif //_LIST_H_
