/*
 * Copyright (c) 1992, 1993, 1994, 1995, 1996
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * Macros to extract possibly-unaligned big-endian integral values.
 */
#include "global.h"


#define EXTRACT_24BITS(p) \
	((u32)(((u32)(*((const u8 *)(p) + 0)) << 16) | \
		((u32)(*((const u8 *)(p) + 1)) << 8) | \
		((u32)(*((const u8 *)(p) + 2)) << 0)))

#define EXTRACT_40BITS(p) \
	((u64)(((u64)(*((const u8 *)(p) + 0)) << 32) | \
		((u64)(*((const u8 *)(p) + 1)) << 24) | \
		((u64)(*((const u8 *)(p) + 2)) << 16) | \
		((u64)(*((const u8 *)(p) + 3)) << 8) | \
		((u64)(*((const u8 *)(p) + 4)) << 0)))

#define EXTRACT_48BITS(p) \
	((u64)(((u64)(*((const u8 *)(p) + 0)) << 40) | \
		((u64)(*((const u8 *)(p) + 1)) << 32) | \
		((u64)(*((const u8 *)(p) + 2)) << 24) | \
		((u64)(*((const u8 *)(p) + 3)) << 16) | \
		((u64)(*((const u8 *)(p) + 4)) << 8) | \
		((u64)(*((const u8 *)(p) + 5)) << 0)))

#define EXTRACT_56BITS(p) \
	((u64)(((u64)(*((const u8 *)(p) + 0)) << 48) | \
		((u64)(*((const u8 *)(p) + 1)) << 40) | \
		((u64)(*((const u8 *)(p) + 2)) << 32) | \
		((u64)(*((const u8 *)(p) + 3)) << 24) | \
		((u64)(*((const u8 *)(p) + 4)) << 16) | \
		((u64)(*((const u8 *)(p) + 5)) << 8) | \
		((u64)(*((const u8 *)(p) + 6)) << 0)))

/*
 * Macros to extract possibly-unaligned little-endian integral values.
 * XXX - do loads on little-endian machines that support unaligned loads?
 */
#define EXTRACT_LE_8BITS(p) (*(p))
#define EXTRACT_LE_16BITS(p) \
	((u16)(((u16)(*((const u8 *)(p) + 1)) << 8) | \
		((u16)(*((const u8 *)(p) + 0)) << 0)))
#define EXTRACT_LE_32BITS(p) \
	((u32)(((u32)(*((const u8 *)(p) + 3)) << 24) | \
		((u32)(*((const u8 *)(p) + 2)) << 16) | \
		((u32)(*((const u8 *)(p) + 1)) << 8) | \
		((u32)(*((const u8 *)(p) + 0)) << 0)))
#define EXTRACT_LE_24BITS(p) \
	((u32)(((u32)(*((const u8 *)(p) + 2)) << 16) | \
		((u32)(*((const u8 *)(p) + 1)) << 8) | \
		((u32)(*((const u8 *)(p) + 0)) << 0)))
#define EXTRACT_LE_64BITS(p) \
	((u64)(((u64)(*((const u8 *)(p) + 7)) << 56) | \
		((u64)(*((const u8 *)(p) + 6)) << 48) | \
		((u64)(*((const u8 *)(p) + 5)) << 40) | \
		((u64)(*((const u8 *)(p) + 4)) << 32) | \
		((u64)(*((const u8 *)(p) + 3)) << 24) | \
		((u64)(*((const u8 *)(p) + 2)) << 16) | \
		((u64)(*((const u8 *)(p) + 1)) << 8) | \
		((u64)(*((const u8 *)(p) + 0)) << 0)))
