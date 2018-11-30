# AliOS-Things KV分析

## 1 简介

**KV组件写平衡特性的实现考量**  

由于小型物联网嵌入式设备的硬件资源较为匮乏，对code size以及RAM的占用size比较敏感。所以基于资源消耗的考量，写平衡特性在KV组件中的实现遵循make it simple原则，主要依赖以下两个策略来实现： 

1. 异地更新策略：Key-Value键值对采用顺序写入、异地更新的方式，即不再在原存储位置擦除重写，而是在其余空闲位置写入新键值并将原键值标记无效等待回收。这样既可以减少flash的擦除操作次数，又可以提高flash的空间利用率，也避免了对“特定”存储区块过度使用的问题。示意图如下：

![kv1](images\1.png)

2. 垃圾回收策略：当flash存储区块的剩余可用空间达到阈值时，会触发垃圾回收机制。垃圾回收机制采用基础的SGC算法进行资源回收释放，即当系统触发垃圾回收时，从当前写入块的下一个存储块开始依次检查存储块的管理状态，若存储块的管理状态为Dirty状态，则将该存储块中的有效数据依次挪向垃圾回收预留的空闲存储块，当数据迁移完成后，会擦除Dirty存储块并标记可用空闲状态。

![kv2](images\2.png) 



## 2 使用

在developer中kv对外提供了3个接口 ：

- aos_kv_set：添加数据
- aos_kv_get：获取数据
- aos_kv_del：删除数据

以上转载自：[AliOS Things KV组件的写平衡特性](https://yq.aliyun.com/articles/291229#comment)

## 3 源码分析

一下以aos_kv_set为例，进行简单的分析。

aos_kv_set:

```
int aos_kv_set(const char *key, const void *val, int len, int sync)
{
    kv_item_t *item;
    int        ret;
    // 数据有效验证
    if (!key || !val || len <= 0 || strlen(key) > ITEM_MAX_KEY_LEN ||
        len > ITEM_MAX_VAL_LEN) {
        return RES_INVALID_PARAM;
    }
	// The flag to indicate garbage collection is triggered
    if (g_kv_mgr.gc_triggered) {
        (g_kv_mgr.gc_waiter)++;
        aos_sem_wait(&(g_kv_mgr.gc_sem), AOS_WAIT_FOREVER);
    }
	// 获取锁
    if ((ret = aos_mutex_lock(&(g_kv_mgr.kv_mutex), AOS_WAIT_FOREVER)) !=
        RES_OK) {
        return ret;
    }

    item = kv_item_get(key);
    if (item) {  /* 已经存在，则更新，否则增加一个key-value*/
        ret = kv_item_update(item, key, val, len);
        kv_item_free(item);
    } else {
        ret = kv_item_store(key, val, len, 0);
    }
	// 执行完毕，释放锁
    aos_mutex_unlock(&(g_kv_mgr.kv_mutex));
    return ret;
}
```

如果满足条件，则执行kv_item_store对数据进行保存

kv_item_store:

```
static int kv_item_store(const char *key, const void *val, int len,
                         uint16_t origin_off)
{
    kv_storeage_t store;
    item_hdr_t    hdr;
    char *        p;
    kvpos_t       pos;
    uint8_t       index;

    hdr.magic      = ITEM_MAGIC_NUM;
    hdr.state      = ITEM_STATE_NORMAL;
    hdr.key_len    = strlen(key);
    hdr.val_len    = len;
    hdr.origin_off = origin_off;

    store.len =
      (ITEM_HEADER_SIZE + hdr.key_len + hdr.val_len + ~KV_ALIGN_MASK) &
      KV_ALIGN_MASK;
    store.p = (char *)aos_malloc(store.len);
    if (!store.p) {
        return RES_MALLOC_FAILED;
    }

    memset(store.p, 0, store.len);
    p = store.p + ITEM_HEADER_SIZE;
    memcpy(p, key, hdr.key_len);
    p += hdr.key_len;
    memcpy(p, val, hdr.val_len);
    p -= hdr.key_len;
    hdr.crc = utils_crc8((uint8_t *)p, hdr.key_len + hdr.val_len);
    memcpy(store.p, &hdr, ITEM_HEADER_SIZE);

    pos = kv_item_calc_pos(store.len); /* 获取要写入的地址，并且启动垃圾回收策略 */
    if (pos > 0) {
        // 写到flash
        store.ret = raw_write(pos, store.p, store.len);
        if (store.ret == RES_OK) {
            g_kv_mgr.write_pos = pos + store.len;
            index              = g_kv_mgr.write_pos >> BLK_BITS;
            g_kv_mgr.block_info[index].space -= store.len;
        }
    } else {
        store.ret = RES_NO_SPACE;
    }

    if (store.p) {
        aos_free(store.p);
    }
    return store.ret;
}
```



## 参考

- [AliOS Things KV组件的写平衡特性](https://yq.aliyun.com/articles/291229#comment)