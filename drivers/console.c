#include "types.h" 
#include "console.h" 
#include "common.h" 
#include "vmm.h" 
static uint16_t *video_memory= (uint16_t *)(0xB8000+PAGE_OFFSET);//显存的起始地址，每两个字节表示一个字符所以是16位
//屏幕光标的坐标
static uint8_t cursor_x=0;
static uint8_t cursor_y=0;

//移动光标的位置
static void move_cursor ()
{
    uint16_t cursorLocation=cursor_y*80+cursor_x;//一行有80个字符
    /*控制光标位置的寄存器为14号和15号寄存器，
     * 分别存储位置的高八位和低八位*/
    outb(0x3D4,14); //要设置14号寄存器，即位置信息的高八位
    outb(0x3D5,cursorLocation>>8);
    outb(0x3D4,15); //要设置15号寄存器，即位置信息的低八位
    outb(0x3D5,cursorLocation);
}

//清屏操作
void console_clear()
{
    uint8_t attribute_byte=(0<<4)|(15&0x0f);//白底黑字
    uint16_t blank=0x20|(attribute_byte<<8);
    int i;
    for (i=0;i<80*25;i++)
    {
        video_memory[i]=blank;
    }
    cursor_x=cursor_y=0;
    move_cursor();
}

//屏幕的滚动
static void scroll()
{
    int i;
    uint8_t attribute_byte=(0<<4)|(15&0x0f);//白底黑字
    uint16_t blank=0x20|(attribute_byte<<8);
    //大于25行就该滚动屏幕了
    if(cursor_y>=25)
    {
        //米一行向上移动一行
        for (i=0;i<24*80;i++)
        {
            video_memory[i]=video_memory[i+80];
        }
        //最后一行填充空格
        for (i=24*80;i<25*80;i++)
            video_memory[i]=blank;
        //光标向上一行
        cursor_y--;
    }
}

// 屏幕输出一个字符带颜色
void console_putc_color(char c, real_color_t back, real_color_t fore)
{
    //设置字体颜色
    uint8_t back_color =(uint8_t) back;
    uint8_t fore_color=(uint8_t) fore;
    uint8_t attribute_byte=(back_color<<4)|(fore_color&0x0f);
    uint16_t attirbute=attribute_byte<<8;
    // 0x08 是退格键的 ASCII 码
    // 0x09 是tab 键的 ASCII 码
    if (c==0x08&&cursor_x)
        cursor_x--;
    else if(c==0x09)
        cursor_x=(cursor_x+4)& ~(4-1);//即当前位置移动4下后的最高位的1所表示的值
    else if(c=='\r')
        cursor_x=0;
    else if(c=='\n')
    {
        cursor_x=0;
        cursor_y++;
    }else if(c>=' ')
    {
        video_memory[cursor_y*80+cursor_x]=c|attirbute;
        cursor_x++;
    }
    //一行满了换行
    if(cursor_x>=80)
    {
        cursor_x=0;
        cursor_y++;
    }
    // 如果需要的话滚动屏幕显示
    scroll();
    // 移动硬件的输入光标
    move_cursor();
}

// 屏幕打印一个以 \0 结尾的字符串默认黑底白字
void console_write(char *cstr)
{
    while(*cstr)
    {
        console_putc_color(*cstr++,rc_black,rc_white);
    }
}

// 屏幕打印一个以 \0 结尾的字符串带颜色
void console_write_color(char *cstr, real_color_t back, real_color_t fore)
{
    while(*cstr)
    {
        console_putc_color(*cstr++,back,fore);
    }
}

// 屏幕输出一个十六进制的整型数
void console_write_hex(uint32_t n, real_color_t back, real_color_t fore)
{
    uint8_t data[8];
    int16_t len=0;
    while(n)
    {
        data[len++]=n%16;
        n/=16;
    }
    len--;
    console_putc_color('0',back,fore);
    console_putc_color('x',back,fore);
    while(len>=0)
    {
        if(data[len]<10)
            console_putc_color(data[len]+'0',back,fore);
        else
            console_putc_color(data[len]-10+'A',back,fore);
        len--;
    }
}


// 屏幕输出一个十进制的整型数
void console_write_dec(uint32_t n, real_color_t back, real_color_t fore)
{
    uint8_t data[10];
    int16_t len=0;
    while(n)
    {
        data[len++]=n%10;
        n/=10;
    }
    len--;
    while(len>=0)
    {
        console_putc_color(data[len]+'0',back,fore);
        len--;
    }
}
