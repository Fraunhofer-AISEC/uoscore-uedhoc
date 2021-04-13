/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

#include "main.h"

#include <stdio.h>
#include <zephyr.h>
#include <ztest.h>

#ifdef EDHOC_TESTS
#include <edhoc.h>

#include "test_vectors_edhoc.h"
#include "txrx_wrapper.h"

enum party_type { INITIATOR, RESPONDER };
enum test { T1, T2, T3, T4 };

struct expected_result {
	uint8_t *prk_4x3m;
	uint8_t *th4;
	uint8_t *oscore_master_secret;
	uint8_t *oscore_master_salt;
};

uint8_t ad_1[AD_DEFAULT_SIZE];
uint64_t ad_1_len = sizeof(ad_1);
uint8_t ad_2[AD_DEFAULT_SIZE];
uint64_t ad_2_len = sizeof(ad_2);
uint8_t ad_3[AD_DEFAULT_SIZE];
uint64_t ad_3_len = sizeof(ad_3);
uint8_t err_msg[ERR_MSG_DEFAULT_SIZE];
uint32_t err_msg_len = sizeof(err_msg);
uint8_t PRK_4x3m[PRK_DEFAULT_SIZE];
uint8_t th4[SHA_DEFAULT_SIZE];
uint8_t oscore_master_secret[16];
uint8_t oscore_master_salt[8];

volatile struct byte_array M1, M2, M3;

static void init_test_messages(enum party_type t, enum test test)
{
	if (t == INITIATOR) {
		rx_initiator_switch = true;
	} else {
		rx_initiator_switch = false;
	}

	switch (test) {
	case T1:
		M1.ptr = T1_MSG_1;
		M1.len = T1_MSG_1_LEN;
		M2.ptr = T1_MSG_2;
		M2.len = T1_MSG_2_LEN;
		M3.ptr = T1_MSG_3;
		M3.len = T1_MSG_3_LEN;
		break;
	case T2:
		M1.ptr = T2_MSG_1;
		M1.len = T2_MSG_1_LEN;
		M2.ptr = T2_MSG_2;
		M2.len = T2_MSG_2_LEN;
		M3.ptr = T2_MSG_3;
		M3.len = T2_MSG_3_LEN;
		break;
	case T3:
		M1.ptr = T3_MSG_1;
		M1.len = T3_MSG_1_LEN;
		M2.ptr = T3_MSG_2;
		M2.len = T3_MSG_2_LEN;
		M3.ptr = T3_MSG_3;
		M3.len = T3_MSG_3_LEN;
		break;
	case T4:
		M1.ptr = T4_MSG_1;
		M1.len = T4_MSG_1_LEN;
		M2.ptr = T4_MSG_2;
		M2.len = T4_MSG_2_LEN;
		M3.ptr = T4_MSG_3;
		M3.len = T4_MSG_3_LEN;
		break;
	default:
		break;
	}
}
static void init_expected_result(struct expected_result *e, enum test t)
{
	switch (t) {
	case T1:
		e->prk_4x3m = T1_PRK_4X3M;
		e->th4 = T1_TH4;
		e->oscore_master_secret = T1_OSCORE_MSECRET;
		e->oscore_master_salt = T1_OSCORE_MSALT;
		break;
	case T2:
		e->prk_4x3m = T2_PRK_4X3M;
		e->th4 = T2_TH4;
		e->oscore_master_secret = T2_OSCORE_MSECRET;
		e->oscore_master_salt = T2_OSCORE_MSALT;
		break;
	case T3:
		e->prk_4x3m = T3_PRK_4X3M;
		e->th4 = T3_TH4;
		e->oscore_master_secret = T3_OSCORE_MSECRET;
		e->oscore_master_salt = T3_OSCORE_MSALT;
		break;
	case T4:
		e->prk_4x3m = T4_PRK_4X3M;
		e->th4 = T4_TH4;
		e->oscore_master_secret = T4_OSCORE_MSECRET;
		e->oscore_master_salt = T4_OSCORE_MSALT;
		break;
	default:
		break;
	}
}

static void init_other_party_cred_r(struct other_party_cred *a, enum test test)
{
	switch (test) {
	case T1:
		a->ca.ptr = T1I__CA;
		a->ca.len = T1I__CA_LEN;
		a->ca_pk.ptr = T1I__CA_PK;
		a->ca_pk.len = T1I__CA_PK_LEN;
		a->cred.ptr = T1I__CRED_R;
		a->cred.len = T1I__CRED_R_LEN;
		a->g.ptr = T1I__G_R;
		a->g.len = T1I__G_R_LEN;
		a->id_cred.ptr = T1I__ID_CRED_R;
		a->id_cred.len = T1I__ID_CRED_R_LEN;
		a->pk.ptr = T1I__PK_R;
		a->pk.len = T1I__PK_R_LEN;
		break;
	case T2:
		a->ca.ptr = T2I__CA;
		a->ca.len = T2I__CA_LEN;
		a->ca_pk.ptr = T2I__CA_PK;
		a->ca_pk.len = T2I__CA_PK_LEN;
		a->cred.ptr = T2I__CRED_R;
		a->cred.len = T2I__CRED_R_LEN;
		a->g.ptr = T2I__G_R;
		a->g.len = T2I__G_R_LEN;
		a->id_cred.ptr = T2I__ID_CRED_R;
		a->id_cred.len = T2I__ID_CRED_R_LEN;
		a->pk.ptr = T2I__PK_R;
		a->pk.len = T2I__PK_R_LEN;
		break;
	case T3:
		a->ca.ptr = T3I__CA;
		a->ca.len = T3I__CA_LEN;
		a->ca_pk.ptr = T3I__CA_PK;
		a->ca_pk.len = T3I__CA_PK_LEN;
		a->cred.ptr = T3I__CRED_R;
		a->cred.len = T3I__CRED_R_LEN;
		a->g.ptr = T3I__G_R;
		a->g.len = T3I__G_R_LEN;
		a->id_cred.ptr = T3I__ID_CRED_R;
		a->id_cred.len = T3I__ID_CRED_R_LEN;
		a->pk.ptr = T3I__PK_R;
		a->pk.len = T3I__PK_R_LEN;
		break;
	case T4:
		a->ca.ptr = T4I__CA;
		a->ca.len = T4I__CA_LEN;
		a->ca_pk.ptr = T4I__CA_PK;
		a->ca_pk.len = T4I__CA_PK_LEN;
		a->cred.ptr = T4I__CRED_R;
		a->cred.len = T4I__CRED_R_LEN;
		a->g.ptr = T4I__G_R;
		a->g.len = T4I__G_R_LEN;
		a->id_cred.ptr = T4I__ID_CRED_R;
		a->id_cred.len = T4I__ID_CRED_R_LEN;
		a->pk.ptr = T4I__PK_R;
		a->pk.len = T4I__PK_R_LEN;
		break;
	default:
		break;
	}
}

static void init_other_party_cred_i(struct other_party_cred *a, enum test test)
{
	switch (test) {
	case T1:
		a->id_cred.ptr = T1R__ID_CRED_I;
		a->id_cred.len = T1R__ID_CRED_I_LEN;
		a->cred.ptr = T1R__CRED_I;
		a->cred.len = T1R__CRED_I_LEN;
		a->pk.ptr = T1R__PK_I;
		a->pk.len = T1R__PK_I_LEN;
		a->g.ptr = T1R__G_I;
		a->g.len = T1R__G_I_LEN;
		a->ca.ptr = T1R__CA;
		a->ca.len = T1R__CA_LEN;
		a->ca_pk.ptr = T1R__CA_PK;
		a->ca_pk.len = T1R__CA_PK_LEN;
		break;
	case T2:
		a->id_cred.ptr = T2R__ID_CRED_I;
		a->id_cred.len = T2R__ID_CRED_I_LEN;
		a->cred.ptr = T2R__CRED_I;
		a->cred.len = T2R__CRED_I_LEN;
		a->pk.ptr = T2R__PK_I;
		a->pk.len = T2R__PK_I_LEN;
		a->g.ptr = T2R__G_I;
		a->g.len = T2R__G_I_LEN;
		a->ca.ptr = T2R__CA;
		a->ca.len = T2R__CA_LEN;
		a->ca_pk.ptr = T2R__CA_PK;
		a->ca_pk.len = T2R__CA_PK_LEN;
		break;
	case T3:
		a->id_cred.ptr = T3R__ID_CRED_I;
		a->id_cred.len = T3R__ID_CRED_I_LEN;
		a->cred.ptr = T3R__CRED_I;
		a->cred.len = T3R__CRED_I_LEN;
		a->pk.ptr = T3R__PK_I;
		a->pk.len = T3R__PK_I_LEN;
		a->g.ptr = T3R__G_I;
		a->g.len = T3R__G_I_LEN;
		a->ca.ptr = T3R__CA;
		a->ca.len = T3R__CA_LEN;
		a->ca_pk.ptr = T3R__CA_PK;
		a->ca_pk.len = T3R__CA_PK_LEN;
		break;
	case T4:
		a->id_cred.ptr = T4R__ID_CRED_I;
		a->id_cred.len = T4R__ID_CRED_I_LEN;
		a->cred.ptr = T4R__CRED_I;
		a->cred.len = T4R__CRED_I_LEN;
		a->pk.ptr = T4R__PK_I;
		a->pk.len = T4R__PK_I_LEN;
		a->g.ptr = T4R__G_I;
		a->g.len = T4R__G_I_LEN;
		a->ca.ptr = T4R__CA;
		a->ca.len = T4R__CA_LEN;
		a->ca_pk.ptr = T4R__CA_PK;
		a->ca_pk.len = T4R__CA_PK_LEN;
		break;
	default:
		break;
	}
}

static void init_edhoc_initiator_context(struct edhoc_initiator_context *a,
					 enum test test)
{
	switch (test) {
	case T1:
		a->method_type = T1I__METHOD_TYPE;
		a->corr = T1I__CORR;
		a->suites_i.ptr = T1I__SUITES_I;
		a->suites_i.len = T1I__SUITES_I_LEN;
		a->g_x.ptr = T1I__G_X;
		a->g_x.len = T1I__G_X_LEN;
		a->x.ptr = T1I__X;
		a->x.len = T1I__X_LEN;
		a->c_i.ptr = T1I__C_I;
		a->c_i.len = T1I__C_I_LEN;
		a->ad_1.ptr = T1I__AD_1;
		a->ad_1.len = T1I__AD_1_LEN;
		a->g_i.ptr = T1I__G_I;
		a->g_i.len = T1I__G_I_LEN;
		a->i.ptr = T1I__I;
		a->i.len = T1I__I_LEN;
		a->ad_3.ptr = T1I__AD_3;
		a->ad_3.len = T1I__AD_3_LEN;
		a->id_cred_i.ptr = T1I__ID_CRED_I;
		a->id_cred_i.len = T1I__ID_CRED_I_LEN;
		a->cred_i.ptr = T1I__CRED_I;
		a->cred_i.len = T1I__CRED_I_LEN;
		a->sk_i.ptr = T1I__SK_I;
		a->sk_i.len = T1I__SK_I_LEN;
		a->pk_i.ptr = T1I__PK_I;
		a->pk_i.len = T1I__PK_I_LEN;
		break;
	case T2:
		a->method_type = T2I__METHOD_TYPE;
		a->corr = T2I__CORR;
		a->suites_i.ptr = T2I__SUITES_I;
		a->suites_i.len = T2I__SUITES_I_LEN;
		a->g_x.ptr = T2I__G_X;
		a->g_x.len = T2I__G_X_LEN;
		a->x.ptr = T2I__X;
		a->x.len = T2I__X_LEN;
		a->c_i.ptr = T2I__C_I;
		a->c_i.len = T2I__C_I_LEN;
		a->ad_1.ptr = T2I__AD_1;
		a->ad_1.len = T2I__AD_1_LEN;
		a->g_i.ptr = T2I__G_I;
		a->g_i.len = T2I__G_I_LEN;
		a->i.ptr = T2I__I;
		a->i.len = T2I__I_LEN;
		a->ad_3.ptr = T2I__AD_3;
		a->ad_3.len = T2I__AD_3_LEN;
		a->id_cred_i.ptr = T2I__ID_CRED_I;
		a->id_cred_i.len = T2I__ID_CRED_I_LEN;
		a->cred_i.ptr = T2I__CRED_I;
		a->cred_i.len = T2I__CRED_I_LEN;
		a->sk_i.ptr = T2I__SK_I;
		a->sk_i.len = T2I__SK_I_LEN;
		a->pk_i.ptr = T2I__PK_I;
		a->pk_i.len = T2I__PK_I_LEN;
		break;
	case T3:
		a->method_type = T3I__METHOD_TYPE;
		a->corr = T3I__CORR;
		a->suites_i.ptr = T3I__SUITES_I;
		a->suites_i.len = T3I__SUITES_I_LEN;
		a->g_x.ptr = T3I__G_X;
		a->g_x.len = T3I__G_X_LEN;
		a->x.ptr = T3I__X;
		a->x.len = T3I__X_LEN;
		a->c_i.ptr = T3I__C_I;
		a->c_i.len = T3I__C_I_LEN;
		a->ad_1.ptr = T3I__AD_1;
		a->ad_1.len = T3I__AD_1_LEN;
		a->g_i.ptr = T3I__G_I;
		a->g_i.len = T3I__G_I_LEN;
		a->i.ptr = T3I__I;
		a->i.len = T3I__I_LEN;
		a->ad_3.ptr = T3I__AD_3;
		a->ad_3.len = T3I__AD_3_LEN;
		a->id_cred_i.ptr = T3I__ID_CRED_I;
		a->id_cred_i.len = T3I__ID_CRED_I_LEN;
		a->cred_i.ptr = T3I__CRED_I;
		a->cred_i.len = T3I__CRED_I_LEN;
		a->sk_i.ptr = T3I__SK_I;
		a->sk_i.len = T3I__SK_I_LEN;
		a->pk_i.ptr = T3I__PK_I;
		a->pk_i.len = T3I__PK_I_LEN;
		break;
	case T4:
		a->method_type = T4I__METHOD_TYPE;
		a->corr = T4I__CORR;
		a->suites_i.ptr = T4I__SUITES_I;
		a->suites_i.len = T4I__SUITES_I_LEN;
		a->g_x.ptr = T4I__G_X;
		a->g_x.len = T4I__G_X_LEN;
		a->x.ptr = T4I__X;
		a->x.len = T4I__X_LEN;
		a->c_i.ptr = T4I__C_I;
		a->c_i.len = T4I__C_I_LEN;
		a->ad_1.ptr = T4I__AD_1;
		a->ad_1.len = T4I__AD_1_LEN;
		a->g_i.ptr = T4I__G_I;
		a->g_i.len = T4I__G_I_LEN;
		a->i.ptr = T4I__I;
		a->i.len = T4I__I_LEN;
		a->ad_3.ptr = T4I__AD_3;
		a->ad_3.len = T4I__AD_3_LEN;
		a->id_cred_i.ptr = T4I__ID_CRED_I;
		a->id_cred_i.len = T4I__ID_CRED_I_LEN;
		a->cred_i.ptr = T4I__CRED_I;
		a->cred_i.len = T4I__CRED_I_LEN;
		a->sk_i.ptr = T4I__SK_I;
		a->sk_i.len = T4I__SK_I_LEN;
		a->pk_i.ptr = T4I__PK_I;
		a->pk_i.len = T4I__PK_I_LEN;
		break;
	default:
		break;
	}
}

static void init_edhoc_responder_context(struct edhoc_responder_context *a,
					 enum test test)
{
	switch (test) {
	case T1:
		a->suites_r.ptr = T1R__SUITES_R;
		a->suites_r.len = T1R__SUITES_R_LEN;
		a->g_y.ptr = T1R__G_Y;
		a->g_y.len = T1R__G_Y_LEN;
		a->y.ptr = T1R__Y;
		a->y.len = T1R__Y_LEN;
		a->c_r.ptr = T1R__C_R;
		a->c_r.len = T1R__C_R_LEN;
		a->ad_2.ptr = T1R__AD_2;
		a->ad_2.len = T1R__AD_2_LEN;
		a->id_cred_r.ptr = T1R__ID_CRED_R;
		a->id_cred_r.len = T1R__ID_CRED_R_LEN;
		a->cred_r.ptr = T1R__CRED_R;
		a->cred_r.len = T1R__CRED_R_LEN;
		a->sk_r.ptr = T1R__SK_R;
		a->sk_r.len = T1R__SK_R_LEN;
		a->pk_r.ptr = T1R__PK_R;
		a->pk_r.len = T1R__PK_R_LEN;
		a->r.ptr = T1R__R;
		a->r.len = T1R__R_LEN;
		a->g_r.ptr = T1R__G_R;
		a->g_r.len = T1R__G_R_LEN;
		break;
	case T2:
		a->suites_r.ptr = T2R__SUITES_R;
		a->suites_r.len = T2R__SUITES_R_LEN;
		a->g_y.ptr = T2R__G_Y;
		a->g_y.len = T2R__G_Y_LEN;
		a->y.ptr = T2R__Y;
		a->y.len = T2R__Y_LEN;
		a->c_r.ptr = T2R__C_R;
		a->c_r.len = T2R__C_R_LEN;
		a->ad_2.ptr = T2R__AD_2;
		a->ad_2.len = T2R__AD_2_LEN;
		a->id_cred_r.ptr = T2R__ID_CRED_R;
		a->id_cred_r.len = T2R__ID_CRED_R_LEN;
		a->cred_r.ptr = T2R__CRED_R;
		a->cred_r.len = T2R__CRED_R_LEN;
		a->sk_r.ptr = T2R__SK_R;
		a->sk_r.len = T2R__SK_R_LEN;
		a->pk_r.ptr = T2R__PK_R;
		a->pk_r.len = T2R__PK_R_LEN;
		a->r.ptr = T2R__R;
		a->r.len = T2R__R_LEN;
		a->g_r.ptr = T2R__G_R;
		a->g_r.len = T2R__G_R_LEN;
		break;
	case T3:
		a->suites_r.ptr = T3R__SUITES_R;
		a->suites_r.len = T3R__SUITES_R_LEN;
		a->g_y.ptr = T3R__G_Y;
		a->g_y.len = T3R__G_Y_LEN;
		a->y.ptr = T3R__Y;
		a->y.len = T3R__Y_LEN;
		a->c_r.ptr = T3R__C_R;
		a->c_r.len = T3R__C_R_LEN;
		a->ad_2.ptr = T3R__AD_2;
		a->ad_2.len = T3R__AD_2_LEN;
		a->id_cred_r.ptr = T3R__ID_CRED_R;
		a->id_cred_r.len = T3R__ID_CRED_R_LEN;
		a->cred_r.ptr = T3R__CRED_R;
		a->cred_r.len = T3R__CRED_R_LEN;
		a->sk_r.ptr = T3R__SK_R;
		a->sk_r.len = T3R__SK_R_LEN;
		a->pk_r.ptr = T3R__PK_R;
		a->pk_r.len = T3R__PK_R_LEN;
		a->r.ptr = T3R__R;
		a->r.len = T3R__R_LEN;
		a->g_r.ptr = T3R__G_R;
		a->g_r.len = T3R__G_R_LEN;
		break;
	case T4:
		a->suites_r.ptr = T4R__SUITES_R;
		a->suites_r.len = T4R__SUITES_R_LEN;
		a->g_y.ptr = T4R__G_Y;
		a->g_y.len = T4R__G_Y_LEN;
		a->y.ptr = T4R__Y;
		a->y.len = T4R__Y_LEN;
		a->c_r.ptr = T4R__C_R;
		a->c_r.len = T4R__C_R_LEN;
		a->ad_2.ptr = T4R__AD_2;
		a->ad_2.len = T4R__AD_2_LEN;
		a->id_cred_r.ptr = T4R__ID_CRED_R;
		a->id_cred_r.len = T4R__ID_CRED_R_LEN;
		a->cred_r.ptr = T4R__CRED_R;
		a->cred_r.len = T4R__CRED_R_LEN;
		a->sk_r.ptr = T4R__SK_R;
		a->sk_r.len = T4R__SK_R_LEN;
		a->pk_r.ptr = T4R__PK_R;
		a->pk_r.len = T4R__PK_R_LEN;
		a->r.ptr = T4R__R;
		a->r.len = T4R__R_LEN;
		a->g_r.ptr = T4R__G_R;
		a->g_r.len = T4R__G_R_LEN;
		break;
	default:
		break;
	}
}

static void test_edhoc(enum party_type p, enum test t)
{
	EdhocError r;
	init_test_messages(p, t);
	struct expected_result e;
	init_expected_result(&e, t);

	if (p == INITIATOR) {
		const uint16_t num_cred_r_elements = 1;
		struct other_party_cred cred_r;
		init_other_party_cred_r(&cred_r, t);
		struct edhoc_initiator_context c_i;
		init_edhoc_initiator_context(&c_i, t);

		r = edhoc_initiator_run(&c_i, &cred_r, num_cred_r_elements,
					err_msg, &err_msg_len, ad_2, &ad_2_len,
					PRK_4x3m, sizeof(PRK_4x3m), th4,
					sizeof(th4));
	} else {
		const uint16_t num_cred_i_elements = 1;
		struct other_party_cred cred_i;
		init_other_party_cred_i(&cred_i, t);
		struct edhoc_responder_context c_r;
		init_edhoc_responder_context(&c_r, t);

		r = edhoc_responder_run(&c_r, &cred_i, num_cred_i_elements,
					err_msg, &err_msg_len, (uint8_t *)&ad_1,
					&ad_1_len, (uint8_t *)&ad_3, &ad_3_len,
					PRK_4x3m, sizeof(PRK_4x3m), th4,
					sizeof(th4));
	}
	if (r != EdhocNoError)
		printf("error responder run (Error Code %d)\n", r);

	edhoc_exporter(SHA_256, AES_CCM_16_64_128, PRK_4x3m, sizeof(PRK_4x3m),
		       th4, sizeof(th4), "OSCORE Master Secret",
		       oscore_master_secret, 16);

	edhoc_exporter(SHA_256, AES_CCM_16_64_128, PRK_4x3m, sizeof(PRK_4x3m),
		       th4, sizeof(th4), "OSCORE Master Salt",
		       oscore_master_salt, 8);

	// check th4, PRK_4x3m, OSCORE Master secret and salt are correct
	zassert_mem_equal__(&PRK_4x3m, e.prk_4x3m, sizeof(PRK_4x3m),
			    "wrong PRK_4x3m");
	zassert_mem_equal__(&th4, e.th4, sizeof(th4), "wrong TH4");
	zassert_mem_equal__(&oscore_master_secret, e.oscore_master_secret,
			    sizeof(oscore_master_secret),
			    "wrong OSCORE Master Secret");
	zassert_mem_equal__(&oscore_master_salt, e.oscore_master_salt,
			    sizeof(oscore_master_salt),
			    "wrong OSCORE Master Salt");
}

/*
 * Test 1
 * Test No: |mode                          | RPK/Cert | suite | Ref [1]
 *        1 |INITIATOR_SK_RESPONDER_SK     | RPK x5t  | 0     | 1-290
 *        2 |INITIATOR_SDHK_RESPONDER_SDHK | RPK kid  | 0     | 292-540
 *        3 |INITIATOR_SK_RESPONDER_SK     | x5chain  | 0     | none
 *        4 |INITIATOR_SDHK_RESPONDER_SDHK | x5chain  | 0     | none
 * [1]:https://github.com/lake-wg/edhoc/blob/master/test-vectors/vectors.txt
 */
static void test_initiator1(void)
{
	test_edhoc(INITIATOR, T1);
}
static void test_initiator2(void)
{
	test_edhoc(INITIATOR, T2);
}
static void test_initiator3(void)
{
	test_edhoc(INITIATOR, T3);
}
static void test_initiator4(void)
{
	test_edhoc(INITIATOR, T4);
}

static void test_responder1(void)
{
	test_edhoc(RESPONDER, T1);
}
static void test_responder2(void)
{
	test_edhoc(RESPONDER, T2);
}
static void test_responder3(void)
{
	test_edhoc(RESPONDER, T3);
}
static void test_responder4(void)
{
	test_edhoc(RESPONDER, T4);
}

#endif

#ifdef OSCORE_TESTS
#include <oscore.h>

#include "test_vectors_oscore.h"

/**
 * Test 1:
 * - Client Key derivation with master salt see RFC8613 Appendix C.1.1
 * - Generating OSCORE request with key form C.1.1 see RFC8613 Appendix C.4
 */
static void oscore_client_test1(void)
{
	OscoreError r;
	struct context c_client;
	struct oscore_init_params params = {
		.dev_type = CLIENT,
		.master_secret.ptr = T1__MASTER_SECRET,
		.master_secret.len = T1__MASTER_SECRET_LEN,
		.sender_id.ptr = T1__SENDER_ID,
		.sender_id.len = T1__SENDER_ID_LEN,
		.recipient_id.ptr = T1__RECIPIENT_ID,
		.recipient_id.len = T1__RECIPIENT_ID_LEN,
		.master_salt.ptr = T1__MASTER_SALT,
		.master_salt.len = T1__MASTER_SALT_LEN,
		.id_context.ptr = T1__ID_CONTEXT,
		.id_context.len = T1__ID_CONTEXT_LEN,
		.aead_alg = AES_CCM_16_64_128,
		.hkdf = SHA_256,
	};

	r = oscore_context_init(&params, &c_client);

	zassert_equal(r, OscoreNoError, "Error in oscore_context_init");

	/* 
    required only for the test vector.
    during normal operation the sender sequence number is
    increased automatically after every sending
    */
	c_client.sc.sender_seq_num = 20;

	uint8_t buf_oscore[256];
	uint16_t buf_oscore_len = sizeof(buf_oscore);

	r = coap2oscore(T1__COAP_REQ, T1__COAP_REQ_LEN, (uint8_t *)&buf_oscore,
			&buf_oscore_len, &c_client);
	zassert_equal(r, OscoreNoError, "Error in coap2oscore!");

	zassert_mem_equal__(c_client.sc.sender_key.ptr, T1__SENDER_KEY,
			    c_client.sc.sender_key.len,
			    "T1 sender key derivation failed");

	zassert_mem_equal__(c_client.rc.recipient_key.ptr, T1__RECIPIENT_KEY,
			    c_client.rc.recipient_key.len,
			    "T1 recipient key derivation failed");

	zassert_mem_equal__(c_client.cc.common_iv.ptr, T1__COMMON_IV,
			    c_client.cc.common_iv.len,
			    "T1 common IV derivation failed");

	zassert_mem_equal__(&buf_oscore, T1__OSCORE_REQ, T1__OSCORE_REQ_LEN,
			    "coap2oscore failed");
}

/**
 * Test 3:
 * - Client Key derivation without master salt see RFC8613 Appendix C.2.1
 * - Generating OSCORE request with key form C.2.1 see RFC8613 Appendix C.5
 */
static void oscore_client_test3(void)
{
	OscoreError r;
	struct context c_client;
	struct oscore_init_params params = {
		.dev_type = CLIENT,
		.master_secret.ptr = T3__MASTER_SECRET,
		.master_secret.len = T3__MASTER_SECRET_LEN,
		.sender_id.ptr = T3__SENDER_ID,
		.sender_id.len = T3__SENDER_ID_LEN,
		.recipient_id.ptr = T3__RECIPIENT_ID,
		.recipient_id.len = T3__RECIPIENT_ID_LEN,
		.master_salt.ptr = T3__MASTER_SALT,
		.master_salt.len = T3__MASTER_SALT_LEN,
		.id_context.ptr = T3__ID_CONTEXT,
		.id_context.len = T3__ID_CONTEXT_LEN,
		.aead_alg = AES_CCM_16_64_128,
		.hkdf = SHA_256,
	};

	r = oscore_context_init(&params, &c_client);

	zassert_equal(r, OscoreNoError, "Error in oscore_context_init");

	/* 
    required only for the test vector.
    during normal operation the sender sequence number is
    increased automatically after every sending
    */
	c_client.sc.sender_seq_num = 20;

	uint8_t buf_oscore[256];
	uint16_t buf_oscore_len = sizeof(buf_oscore);

	r = coap2oscore(T3__COAP_REQ, T3__COAP_REQ_LEN, (uint8_t *)&buf_oscore,
			&buf_oscore_len, &c_client);

	zassert_equal(r, OscoreNoError, "Error in coap2oscore!");

	zassert_mem_equal__(&buf_oscore, T3__OSCORE_REQ, T3__OSCORE_REQ_LEN,
			    "coap2oscore failed");
}

/**
 * Test 5 :
 * - Client Key derivation with ID Context see Appendix 3.1
 * - OSCORE request generation see Appendix C6
 */
static void oscore_client_test5(void)
{
	OscoreError r;
	struct context c_client;
	struct oscore_init_params params = {
		.dev_type = CLIENT,
		.master_secret.ptr = T5__MASTER_SECRET,
		.master_secret.len = T5__MASTER_SECRET_LEN,
		.sender_id.ptr = T5__SENDER_ID,
		.sender_id.len = T5__SENDER_ID_LEN,
		.recipient_id.ptr = T5__RECIPIENT_ID,
		.recipient_id.len = T5__RECIPIENT_ID_LEN,
		.master_salt.ptr = T5__MASTER_SALT,
		.master_salt.len = T5__MASTER_SALT_LEN,
		.id_context.ptr = T5__ID_CONTEXT,
		.id_context.len = T5__ID_CONTEXT_LEN,
		.aead_alg = AES_CCM_16_64_128,
		.hkdf = SHA_256,
	};

	r = oscore_context_init(&params, &c_client);

	zassert_equal(r, OscoreNoError, "Error in oscore_context_init");

	/* 
    required only for the test vector.
    during normal operation the sender sequence number is
    increased automatically after every sending
    */
	c_client.sc.sender_seq_num = 20;

	uint8_t buf_oscore[256];
	uint16_t buf_oscore_len = sizeof(buf_oscore);

	r = coap2oscore(T5__COAP_REQ, T5__COAP_REQ_LEN, (uint8_t *)&buf_oscore,
			&buf_oscore_len, &c_client);

	zassert_equal(r, OscoreNoError, "Error in coap2oscore!");

	zassert_mem_equal__(&buf_oscore, T5__OSCORE_REQ, buf_oscore_len,
			    "coap2oscore failed");
}

/**
 * Test 2:
 * - Server Key derivation with master salt see RFC8613 Appendix C.1.2
 * - Generating OSCORE response with key form C.1.2 see RFC8613 Appendix C.7
 */
static void oscore_server_test2(void)
{
	OscoreError r;
	struct context c_server;
	struct oscore_init_params params_server = {
		.dev_type = SERVER,
		.master_secret.ptr = T2__MASTER_SECRET,
		.master_secret.len = T2__MASTER_SECRET_LEN,
		.sender_id.ptr = T2__SENDER_ID,
		.sender_id.len = T2__SENDER_ID_LEN,
		.recipient_id.ptr = T2__RECIPIENT_ID,
		.recipient_id.len = T2__RECIPIENT_ID_LEN,
		.master_salt.ptr = T2__MASTER_SALT,
		.master_salt.len = T2__MASTER_SALT_LEN,
		.id_context.ptr = T2__ID_CONTEXT,
		.id_context.len = T2__ID_CONTEXT_LEN,
		.aead_alg = AES_CCM_16_64_128,
		.hkdf = SHA_256,
	};

	r = oscore_context_init(&params_server, &c_server);

	zassert_equal(r, OscoreNoError, "Error in oscore_context_init");

	/*Test decrypting of an incoming request*/
	uint8_t buf_coap[256];
	uint16_t buf_coap_len = sizeof(buf_coap);
	bool oscore_present_flag = false;

	r = oscore2coap(T2__OSCORE_REQ, T2__OSCORE_REQ_LEN, buf_coap,
			&buf_coap_len, &oscore_present_flag, &c_server);

	zassert_equal(r, OscoreNoError, "Error in oscore2coap!");
	zassert_true(oscore_present_flag, "The packet is not OSCORE packet");
	zassert_mem_equal__(&buf_coap, T2__COAP_REQ, buf_coap_len,
			    "oscore2coap failed");

	/*Test generating an encrypted response, see Appendix C7*/
	uint8_t buf_oscore[256];
	uint16_t buf_oscore_len = sizeof(buf_oscore);

	r = coap2oscore(T2__COAP_RESPONSE, T2__COAP_RESPONSE_LEN,
			(uint8_t *)&buf_oscore, &buf_oscore_len, &c_server);

	zassert_equal(r, OscoreNoError, "Error in coap2oscore");

	zassert_mem_equal__(&buf_oscore, T2__OSCORE_RESP, buf_oscore_len,
			    "coap2oscore failed");
}

static void oscore_server_test4(void)
{
	OscoreError r;
	struct context c_server;
	struct oscore_init_params params_server = {
		.dev_type = SERVER,
		.master_secret.ptr = T4__MASTER_SECRET,
		.master_secret.len = T4__MASTER_SECRET_LEN,
		.sender_id.ptr = T4__SENDER_ID,
		.sender_id.len = T4__SENDER_ID_LEN,
		.recipient_id.ptr = T4__RECIPIENT_ID,
		.recipient_id.len = T4__RECIPIENT_ID_LEN,
		.master_salt.ptr = T4__MASTER_SALT,
		.master_salt.len = T4__MASTER_SALT_LEN,
		.id_context.ptr = T4__ID_CONTEXT,
		.id_context.len = T4__ID_CONTEXT_LEN,
		.aead_alg = AES_CCM_16_64_128,
		.hkdf = SHA_256,
	};

	r = oscore_context_init(&params_server, &c_server);

	zassert_equal(r, OscoreNoError, "Error in oscore_context_init");

	zassert_mem_equal__(c_server.sc.sender_key.ptr, T4__SENDER_KEY,
			    c_server.sc.sender_key.len,
			    "T4 sender key derivation failed");

	zassert_mem_equal__(c_server.rc.recipient_key.ptr, T4__RECIPIENT_KEY,
			    c_server.rc.recipient_key.len,
			    "T4 recipient key derivation failed");

	zassert_mem_equal__(c_server.cc.common_iv.ptr, T4__COMMON_IV,
			    c_server.cc.common_iv.len,
			    "T4 common IV derivation failed");
}

/**
 * Test 6:
 * - Server Key derivation with ID context see RFC8613 Appendix C.3.2
 */
static void oscore_server_test6(void)
{
	OscoreError r;
	struct context c_server;
	struct oscore_init_params params_server = {
		.dev_type = SERVER,
		.master_secret.ptr = T6__MASTER_SECRET,
		.master_secret.len = T6__MASTER_SECRET_LEN,
		.sender_id.ptr = T6__SENDER_ID,
		.sender_id.len = T6__SENDER_ID_LEN,
		.recipient_id.ptr = T6__RECIPIENT_ID,
		.recipient_id.len = T6__RECIPIENT_ID_LEN,
		.master_salt.ptr = T6__MASTER_SALT,
		.master_salt.len = T6__MASTER_SALT_LEN,
		.id_context.ptr = T6__ID_CONTEXT,
		.id_context.len = T6__ID_CONTEXT_LEN,
		.aead_alg = AES_CCM_16_64_128,
		.hkdf = SHA_256,
	};

	r = oscore_context_init(&params_server, &c_server);

	zassert_equal(r, OscoreNoError, "Error in oscore_context_init");

	zassert_mem_equal__(c_server.sc.sender_key.ptr, T6__SENDER_KEY,
			    c_server.sc.sender_key.len,
			    "T6 sender key derivation failed");

	zassert_mem_equal__(c_server.rc.recipient_key.ptr, T6__RECIPIENT_KEY,
			    c_server.rc.recipient_key.len,
			    "T6 recipient key derivation failed");

	zassert_mem_equal__(c_server.cc.common_iv.ptr, T6__COMMON_IV,
			    c_server.cc.common_iv.len,
			    "T6 common IV derivation failed");
}

#endif

void test_main(void)
{
#ifdef EDHOC_TESTS

	ztest_test_suite(initiator_tests, ztest_unit_test(test_initiator1),
			 ztest_unit_test(test_initiator2),
			 ztest_unit_test(test_initiator3),
			 ztest_unit_test(test_initiator4));

	ztest_test_suite(responder_tests, ztest_unit_test(test_responder1),
			 ztest_unit_test(test_responder2),
			 ztest_unit_test(test_responder3),
			 ztest_unit_test(test_responder4));

	ztest_run_test_suite(initiator_tests);
	ztest_run_test_suite(responder_tests);

#endif

#ifdef OSCORE_TESTS
	ztest_test_suite(oscore_tests, ztest_unit_test(oscore_client_test1),
			 ztest_unit_test(oscore_server_test2),
			 ztest_unit_test(oscore_client_test3),
			 ztest_unit_test(oscore_server_test4),
			 ztest_unit_test(oscore_client_test5),
			 ztest_unit_test(oscore_server_test6));

	ztest_run_test_suite(oscore_tests);
#endif
}
