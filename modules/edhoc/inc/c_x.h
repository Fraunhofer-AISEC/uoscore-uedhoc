#ifndef C_X_H
#define C_X_H
#include "../../common/inc/byte_array.h"
#include "../../common/inc/oscore_edhoc_error.h"

enum c_x_type { INT, BSTR };

union c_x_mem {
	int c_x_int;
	struct byte_array c_x_bstr;
};

struct c_x {
	enum c_x_type type;
	union c_x_mem mem;
};

/**
 * 
 * 
 */
enum err c_x_set(enum c_x_type t, const uint8_t *c_x_raw_buf,
		 uint32_t c_x_raw_buf_len, int c_x_int, struct c_x *out);
/**
 * 
 * 
 */
void c_x_init(struct c_x *c, uint8_t *buf, uint32_t buf_len);

/**
 * we may not need that
 * 
 */
void c_x_get(struct c_x *c_x, enum c_x_type *t, uint8_t *c_x_raw_buf,
	     uint32_t *c_x_raw_buf_len, int *c_x_int);
#endif
