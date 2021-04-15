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
#include <errno.h>
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
#include "main.h"

#define USE_IPV4

CoapPDU *edhocPDU = new CoapPDU();

char buffer[MAXLINE];
CoapPDU *recvPDU;

/*comment this out to use DH keys from the test vectors*/
//#define USE_RANDOM_EPHEMERAL_DH_KEY

#ifdef USE_IPV6
struct sockaddr_in6 client_addr;
#endif
#ifdef USE_IPV4
struct sockaddr_in client_addr;
#endif
socklen_t client_addr_len;

/**
 * @brief	Initializes socket for CoAP server.
 * @param	
 * @retval	error code
 */
static int start_coap_server(void)
{
	int err;
#ifdef USE_IPV4
	struct sockaddr_in servaddr;
	//struct sockaddr_in client_addr;
	client_addr_len = sizeof(client_addr);
	memset(&client_addr, 0, sizeof(client_addr));
	const char IPV4_SERVADDR[] = { "127.0.0.1" };
	err = sock_init(SOCK_SERVER, IPV4_SERVADDR, IPv4, &servaddr,
			sizeof(servaddr));
	if (err < 0) {
		printf("error during socket initialization (error code: %d)",
		       err);
		return -1;
	}
#endif
#ifdef USE_IPV6
	struct sockaddr_in6 servaddr;
	//struct sockaddr_in6 client_addr;
	client_addr_len = sizeof(client_addr);
	memset(&client_addr, 0, sizeof(client_addr));
	const char IPV6_SERVADDR[] = { "::1" };
	err = sock_init(SOCK_SERVER, IPV6_SERVADDR, IPv6, &servaddr,
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
 * @brief	Sends CoAP packet over network.
 * @param	pdu pointer to CoAP packet
 * @retval	error code
 */
static int send_coap_reply(CoapPDU *pdu)
{
	int r;

	PRINT_ARRAY("CoAP message", pdu->getPDUPointer(), pdu->getPDULength());
	r = sendto(sockfd, pdu->getPDUPointer(), pdu->getPDULength(), 0,
		   (struct sockaddr *)&client_addr, client_addr_len);
	if (r < 0) {
		printf("Error: failed to send reply (Code: %d, ErrNo: %d)\n", r,
		       errno);
		return r;
	}

	printf("CoAP reply sent!\n");
	return 0;
}

/**
 * @brief	Adds payload to CoAP response packet and sends it over network.
 * @param	msg pointer to message payload
 * @param	msg_len length of message payload
 * @param
 * @retval	
 */
void send_coap(uint8_t *msg, uint32_t msg_len)
{
	edhocPDU->setCode(CoapPDU::COAP_CHANGED);
	edhocPDU->setPayload(msg, msg_len);
	send_coap_reply(edhocPDU);
}

/**
 * @brief	Receives CoAP request packet and supplies the payload to the callee.
 * @param	msg buffer to store message payload
 * @param	msg_len integer to store length of the received message payload	
 * @param
 * @retval	
 */
void recv_coap(uint8_t **msg, uint32_t *msg_len)
{
	int n;

	/* receive */
	client_addr_len = sizeof(client_addr);
	memset(&client_addr, 0, sizeof(client_addr));

	n = recvfrom(sockfd, (char *)buffer, sizeof(buffer), 0,
		     (struct sockaddr *)&client_addr, &client_addr_len);
	if (n < 0)
		printf("recv error");

	recvPDU = new CoapPDU((uint8_t *)buffer, n);

	if (recvPDU->validate()) {
		recvPDU->printHuman();
	}

	edhocPDU->reset();
	edhocPDU->setVersion(recvPDU->getVersion());
	edhocPDU->setMessageID(recvPDU->getMessageID());
	edhocPDU->setToken(recvPDU->getTokenPointer(),
			   recvPDU->getTokenLength());

	if (recvPDU->getType() == CoapPDU::COAP_CONFIRMABLE) {
		edhocPDU->setType(CoapPDU::COAP_ACKNOWLEDGEMENT);
	} else {
		edhocPDU->setType(CoapPDU::COAP_NON_CONFIRMABLE);
	}

	*msg = recvPDU->getPayloadPointer();
	*msg_len = recvPDU->getPayloadLength();
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
	uint8_t ad_1[AD_DEFAULT_SIZE];
	uint64_t ad_1_len = sizeof(ad_1);
	uint8_t ad_3[AD_DEFAULT_SIZE];
	uint64_t ad_3_len = sizeof(ad_1);
	EdhocError r;

#ifdef USE_RANDOM_EPHEMERAL_DH_KEY
	uint32_t seed;
	uint8_t G_Y_random[32];
	uint8_t Y_random[32];

	/*create a random seed*/
	FILE *fp;
	fp = fopen("/dev/urandom", "r");
	uint64_t seed_len = fread((uint8_t *)&seed, 1, sizeof(seed), fp);
	fclose(fp);
	PRINT_ARRAY("seed", (uint8_t *)&seed, seed_len);

	/*create ephemeral DH keys from seed*/
	r = ephemeral_dh_key_gen(X25519, seed, Y_random, G_Y_random);
	if (r != EdhocNoError) {
		printf("Error in ephemeral_dh_key_gen, (Error code %d)\n", r);
	}
	PRINT_ARRAY("secret ephemeral DH key", Y_random, sizeof(Y_random));
	PRINT_ARRAY("public ephemeral DH key", G_Y_random, sizeof(G_Y_random));
#endif

	struct other_party_cred cred_i = { { ID_CRED_I_LEN, ID_CRED_I },
					   { CRED_I_LEN, CRED_I },
					   { PK_I_LEN, PK_I },
					   { G_I_LEN, G_I },
					   { CA_LEN, CA },
					   { CA_PK_LEN, CA_PK } };
	uint16_t cred_num = 1;
	struct edhoc_responder_context c_r = { { SUITES_R_LEN, SUITES_R },
#ifdef USE_RANDOM_EPHEMERAL_DH_KEY
					       { sizeof(G_Y_random),
						 G_Y_random },
					       { sizeof(Y_random), Y_random },
#else
					       { G_Y_LEN, G_Y },
					       { Y_LEN, Y },
#endif
					       { C_R_LEN, C_R },
					       { G_R_LEN, G_R },
					       { R_LEN, R },
					       { AD_2_LEN, AD_2 },
					       { ID_CRED_R_LEN, ID_CRED_R },
					       { CRED_R_LEN, CRED_R },
					       { SK_R_LEN, SK_R },
					       { PK_R_LEN, PK_R } };

	start_coap_server();

	while (1) {
		r = edhoc_responder_run(&c_r, &cred_i, cred_num, err_msg,
					&err_msg_len, (uint8_t *)&ad_1,
					&ad_1_len, (uint8_t *)&ad_3, &ad_3_len,
					PRK_4x3m, sizeof(PRK_4x3m), th4,
					sizeof(th4));
		if (r != EdhocNoError) {
			printf("error responder run (Error Code %d)\n", r);
		}

		PRINT_ARRAY("PRK_4x3m", PRK_4x3m, sizeof(PRK_4x3m));
		PRINT_ARRAY("th4", th4, sizeof(th4));

		edhoc_exporter(SHA_256, AES_CCM_16_64_128, PRK_4x3m,
			       sizeof(PRK_4x3m), th4, sizeof(th4),
			       "OSCORE Master Secret", oscore_master_secret,
			       16);
		PRINT_ARRAY("OSCORE Master Secret", oscore_master_secret, 16);

		edhoc_exporter(SHA_256, AES_CCM_16_64_128, PRK_4x3m,
			       sizeof(PRK_4x3m), th4, sizeof(th4),
			       "OSCORE Master Salt", oscore_master_salt, 8);
		PRINT_ARRAY("OSCORE Master Salt", oscore_master_salt, 8);
	}

	close(sockfd);
	return 0;
}
