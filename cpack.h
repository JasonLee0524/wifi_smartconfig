#ifndef _CPACK_H
#define _CPACK_H

#include "global.h"

struct cpack_state {
	const u8 *c_buf;
	const u8 *c_next;
	size_t c_len;
};

s32 cpack_init(struct cpack_state *, const u8 *, size_t);
s32 cpack_uint8(struct cpack_state *, u8 *);
s32 cpack_uint16(struct cpack_state *, u16 *);
s32 cpack_uint32(struct cpack_state *, u32 *);
s32 cpack_uint64(struct cpack_state *, u64 *);

const u8 *cpack_next_boundary(const u8 * buf, const u8 * p, size_t alignment);
const u8 *cpack_align_and_reserve(struct cpack_state *cs, size_t wordsize);

#define cpack_int8(__s, __p)	cpack_uint8((__s),  (u8*)(__p))
#define cpack_int16(__s, __p)	cpack_uint16((__s), (u16*)(__p))
#define cpack_int32(__s, __p)	cpack_uint32((__s), (u32*)(__p))
#define cpack_int64(__s, __p)	cpack_uint64((__s), (u64*)(__p))

extern s32 cpack_advance(struct cpack_state *, const size_t);

#endif /* _CPACK_H */
