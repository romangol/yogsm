#ifndef HEADERS_DATATYPE_H
#define HEADERS_DATATYPE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint64_t	u8;
typedef uint32_t	u4;
typedef uint16_t	u2;
typedef uint8_t		u1;

struct u32
{
	u8 v[4];
};


#endif