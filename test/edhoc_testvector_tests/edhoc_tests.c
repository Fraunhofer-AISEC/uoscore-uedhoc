/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

#include <stdio.h>
#include <zephyr.h>
#include <ztest.h>

#include <edhoc.h>
#include "txrx_wrapper.h"
#include "edhoc_tests.h"
//#include "../../samples/common/test_vec_parser.h"
#include "test_vectors.h"

struct messages_bufs m_bufs;
struct messages m;

//todo check that as well that the AD are correct
int test_edhoc(enum role p, uint8_t vec_num)
{
	/* oscore declarations*/
	uint8_t oscore_master_secret[16];
	uint8_t oscore_master_salt[8];
	/* edhoc declarations */
	uint8_t PRK_4x3m[PRK_DEFAULT_SIZE];
	uint8_t th4[SHA_DEFAULT_SIZE];
	uint8_t err_msg[ERR_MSG_DEFAULT_SIZE];
	uint32_t err_msg_len = sizeof(err_msg);
	uint8_t ad_1[AD_DEFAULT_SIZE];
	uint32_t ad_1_len = sizeof(ad_1);
	uint8_t ad_2[AD_DEFAULT_SIZE];
	uint32_t ad_2_len = sizeof(ad_2);
	uint8_t ad_3[AD_DEFAULT_SIZE];
	uint32_t ad_3_len = sizeof(ad_3);
	uint8_t ad_4[AD_DEFAULT_SIZE];
	uint32_t ad_4_len = sizeof(ad_2);
	/* test vector inputs */
	//char filename[] = { "../../test_vectors/edhoc-vectors-json_v11.txt" };
	//char test_vec_buf[1024 * 160];
	//uint32_t test_vec_buf_len = sizeof(test_vec_buf);
	//struct results_bufs res_bufs;
	struct results res;
	int r;
	enum err err;

	vec_num = vec_num - 1;

	rx_init();

	// r = read_test_vectors(filename, test_vec_buf, &test_vec_buf_len);
	// zassert_true(r == 0, "reading the testvectors failed");

	// r = get_RESULTS_from_test_vec(&res_bufs, &res, vec_num, test_vec_buf,
	// 			      test_vec_buf_len);
	// zassert_true(r == 0, "parsing results form testvectors failed");

	// r = get_MESSAGES_from_test_vec(&m_bufs, &m, vec_num, test_vec_buf,
	// 			       test_vec_buf_len);
	// zassert_true(r == 0, "parsing messages form testvectors failed");

	res.oscore_master_salt.len = test_vectors[vec_num].oscore_salt_raw_len;
	res.oscore_master_salt.ptr = test_vectors[vec_num].oscore_salt_raw;
	res.oscore_master_secret.len =
		test_vectors[vec_num].oscore_secret_raw_len;
	res.oscore_master_secret.ptr = test_vectors[vec_num].oscore_secret_raw;
	res.prk_4x3m.len = test_vectors[vec_num].prk_4x3m_raw_len;
	res.prk_4x3m.ptr = test_vectors[vec_num].prk_4x3m_raw;

	res.th4.len = test_vectors[vec_num].th_4_raw_len;
	res.th4.ptr = test_vectors[vec_num].th_4_raw;

	m.m1.len = test_vectors[vec_num].message_1_len;
	m.m1.ptr = test_vectors[vec_num].message_1;
	m.m2.len = test_vectors[vec_num].message_2_len;
	m.m2.ptr = test_vectors[vec_num].message_2;
	m.m3.len = test_vectors[vec_num].message_3_len;
	m.m3.ptr = test_vectors[vec_num].message_3;
	m.m4.len = test_vectors[vec_num].message_4_len;
	m.m4.ptr = test_vectors[vec_num].message_4;

	if (p == INITIATOR) {
		uint16_t cred_num = 1;
		struct other_party_cred cred_r;
		struct edhoc_initiator_context c_i;
		// struct other_party_cred_bufs other_party_bufs;
		// struct edhoc_initiator_context_bufs initiator_context_bufs;

		rx_initiator_switch = true;
		cred_r.id_cred.len = test_vectors[vec_num].id_cred_r_len;
		cred_r.id_cred.ptr = test_vectors[vec_num].id_cred_r;
		cred_r.cred.len = test_vectors[vec_num].cred_r_len;
		cred_r.cred.ptr = test_vectors[vec_num].cred_r;
		cred_r.g.len = test_vectors[vec_num].g_r_raw_len;
		cred_r.g.ptr = test_vectors[vec_num].g_r_raw;
		cred_r.pk.len = test_vectors[vec_num].pk_r_raw_len;
		cred_r.pk.ptr = test_vectors[vec_num].pk_r_raw;
		cred_r.ca.len = test_vectors[vec_num].ca_len;
		cred_r.ca.ptr = test_vectors[vec_num].ca;
		cred_r.ca_pk.len = test_vectors[vec_num].ca_pk_len;
		cred_r.ca_pk.ptr = test_vectors[vec_num].ca_pk;

		//todo check how to set up c_i and ead
		//c_i.c_i.mem
		c_i.msg4 = true; /*we allways test message 4 */
		//c_i.c_i.mem
		c_i.method = *test_vectors[vec_num].method;

		c_i.suites_i.len = test_vectors[vec_num].suites_i_len;
		c_i.suites_i.ptr = test_vectors[vec_num].suites_i;

		c_i.ead_1.len = test_vectors[vec_num].ead_1_len;
		c_i.ead_1.ptr = test_vectors[vec_num].ead_1;

		c_i.ead_3.len = test_vectors[vec_num].ead_3_len;
		c_i.ead_3.ptr = test_vectors[vec_num].ead_3;

		c_i.id_cred_i.len = test_vectors[vec_num].id_cred_i_len;
		c_i.id_cred_i.ptr = test_vectors[vec_num].id_cred_i;

		c_i.cred_i.len = test_vectors[vec_num].cred_i_len;
		c_i.cred_i.ptr = test_vectors[vec_num].cred_i;

		c_i.g_x.len = test_vectors[vec_num].g_x_raw_len;
		c_i.g_x.ptr = test_vectors[vec_num].g_x_raw;

		c_i.x.len = test_vectors[vec_num].x_raw_len;
		c_i.x.ptr = test_vectors[vec_num].x_raw;

		c_i.g_i.len = test_vectors[vec_num].g_i_raw_len;
		c_i.g_i.ptr = test_vectors[vec_num].g_i_raw;

		c_i.i.len = test_vectors[vec_num].i_raw_len;
		c_i.i.ptr = test_vectors[vec_num].i_raw;

		c_i.sk_i.len = test_vectors[vec_num].sk_i_raw_len;
		c_i.sk_i.ptr = test_vectors[vec_num].sk_i_raw;

		c_i.pk_i.len = test_vectors[vec_num].pk_i_raw_len;
		c_i.pk_i.ptr = test_vectors[vec_num].pk_i_raw;

		//  r = get_OTHER_PARTY_CRED_from_test_vec(
		//  	RESPONDER, &other_party_bufs, &cred_r, vec_num,
		//  	test_vec_buf, test_vec_buf_len);
		// zassert_true(
		// 	r == 0,
		// 	"parsing other party credentials form testvectors failed");

		// r = get_EDHOC_INITIATOR_CONTEXT_from_test_vec(
		// 	&initiator_context_bufs, &c_i, vec_num, test_vec_buf,
		// 	test_vec_buf_len);
		// zassert_true(
		// 	r == 0,
		// 	"parsing initiator context form testvectors failed");

		err = edhoc_initiator_run(&c_i, &cred_r, cred_num, err_msg,
					  &err_msg_len, ad_2, &ad_2_len, ad_4,
					  &ad_4_len, PRK_4x3m, sizeof(PRK_4x3m),
					  th4, sizeof(th4), tx, rx);
		zassert_true(err == 0, "edhoc_initiator_run failed");

	} else {
		const uint16_t num_cred_i_elements = 1;
		struct other_party_cred cred_i;
		struct edhoc_responder_context c_r;
		struct other_party_cred_bufs other_party_bufs;
		struct edhoc_responder_context_bufs responder_context_bufs;

		rx_initiator_switch = false;
		cred_i.id_cred.len = test_vectors[vec_num].id_cred_i_len;
		cred_i.id_cred.ptr = test_vectors[vec_num].id_cred_i;
		cred_i.g.len = test_vectors[vec_num].g_i_raw_len;
		cred_i.g.ptr = test_vectors[vec_num].g_i_raw;
		cred_i.pk.len = test_vectors[vec_num].pk_i_raw_len;
		cred_i.pk.ptr = test_vectors[vec_num].pk_i_raw;
		cred_i.cred.len = test_vectors[vec_num].cred_i_len;
		cred_i.cred.ptr = test_vectors[vec_num].cred_i;
		cred_i.ca.len = test_vectors[vec_num].ca_len;
		cred_i.ca.ptr = test_vectors[vec_num].ca;
		cred_i.ca_pk.len = test_vectors[vec_num].ca_pk_len;
		cred_i.ca_pk.ptr = test_vectors[vec_num].ca_pk;

		//todo check how to set up c_i and ead
		//c_i.c_i.mem
		c_r.msg4 = true; /*we allways test message 4 */
		//c_i.c_i.mem

		c_r.suites_r.len = test_vectors[vec_num].suites_r_len;
		c_r.suites_r.ptr = test_vectors[vec_num].suites_r;

		c_r.ead_1.len = test_vectors[vec_num].ead_2_len;
		c_r.ead_1.ptr = test_vectors[vec_num].ead_2;

		c_r.ead_3.len = test_vectors[vec_num].ead_4_len;
		c_r.ead_3.ptr = test_vectors[vec_num].ead_4;

		c_r.id_cred_r.len = test_vectors[vec_num].id_cred_r_len;
		c_r.id_cred_r.ptr = test_vectors[vec_num].id_cred_r;

		c_r.cred_r.len = test_vectors[vec_num].cred_r_len;
		c_r.cred_r.ptr = test_vectors[vec_num].cred_r;

		c_r.g_y.len = test_vectors[vec_num].g_y_raw_len;
		c_r.g_y.ptr = test_vectors[vec_num].g_y_raw;

		c_r.y.len = test_vectors[vec_num].y_raw_len;
		c_r.y.ptr = test_vectors[vec_num].y_raw;

		c_r.g_r.len = test_vectors[vec_num].g_r_raw_len;
		c_r.g_r.ptr = test_vectors[vec_num].g_r_raw;

		c_r.r.len = test_vectors[vec_num].r_raw_len;
		c_r.r.ptr = test_vectors[vec_num].r_raw;

		c_r.sk_r.len = test_vectors[vec_num].sk_r_raw_len;
		c_r.sk_r.ptr = test_vectors[vec_num].sk_r_raw;

		c_r.pk_r.len = test_vectors[vec_num].pk_r_raw_len;
		c_r.pk_r.ptr = test_vectors[vec_num].pk_r_raw;

		// r = get_OTHER_PARTY_CRED_from_test_vec(
		// 	INITIATOR, &other_party_bufs, &cred_i, vec_num,
		// 	test_vec_buf, test_vec_buf_len);
		// zassert_true(
		// 	r == 0,
		// 	"parsing other party credentials form testvectors failed");

		// r = get_EDHOC_RESPONDER_CONTEXT_from_test_vec(
		// 	&responder_context_bufs, &c_r, vec_num, test_vec_buf,
		// 	test_vec_buf_len);
		// zassert_true(
		// 	r == 0,
		// 	"parsing responder context form testvectors failed");

		err = edhoc_responder_run(&c_r, &cred_i, num_cred_i_elements,
					  err_msg, &err_msg_len,
					  (uint8_t *)&ad_1, &ad_1_len,
					  (uint8_t *)&ad_3, &ad_3_len, PRK_4x3m,
					  sizeof(PRK_4x3m), th4, sizeof(th4),
					  tx, rx);
		zassert_true(err == 0, "edhoc_responder_run failed");
	}

	err = edhoc_exporter(SHA_256, PRK_4x3m, sizeof(PRK_4x3m), th4,
			     sizeof(th4), "OSCORE_Master_Secret",
			     oscore_master_secret, 16);

	zassert_true(err == 0, "edhoc_exporter failed");

	err = edhoc_exporter(SHA_256, PRK_4x3m, sizeof(PRK_4x3m), th4,
			     sizeof(th4), "OSCORE_Master_Salt",
			     oscore_master_salt, 8);

	zassert_true(err == 0, "edhoc_exporter failed");

	/* check th4, PRK_4x3m, OSCORE Master secret and salt are correct */
	zassert_mem_equal__(&PRK_4x3m, res.prk_4x3m.ptr, sizeof(PRK_4x3m),
			    "wrong PRK_4x3m");
	zassert_mem_equal__(&th4, res.th4.ptr, sizeof(th4), "wrong TH4");
	zassert_mem_equal__(&oscore_master_secret, res.oscore_master_secret.ptr,
			    sizeof(oscore_master_secret),
			    "wrong OSCORE Master Secret");
	zassert_mem_equal__(&oscore_master_salt, res.oscore_master_salt.ptr,
			    sizeof(oscore_master_salt),
			    "wrong OSCORE Master Salt");
	return 0;
}
