#include "multiboot.h" 
#include "common.h" 
#include "pmm.h" 
#include "debug.h" 
static uint32_t pmm_stack[PAGE_MAX_SIZE+1];//存放物理页框的栈
static uint32_t pmm_stack_top;//物理页框的栈指针
uint32_t phy_mem_count;//动态分配的页面数量
void show_memory_map()
{
    uint32_t mmap_addr=glb_mboot_ptr->mmap_addr;
    uint32_t mmap_length=glb_mboot_ptr->mmap_length;
    mmap_entry_t * mmap= (mmap_entry_t*)mmap_addr;
for (mmap = (mmap_entry_t *)mmap_addr; (uint32_t)mmap < mmap_addr +
        mmap_length; mmap++)
    {
        printk("base_addr=0x%X%08X,length=0x%X%08X,type=0x%X\n",(uint32_t)mmap->base_addr_high,(uint32_t)mmap->base_addr_low,(uint32_t)mmap->length_high,(uint32_t)
                mmap->length_low,(uint32_t)mmap->type);
    }
}
void init_pmm()
{
    //获取GRUB提供的内存描述结构
    mmap_entry_t * mmap_start_addr=(mmap_entry_t*)glb_mboot_ptr->mmap_addr;
    mmap_entry_t * mmap_end_addr=(mmap_entry_t*)glb_mboot_ptr->mmap_addr+glb_mboot_ptr->mmap_length;
    mmap_entry_t *mmap_entry;
    //遍历该结构寻找可用的内存段
    for(mmap_entry=mmap_start_addr;mmap_entry<mmap_end_addr;mmap_entry++)
    {
        //type为1表示可用，然后低于1MB的内存我们不用
        if(mmap_entry->type==1&&mmap_entry->base_addr_low==0x00100000)
        {
            uint32_t page_addr=mmap_entry->base_addr_low+(kern_end-kern_start);//获得内核加载结束的地址
            uint32_t length=mmap_entry->base_addr_low+mmap_entry->length_low; //获取该内存段的结束地址
            //分配的内存要小于最大支持的内存，也不能超过该内存段的长度
            while(page_addr<=PMM_MAX_SIZE&&page_addr<length)
            {
                pmm_free_page(page_addr);
                page_addr+=PMM_PAGE_SIZE;//向后移动一页的大小
                phy_mem_count++;
            }
        }
    }
}
//释放物理内存空间
void pmm_free_page(uint32_t p)
{
    assert(pmm_stack_top != PAGE_MAX_SIZE , "out of pmm_stack stack");
    pmm_stack[++pmm_stack_top]=p;
}
//申请物理内存空间
uint32_t pmm_alloc_page ()
{
    assert(pmm_stack_top!=0,"out of memory");
    uint32_t page=pmm_stack[pmm_stack_top--];
    return page;
}
