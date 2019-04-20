# 修改lvgl动态内存分配

原版：

```
#if LV_MEM_ADR == 0
    /*Allocate a large array to store the dynamically allocated data*/
    static LV_MEM_ATTR MEM_UNIT work_mem_int[LV_MEM_SIZE / sizeof(MEM_UNIT)];
    work_mem = (uint8_t *) work_mem_int;
#else
    work_mem = (uint8_t *) LV_MEM_ADR;
#endif
```

- 方式一：使用我们的分配方式（我们可以使用外部的RAM来使用），如果使用的是默认的话，会直接使用CPU内部的RAM这样会很浪费内存

```
#if LV_MEM_ADR == 0
    /*Allocate a large array to store the dynamically allocated data*/
    //static LV_MEM_ATTR MEM_UNIT work_mem_int[LV_MEM_SIZE / sizeof(MEM_UNIT)];
    work_mem = (uint8_t *) mymalloc(LV_MEM_SIZE / sizeof(MEM_UNIT)); //work_mem_int;
#else
    work_mem = (uint8_t *) LV_MEM_ADR;
#endif

```

- 方式2：修改地址

```
#if LV_MEM_ADR == 地址（我们想要分配内存的地址）
    /*Allocate a large array to store the dynamically allocated data*/
    static LV_MEM_ATTR MEM_UNIT work_mem_int[LV_MEM_SIZE / sizeof(MEM_UNIT)];
    work_mem = (uint8_t *) work_mem_int;
#else
    work_mem = (uint8_t *) LV_MEM_ADR;
#endif
```

