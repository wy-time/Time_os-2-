#ifndef INCLUDE_PMM_H
#define INCLUDE_PMM_H

#include "multiboot.h" 

// 内核文件在内存中的起始和结束位置
// 在链接器脚本中要求链接器定义
extern uint8_t kern_start[];
extern uint8_t kern_end[];
extern uint32_t phy_mem_count;//动态分配的物理内存总数

#define PMM_MAX_SIZE 0x20000000//规定最大的物理内存为512MB
#define PMM_PAGE_SIZE 0x1000 //一页的大小为4KB
#define PAGE_MAX_SIZE (PMM_MAX_SIZE/PMM_PAGE_SIZE)//最多的物理页面的数量
#define STACK_SIZE 8192//线程栈的大小
#define PHY_PAGE_MASK 0xFFFFF000//页掩码按照 4096 对齐地址

//打印物理内存布局
void show_memory_map();

void init_pmm();//初始化内存布局
uint32_t pmm_alloc_page();//申请一页物理页，返回该页的地址
void pmm_free_page(uint32_t p);//释放申请的内存
#endif// INCLUDE_PMM_H

