#include "idt.h"
#include "string.h"
#include "debug.h"
#include "vmm.h"
#include "pmm.h"
//内核页目录
pgd_t pgd_kern[PGD_SIZE] __attribute__ ((aligned(PAGE_SIZE)));
//内核页表
static pte_t pte_kern[PTE_COUNT][PTE_SIZE] __attribute__ ((aligned(PAGE_SIZE)));

void init_vmm ()
{
    //0xC0000000在页目录的偏移值
    uint32_t kern_pte_first_idx = PGD_INDEX(PAGE_OFFSET);
    uint32_t i,j;
    for (i=kern_pte_first_idx,j=0;i<kern_pte_first_idx+PTE_COUNT;++i,++j)
    {
        //将页表映射到页目录，程序内的地址是虚拟地址，所以要减去偏移
        pgd_kern[i]=(uint32_t)(pte_kern[j]-PAGE_OFFSET)|PAGE_PRESENT | PAGE_WRITE;
        uint32_t * pte= (uint32_t *)pte_kern;
        //映射所有的物理页
        for (i=1;i<PTE_SIZE*PTE_COUNT;++i)
        {
            pte[i]=(i<<12)|PAGE_WRITE|PAGE_PRESENT;
        }
        //页目录的物理地址
        uint32_t pgd_kern_phy_addr = (uint32_t)pgd_kern - PAGE_OFFSET;
        // 注册页错误中断的处理函数 ( 14 是页故障的中断号 )
        register_interrupt_handler(14, &page_fault);
        //切换页表
        switch_pgd(pgd_kern_phy_addr);
    }
}
void switch_pgd (uint32_t  pgd_kern_phy_addr)
{
    asm volatile ("mov %0, %%cr3" : : "r" (pgd_kern_phy_addr));
}
 //使用 flags 指出的页权限，把物理地址 pa 映射到虚拟地址 va
void map (pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags)
{
    uint32_t pgd_idx=PGD_INDEX (va);
    uint32_t pte_idx=PTE_INDEX (va);
    pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
    if (!pte)
    {
        pte=(pte_t*)pmm_alloc_page ();
        // 转换到内核线性地址并清 0
        pgd_now [pgd_idx]= (uint32_t)pte|PAGE_PRESENT | PAGE_WRITE;
        pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
        bzero(pte, PAGE_SIZE);
    }else
    {
        //转换到内核线性地址
        pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
    }
    pte [pte_idx]= (pa&PAGE_MASK)|flags;
    // 通知 CPU 更新页表缓存
    asm volatile ("invlpg (%0)" : : "a" (va));
}
//取消va虚拟地址的映射
void unmap(pgd_t *pgd_now, uint32_t va)
{
    uint32_t pgd_idx=PGD_INDEX (va);
    uint32_t pte_idx=PTE_INDEX (va);
    pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
    //该页表不在页目录中
    if (!pte)
        return;
    //转换到内核线性地址
    pte = (pte_t*)((uint32_t)pte + PAGE_OFFSET);
    pte [pte_idx]=0;
    // 通知 CPU 更新页表缓存
    asm volatile ("invlpg (%0)" : : "a" (va));
}
//获取虚拟地址对应的物理地址,成功获取返回1并将物理地址写入pa，否则返回0
uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa)
{
    uint32_t pgd_idx=PGD_INDEX (va);
    uint32_t pte_idx=PTE_INDEX (va);
    pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
    //该页表不在页目录中
    if (!pte)
        return 0;
    //转换到内核线性地址
    pte = (pte_t*)((uint32_t)pte + PAGE_OFFSET);
    if(pte [pte_idx]!=0&&pa)
    {
        *pa=pte[pte_idx]&PAGE_MASK;
        return 1;
    }
    return 0;
}
