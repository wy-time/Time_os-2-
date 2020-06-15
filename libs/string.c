#include "string.h" 
void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len)
{
    int i;
    for(i=0;i<len;i++)
    {
        dest[i]=src[i];
        src++;
    }
}

void memset(void *dest, uint8_t val, uint32_t len)
{
    int i;
    char * temp=dest;
    for(i=0;i<len;i++)
        temp[i]=val;
}

void bzero(void *dest, uint32_t len)
{
    memset(dest,0,len);
}

int strcmp(const char *str1, const char *str2)
{
    while(1)
    {
        if(*str1<*str2)
            return -1;
        else if(*str1>*str2)
            return 1;
        if(*str1=='\0')
            return 0;
        str1++;
        str2++;
    }
}

char *strcpy(char *dest, const char *src)
{
    while(*src)
    {
        *dest=*src;
        dest++;
        src++;
    }
    *dest='\0';
    return dest;
}

char *strcat(char *dest, const char *src)
{
    while(*dest)
    {
        dest++;
    }
    while(*src)
    {
        *dest=*src;
        src++;
        dest++;
    }
    *dest='\0';
    return dest;
}

int strlen(const char *src)
{
    int len=0;
    while(*src)
    {
        len++;
        src++;
    }
    return len;
}
