#ifndef TEST_VEC_PARSER_H
#define TEST_VEC_PARSER_H

#include <stdint.h>

#include "edhoc.h"

enum role { INITIATOR, RESPONDER };
enum C_X_TYPE { C_I, C_R };

struct other_party_cred_bufs {
	uint8_t id_cred[ID_CRED_DEFAULT_SIZE];
	uint8_t cred[CERT_DEFAUT_SIZE];
	uint8_t pk[PK_DEFAULT_SIZE];
	uint8_t g[G_X_DEFAULT_SIZE];
	uint8_t ca[5]; //todo check how is this defined in the new test vectors
	uint8_t ca_pk[G_X_DEFAULT_SIZE]; //todo check how is this defined in the new test vectors
};

struct edhoc_initiator_context_bufs {
	uint8_t c_i[C_I_DEFAULT_SIZE];
	int c_r_int;
	uint8_t suites_i[5];
	uint8_t ead_1[AD_DEFAULT_SIZE];
	uint8_t ead_3[AD_DEFAULT_SIZE];
	uint8_t id_cred_i[ID_CRED_DEFAULT_SIZE];
	uint8_t cred_i[CERT_DEFAUT_SIZE];
	uint8_t g_x[G_X_DEFAULT_SIZE];
	uint8_t x[G_X_DEFAULT_SIZE];
	uint8_t g_i[G_I_DEFAULT_SIZE];
	uint8_t i[G_I_DEFAULT_SIZE];
	uint8_t sk_i[PK_DEFAULT_SIZE];
	uint8_t pk_i[PK_DEFAULT_SIZE];
};

struct edhoc_responder_context_bufs {
	uint8_t c_r[C_I_DEFAULT_SIZE];
	int c_r_int;
	uint8_t suites_r[5];
	uint8_t g_y[G_X_DEFAULT_SIZE];
	uint8_t y[G_X_DEFAULT_SIZE];
	uint8_t g_r[G_I_DEFAULT_SIZE];
	uint8_t r[G_I_DEFAULT_SIZE];
	uint8_t ead_2[AD_DEFAULT_SIZE];
	uint8_t id_cred_r[ID_CRED_DEFAULT_SIZE];
	uint8_t cred_r[CERT_DEFAUT_SIZE];
	uint8_t sk_r[PK_DEFAULT_SIZE];
	uint8_t pk_r[PK_DEFAULT_SIZE];
};

/**
 * @brief       Reads the test vectors form a json file into a string 
 * @param       filename file containing the testvectors in JSON format
 * @param       test_vec_buf a buffer to hold the test vectors as json string
 * @param       len takes as argument the lenhgt of test_vec_buf and returns the actual lenhgt of the string.
 * @retval      0 if success; -1 otherwise 
 */
int read_test_vectors(char *filename, char *test_vec_buf, uint32_t *len);

/**
 * 
 * 
 */
int get_OTHER_PARTY_CRED_from_test_vec(enum role other_party_role,
				       struct other_party_cred_bufs *bufs,
				       struct other_party_cred *c,
				       uint8_t vec_num, char *test_vec_buf,
				       uint32_t len);
/**
 * 
 * 
 */
int get_EDHOC_INITIATOR_CONTEXT_from_test_vec(
	struct edhoc_initiator_context_bufs *bufs,
	struct edhoc_initiator_context *c, uint8_t vec_num, char *test_vec_buf,
	uint32_t test_vec_buf_len);

/**
 * 
 * 
 */
int get_EDHOC_RESPONDER_CONTEXT_from_test_vec(
	struct edhoc_responder_context_bufs *bufs,
	struct edhoc_responder_context *c, uint8_t vec_num, char *test_vec_buf,
	uint32_t test_vec_buf_len);
#endif