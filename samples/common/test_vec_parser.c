#include <jsmn.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../../modules/edhoc/edhoc.h"
#include "test_vec_parser.h"
#include "cbor/decode_c_x.h"
#include "../../modules/edhoc/inc/memcpy_s.h"

static int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
	bool type_ok = tok->type == JSMN_STRING;
	bool len_ok = (int)strlen(s) == tok->end - tok->start;
	bool content_eq =
		strncmp(json + tok->start, s, tok->end - tok->start) == 0;

	if (type_ok && len_ok && content_eq) {
		return 0;
	}
	return -1;
}

int read_test_vectors(char *filename, char *test_vec_buf, uint32_t *len)
{
	long length;
	printf("Opening file: %s\n", filename);

	FILE *f = fopen(filename, "rb");

	if (f == 0) {
		printf("cannot open file\n");
		return -1;
	}

	fseek(f, 0, SEEK_END);
	length = ftell(f);
	fseek(f, 0, SEEK_SET);
	if (*len < length) {
		printf("Your test_vec_buf is to small! The test_vec_buf has size of %d bytes, but the you try to red %ld bytes\n",
		       *len, length);
		return -1;
	}
	*len = length;
	if (test_vec_buf) {
		if (0 == fread(test_vec_buf, 1, length, f)) {
			return -1;
		}
	}
	fclose(f);

	return 0;
}

int str2bytes(char *str, uint32_t str_len, struct byte_array *bytes)
{
	if (bytes->len < str_len / 2) {
		return -1;
	}
	bytes->len = str_len / 2;
	char *pos = str;
	for (uint32_t i = 0; i < str_len / 2; i++) {
		sscanf(pos, "%2hhx", &bytes->ptr[i]);
		pos += 2;
	}
	return 0;
}

static int get_test_ver_range(jsmntok_t *t, uint32_t element_total_count,
			      uint32_t *start_pos, uint32_t *end_pos,
			      uint8_t vec_num)
{
	uint32_t i;
	uint8_t vec_cnt = 0;
	/*skip the very first element*/
	for (i = 1; i < element_total_count; i++) {
		if (t[i].type == JSMN_OBJECT) {
			vec_cnt++;
			if (vec_cnt == vec_num) {
				//at i the test vector with vec_num starts
				*start_pos = i;
				while (1) {
					i++;
					if (t[i].type == JSMN_OBJECT) {
						*end_pos = i;
						return 0;
					}
				}
			}
		}
	}
	/*there is no vector with this number*/
	return -1;
}

int get_element(char *test_vec_buf, jsmntok_t *t, uint32_t element_total_count,
		char *element_name, uint8_t vec_num, struct byte_array *element)
{
	uint32_t start_pos, end_pos;
	uint32_t i;

	if (get_test_ver_range(t, element_total_count, &start_pos, &end_pos,
			       vec_num) != 0) {
		return -1;
	}
	for (i = start_pos; i < end_pos; i++) {
		if (jsoneq(test_vec_buf, &t[i], element_name) == 0) {
			uint32_t len = t[i + 1].end - t[i + 1].start;
			char *stat_str = test_vec_buf + t[i + 1].start;
			printf("%s: %.*s\n", element_name, len, stat_str);
			str2bytes(stat_str, len, element);
		}
	}

	return 0;
}

int get_C_X_decode(enum C_X_TYPE C_X, uint8_t vec_num, char *test_vec_buf,
		   int r, jsmntok_t *t, uint8_t *cx_buf, uint32_t cx_buf_len,
		   struct c_x *c_x_out)
{
	/* get c_i or c_x and decode it to int/bstr */
	uint8_t c_x_tmp_buf[32];
	struct byte_array c_x_tmp;
	byte_array_init(c_x_tmp_buf, sizeof(c_x_tmp_buf), &c_x_tmp);
	if (C_X == C_I) {
		get_element(test_vec_buf, t, r, "c_i", vec_num, &c_x_tmp);
	} else {
		get_element(test_vec_buf, t, r, "c_r", vec_num, &c_x_tmp);
	}

	bool ok;
	size_t decode_len = 0;
	enum edhoc_error err;
	struct cx_C_X_ cx;

	ok = cbor_decode_cx_C_X(c_x_tmp.ptr, c_x_tmp.len, &cx, &decode_len);
	if (!ok) {
		printf("Cannot decode C_I: %d\n", r);
		return -1;
	}

	if (cx._cx_C_X_choice == _cx_C_X_int) {
		c_x_out->type = INT;
		c_x_out->mem.c_x_int = cx._cx_C_X_int;
		PRINTF("C_x_raw (int): %d\n", c_x_out->mem.c_x_int);
	} else {
		c_x_init(c_x_out, cx_buf, cx_buf_len);
		c_x_out->type = BSTR;
		err = _memcpy_s(c_x_out->mem.c_x_bstr.ptr,
				c_x_out->mem.c_x_bstr.len,
				cx._cx_C_X_bstr.value, cx._cx_C_X_bstr.len);
		c_x_out->mem.c_x_bstr.len = cx._cx_C_X_bstr.len;
		if (err != edhoc_no_error) {
			printf("the buffer for C_X is to small");
			return -1;
		}
		PRINT_ARRAY("C_x_raw (bstr):", c_x_out->mem.c_x_bstr.ptr,
			    c_x_out->mem.c_x_bstr.len);
	}
	return 0;
}

int get_OTHER_PARTY_CRED_from_test_vec(enum role other_party_role,
				       struct other_party_cred_bufs *bufs,
				       struct other_party_cred *c,
				       uint8_t vec_num, char *test_vec_buf,
				       uint32_t test_vec_buf_len)
{
	int r;
	jsmn_parser p;
	jsmntok_t t[20000]; /* We expect no more than 20000 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, test_vec_buf, test_vec_buf_len, t,
		       sizeof(t) / sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return -1;
	}

	byte_array_init(bufs->id_cred, sizeof(bufs->id_cred), &c->id_cred);
	byte_array_init(bufs->cred, sizeof(bufs->cred), &c->cred);
	// byte_array_init(bufs->pk, sizeof(bufs->pk), &c->pk);
	byte_array_init(bufs->g, sizeof(bufs->g), &c->g);
	// byte_array_init(bufs->ca, sizeof(bufs->ca), &c->ca);
	// byte_array_init(bufs->ca_pk, sizeof(bufs->ca_pk), &c->ca_pk);

	if (other_party_role == INITIATOR) {
		get_element(test_vec_buf, t, r, "id_cred_i", vec_num,
			    &c->id_cred);
		get_element(test_vec_buf, t, r, "cred_i", vec_num, &c->cred);
		get_element(test_vec_buf, t, r, "g_i_raw", vec_num, &c->g);
	} else {
		get_element(test_vec_buf, t, r, "id_cred_r", vec_num,
			    &c->id_cred);
		get_element(test_vec_buf, t, r, "cred_r", vec_num, &c->cred);
		get_element(test_vec_buf, t, r, "g_r_raw", vec_num, &c->g);
	}

	return 0;
}

int get_EDHOC_INITIATOR_CONTEXT_from_test_vec(
	struct edhoc_initiator_context_bufs *bufs,
	struct edhoc_initiator_context *c, uint8_t vec_num, char *test_vec_buf,
	uint32_t test_vec_buf_len)
{
	int r;
	jsmn_parser p;
	jsmntok_t t[20000]; /* We expect no more than 20000 tokens */
	uint8_t method_buf[1];
	struct byte_array method;

	jsmn_init(&p);
	r = jsmn_parse(&p, test_vec_buf, test_vec_buf_len, t,
		       sizeof(t) / sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return -1;
	}

	byte_array_init(method_buf, sizeof(method_buf), &method);
	byte_array_init(bufs->suites_i, sizeof(bufs->suites_i), &c->suites_i);
	byte_array_init(bufs->ead_1, sizeof(bufs->ead_1), &c->ead_1);
	byte_array_init(bufs->ead_3, sizeof(bufs->ead_3), &c->ead_3);
	byte_array_init(bufs->id_cred_i, sizeof(bufs->id_cred_i),
			&c->id_cred_i);
	byte_array_init(bufs->cred_i, sizeof(bufs->cred_i), &c->cred_i);
	byte_array_init(bufs->g_x, sizeof(bufs->g_x), &c->g_x);
	byte_array_init(bufs->x, sizeof(bufs->x), &c->x);
	byte_array_init(bufs->g_i, sizeof(bufs->g_i), &c->g_i);
	byte_array_init(bufs->i, sizeof(bufs->i), &c->i);

	get_element(test_vec_buf, t, r, "method", vec_num, &method);
	c->method = method_buf[0];
	get_element(test_vec_buf, t, r, "suites_i", vec_num, &c->suites_i);
	get_element(test_vec_buf, t, r, "ead_1", vec_num, &c->ead_1);
	get_element(test_vec_buf, t, r, "ead_3", vec_num, &c->ead_3);
	get_element(test_vec_buf, t, r, "id_cred_i", vec_num, &c->id_cred_i);
	get_element(test_vec_buf, t, r, "cred_i", vec_num, &c->cred_i);
	get_element(test_vec_buf, t, r, "g_x_raw", vec_num, &c->g_x);
	get_element(test_vec_buf, t, r, "x_raw", vec_num, &c->x);
	get_element(test_vec_buf, t, r, "g_i_raw", vec_num, &c->g_i);
	get_element(test_vec_buf, t, r, "i_raw", vec_num, &c->i);

	return get_C_X_decode(C_I, vec_num, test_vec_buf, r, t, bufs->c_i,
			      sizeof(bufs->c_i), &c->c_i);
}

int get_EDHOC_RESPONDER_CONTEXT_from_test_vec(
	struct edhoc_responder_context_bufs *bufs,
	struct edhoc_responder_context *c, uint8_t vec_num, char *test_vec_buf,
	uint32_t test_vec_buf_len)
{
	int r;
	jsmn_parser p;
	jsmntok_t t[20000]; /* We expect no more than 20000 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, test_vec_buf, test_vec_buf_len, t,
		       sizeof(t) / sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return -1;
	}

	byte_array_init(bufs->suites_r, sizeof(bufs->suites_r), &c->suites_r);
	byte_array_init(bufs->g_y, sizeof(bufs->g_y), &c->g_y);
	byte_array_init(bufs->y, sizeof(bufs->y), &c->y);
	//byte_array_init(bufs->c_r, sizeof(bufs->c_r), &c->c_r);
	byte_array_init(bufs->g_r, sizeof(bufs->g_r), &c->g_r);
	byte_array_init(bufs->r, sizeof(bufs->r), &c->r);
	byte_array_init(bufs->ead_2, sizeof(bufs->ead_2), &c->ead_2);
	byte_array_init(bufs->id_cred_r, sizeof(bufs->id_cred_r),
			&c->id_cred_r);
	byte_array_init(bufs->cred_r, sizeof(bufs->cred_r), &c->cred_r);

	get_element(test_vec_buf, t, r, "suites_r", vec_num, &c->suites_r);
	//get_element(test_vec_buf, t, r, "c_r_raw", vec_num, &c->c_r);
	//get_element(test_vec_buf, t, r, "c_r", vec_num, &c->c_r);
	get_element(test_vec_buf, t, r, "ead_1", vec_num, &c->ead_2);
	get_element(test_vec_buf, t, r, "id_cred_r", vec_num, &c->id_cred_r);
	get_element(test_vec_buf, t, r, "cred_r", vec_num, &c->cred_r);
	get_element(test_vec_buf, t, r, "g_y_raw", vec_num, &c->g_y);
	get_element(test_vec_buf, t, r, "y_raw", vec_num, &c->y);
	get_element(test_vec_buf, t, r, "g_r_raw", vec_num, &c->g_r);
	get_element(test_vec_buf, t, r, "r_raw", vec_num, &c->r);
	//get_element(test_vec_buf, t, r, "sk_i", vec_num, &c->sk_i);

	return get_C_X_decode(C_R, vec_num, test_vec_buf, r, t, bufs->c_r,
			      sizeof(bufs->c_r), &c->c_r);
}