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
}
#include "../../../../externals/cantcoap/cantcoap.h"
#include "credentials.h"
#include "credentials_select.h"

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
 * @brief	Creates CoAP packet and sends supplied payload over network.
 * @param	msg pointer to message payload
 * @param	msg_len length of message payload
 * @param
 * @retval	
 */
void send_coap(uint8_t *msg, uint32_t msg_len)
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
	pdu->setPayload(msg, msg_len);

	send(sockfd, pdu->getPDUPointer(), pdu->getPDULength(), 0);

	delete pdu;
}

/**
 * @brief	Waits for CoAP response packet and provides payload to callee.
 * @param	msg buffer to store received message payload
 * @param	msg_len integer to store length of received message payload
 * @param
 * @retval	
 */
void recv_coap(uint8_t **msg, uint32_t *msg_len)
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

	*msg = recvPDU->getPayloadPointer();
	*msg_len = recvPDU->getPayloadLength();

	delete recvPDU;
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
	EdhocError r;
	int err;

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
	r = ephemeral_dh_key_gen(X25519, seed, X_random, G_X_random);
	if (r != EdhocNoError) {
		printf("Error in ephemeral_dh_key_gen, (Error code %d)\n", r);
	}
	PRINT_ARRAY("secret ephemeral DH key", X_random, sizeof(X_random));
	PRINT_ARRAY("public ephemeral DH key", G_X_random, sizeof(G_X_random));
#endif

	struct other_party_cred cred_r = { { ID_CRED_R_LEN, ID_CRED_R },
					   { CRED_R_LEN, CRED_R },
					   { PK_R_LEN, PK_R },
					   { G_R_LEN, G_R },
					   { CA_LEN, CA },
					   { CA_PK_LEN, CA_PK } };
	uint16_t cred_num = 1;
	struct edhoc_initiator_context c_i = { METHOD_TYPE,
					       CORR,
					       { SUITES_I_LEN, SUITES_I },
					       { C_I_LEN, C_I },
					       { AD_1_LEN, AD_1 },
					       { AD_3_LEN, AD_3 },
					       { ID_CRED_I_LEN, ID_CRED_I },
					       { CRED_I_LEN, CRED_I },
#ifdef USE_RANDOM_EPHEMERAL_DH_KEY
					       { sizeof(G_X_random),
						 G_X_random },
					       { sizeof(X_random), X_random },
#else
					       { G_X_LEN, G_X },
					       { X_LEN, X },
#endif
					       { G_I_LEN, G_I },
					       { I_LEN, I },
					       { SK_I_LEN, SK_I },
					       { PK_I_LEN, PK_I } };

	err = start_coap_client();
	if (err != 0) {
		printf("Error in start_coap_client, (Error code %d)\n", err);
	}

	r = edhoc_initiator_run(&c_i, &cred_r, cred_num, err_msg, &err_msg_len,
				ad_2, &ad_2_len, PRK_4x3m, sizeof(PRK_4x3m),
				th4, sizeof(th4));
	if (r != EdhocNoError) {
		printf("Error in edhoc_initiator_run, (Error code %d)\n", r);
	}

	PRINT_ARRAY("PRK_4x3m", PRK_4x3m, sizeof(PRK_4x3m));
	PRINT_ARRAY("th4", th4, sizeof(th4));

	edhoc_exporter(SHA_256, AES_CCM_16_64_128, PRK_4x3m, sizeof(PRK_4x3m),
		       th4, sizeof(th4), "OSCORE Master Secret",
		       oscore_master_secret, 16);
	PRINT_ARRAY("OSCORE Master Secret", oscore_master_secret, 16);

	edhoc_exporter(SHA_256, AES_CCM_16_64_128, PRK_4x3m, sizeof(PRK_4x3m),
		       th4, sizeof(th4), "OSCORE Master Salt",
		       oscore_master_salt, 8);
	PRINT_ARRAY("OSCORE Master Salt", oscore_master_salt, 8);

	close(sockfd);
	return 0;
}
