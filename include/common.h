#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_
#include "types.h"
//往端口写一个字节
void outb (uint16_t port,uint8_t value);

//从端口读一个字节
uint8_t inb (uint16_t port);

//从端口读一个字
uint16_t intw (uint16_t port);

#endif
