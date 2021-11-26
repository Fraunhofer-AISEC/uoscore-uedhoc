/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

extern "C" {
#include "../../../../modules/edhoc/edhoc.h"
#include "../../../common/sock.h"
#include "../../../common/test_vec_parser.h"
}
#include "../../../../externals/cantcoap/cantcoap.h"

#define USE_IPV4

/*comment this out to use DH keys from the test vectors*/
//#define USE_RANDOM_EPHEMERAL_DH_KEY

/**
 * @brief	Initializes sockets for CoAP client.
 * @param
 * @retval	error code
 */
static int start_coap_client(void)
{
	int err;
#ifdef USE_IPV4
	struct sockaddr_in servaddr;
	const char IPV4_SERVADDR[] = { "127.0.0.1" };
	err = sock_init(SOCK_CLIENT, IPV4_SERVADDR, IPv4, &servaddr,
			sizeof(servaddr));
	if (err < 0) {
		printf("error during socket initialization (error code: %d)",
		       err);
		return -1;
	}
#endif
#ifdef USE_IPV6
	struct sockaddr_in6 servaddr;
	const char IPV6_SERVADDR[] = { "::1" };
	err = sock_init(SOCK_CLIENT, IPV6_SERVADDR, IPv6, &servaddr,
			sizeof(servaddr));
	if (err < 0) {
		printf("error during socket initialization (error code: %d)",
		       err);
		return -1;
	}
#endif
	return 0;
}


/**
 * @brief	Callback function called inside the frontend when data needs to 
 * 		be send over the network. We use here CoAP as transport 
 * @param	data pointer to the data that needs to be send
 * @param	data_len lenhgt of the data in bytes
 */
enum err tx(uint8_t *data, uint32_t data_len)
{
	/*construct a CoAP packet*/
	static uint16_t mid = 0;
	static uint32_t token = 0;
	CoapPDU *pdu = new CoapPDU();
	pdu->reset();
	pdu->setVersion(1);
	pdu->setType(CoapPDU::COAP_CONFIRMABLE);
	pdu->setCode(CoapPDU::COAP_POST);
	pdu->setToken((uint8_t *)&(++token), sizeof(token));
	pdu->setMessageID(mid++);
	pdu->setURI((char *)".well-known/edhoc", 17);
	pdu->setPayload(data, data_len);

	send(sockfd, pdu->getPDUPointer(), pdu->getPDULength(), 0);

	delete pdu;
	return ok;
}

/**
 * @brief	Callback function called inside the frontend when data needs to 
 * 		be received over the network. We use here CoAP as transport 
 * @param	data pointer to the data that needs to be received
 * @param	data_len lenhgt of the data in bytes
 */
enum err rx(uint8_t *data, uint32_t *data_len)
{
	int n;
	char buffer[MAXLINE];
	CoapPDU *recvPDU;
	/* receive */
	n = recv(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL);
	if (n < 0) {
		printf("recv error");
	}

	recvPDU = new CoapPDU((uint8_t *)buffer, n);

	if (recvPDU->validate()) {
		recvPDU->printHuman();
	}

	uint32_t payload_len = recvPDU->getPayloadLength();
	printf("data_len: %d\n", *data_len);
	printf("payload_len: %d\n", payload_len);

	if (*data_len >= payload_len) {
		memcpy(data, recvPDU->getPayloadPointer(), payload_len);
		*data_len = payload_len;
	} else {
		printf("insufficient space in buffer");
		return dest_buffer_to_small;
	}

	delete recvPDU;
	return ok;
}

int main()
{
	uint8_t oscore_master_secret[16];
	uint8_t oscore_master_salt[8];

	/* edhoc declarations */
	uint8_t PRK_4x3m[PRK_DEFAULT_SIZE];
	uint8_t th4[SHA_DEFAULT_SIZE];
	uint8_t err_msg[ERR_MSG_DEFAULT_SIZE];
	uint32_t err_msg_len = sizeof(err_msg);
	uint8_t ad_2[AD_DEFAULT_SIZE];
	uint64_t ad_2_len = sizeof(ad_2);
	uint8_t ad_4[AD_DEFAULT_SIZE];
	uint64_t ad_4_len = sizeof(ad_2);

	/* test vector inputs */
	const uint8_t TEST_VEC_NUM = 14;
	uint16_t cred_num = 1;
	struct other_party_cred cred_r;
	struct edhoc_initiator_context c_i;
	struct other_party_cred_bufs other_party_bufs;
	struct edhoc_initiator_context_bufs initiator_context_bufs;
	char filename[] = { "../../common/edhoc-vectors-json_v11.txt" };
	char test_vec_buf[1024 * 160];
	uint32_t test_vec_buf_len = sizeof(test_vec_buf);

	TRY(read_test_vectors(filename, test_vec_buf, &test_vec_buf_len));

	TRY(get_OTHER_PARTY_CRED_from_test_vec(RESPONDER, &other_party_bufs,
					       &cred_r, TEST_VEC_NUM,
					       test_vec_buf, test_vec_buf_len));

	TRY(get_EDHOC_INITIATOR_CONTEXT_from_test_vec(
		&initiator_context_bufs, &c_i, TEST_VEC_NUM, test_vec_buf,
		test_vec_buf_len));

#ifdef USE_RANDOM_EPHEMERAL_DH_KEY
	uint32_t seed;
	uint8_t G_X_random[32];
	uint8_t X_random[32];

	/*create a random seed*/
	FILE *fp;
	fp = fopen("/dev/urandom", "r");
	uint64_t seed_len = fread((uint8_t *)&seed, 1, sizeof(seed), fp);
	fclose(fp);
	PRINT_ARRAY("seed", (uint8_t *)&seed, seed_len);

	/*create ephemeral DH keys from seed*/
	TRY(ephemeral_dh_key_gen(X25519, seed, X_random, G_X_random));
	c_i.g_x.ptr = G_X_random;
	c_i.g_x.len = sizeof(G_X_random);
	c_i.x.ptr = X_random;
	c_i.x.len = sizeof(X_random);
	PRINT_ARRAY("secret ephemeral DH key", c_i.g_x.ptr, c_i.g_x.len);
	PRINT_ARRAY("public ephemeral DH key", c_i.x.ptr, c_i.x.len);

#endif

	TRY(start_coap_client());

	TRY(edhoc_initiator_run(&c_i, &cred_r, cred_num, err_msg, &err_msg_len,
				ad_2, &ad_2_len, ad_4, &ad_4_len, PRK_4x3m,
				sizeof(PRK_4x3m), th4, sizeof(th4), tx, rx));

	PRINT_ARRAY("PRK_4x3m", PRK_4x3m, sizeof(PRK_4x3m));
	PRINT_ARRAY("th4", th4, sizeof(th4));

	TRY(edhoc_exporter(SHA_256, PRK_4x3m, sizeof(PRK_4x3m), th4,
			   sizeof(th4), "OSCORE_Master_Secret",
			   oscore_master_secret, 16));
	PRINT_ARRAY("OSCORE Master Secret", oscore_master_secret, 16);

	TRY(edhoc_exporter(SHA_256, PRK_4x3m, sizeof(PRK_4x3m), th4,
			   sizeof(th4), "OSCORE_Master_Salt",
			   oscore_master_salt, 8));
	PRINT_ARRAY("OSCORE Master Salt", oscore_master_salt, 8);

	close(sockfd);
	return 0;
}
