#include <string.h>

#include "cpack.h"
#include "extract.h"

const u8 *cpack_next_boundary(const u8 * buf, const u8 * p, size_t alignment)
{
	size_t misalignment = (size_t) (p - buf) % alignment;

	if (misalignment == 0)
		return p;

	return p + (alignment - misalignment);
}

/* Advance to the next wordsize boundary. Return NULL if fewer than
 * wordsize bytes remain in the buffer after the boundary.  Otherwise,
 * return a pointer to the boundary.
 */
const u8 *cpack_align_and_reserve(struct cpack_state *cs, size_t wordsize)
{
	const u8 *next;

	/* Ensure alignment. */
	next = cpack_next_boundary(cs->c_buf, cs->c_next, wordsize);

	/* Too little space for wordsize bytes? */
	if (next - cs->c_buf + wordsize > cs->c_len)
		return NULL;

	return next;
}

/* Advance by N bytes without returning them. */
int cpack_advance(struct cpack_state *cs, const size_t toskip)
{
	/* No space left? */
	if (cs->c_next - cs->c_buf + toskip > cs->c_len)
		return -1;
	cs->c_next += toskip;
	return 0;
}

int cpack_init(struct cpack_state *cs, const u8 * buf, size_t buflen)
{
	memset(cs, 0, sizeof(*cs));

	cs->c_buf = buf;
	cs->c_len = buflen;
	cs->c_next = cs->c_buf;

	return 0;
}

/* Unpack a 64-bit unsigned integer. */
int cpack_uint64(struct cpack_state *cs, u64 * u)
{
	const u8 *next;

	if ((next = cpack_align_and_reserve(cs, sizeof(*u))) == NULL)
		return -1;

	*u = EXTRACT_LE_64BITS(next);

	/* Move pointer past the u64. */
	cs->c_next = next + sizeof(*u);
	return 0;
}

/* Unpack a 32-bit unsigned integer. */
int cpack_uint32(struct cpack_state *cs, u32 * u)
{
	const u8 *next;

	if ((next = cpack_align_and_reserve(cs, sizeof(*u))) == NULL)
		return -1;

	*u = EXTRACT_LE_32BITS(next);

	/* Move pointer past the u32. */
	cs->c_next = next + sizeof(*u);
	return 0;
}

/* Unpack a 16-bit unsigned integer. */
int cpack_uint16(struct cpack_state *cs, u16 * u)
{
	const u8 *next;

	if ((next = cpack_align_and_reserve(cs, sizeof(*u))) == NULL)
		return -1;

	*u = EXTRACT_LE_16BITS(next);

	/* Move pointer past the u16. */
	cs->c_next = next + sizeof(*u);
	return 0;
}

/* Unpack an 8-bit unsigned integer. */
int cpack_uint8(struct cpack_state *cs, u8 * u)
{
	/* No space left? */
	if ((size_t) (cs->c_next - cs->c_buf) >= cs->c_len)
		return -1;

	*u = *cs->c_next;

	/* Move pointer past the u8. */
	cs->c_next++;
	return 0;
}
