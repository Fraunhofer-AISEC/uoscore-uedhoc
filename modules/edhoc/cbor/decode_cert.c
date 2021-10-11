/*
 * Generated using cddl_gen version 0.2.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"
#include "decode_cert.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool decode_cert(
		cbor_state_t *state, struct cert *result)
{
	cbor_print("%s\n", __func__);

	bool tmp_result = (((((intx32_decode(state, (&(*result)._cert_type))))
	&& ((bstrx_decode(state, (&(*result)._cert_serial_number))))
	&& ((tstrx_decode(state, (&(*result)._cert_issuer))))
	&& ((intx32_decode(state, (&(*result)._cert_validity_not_before))))
	&& ((intx32_decode(state, (&(*result)._cert_validity_not_after))))
	&& ((bstrx_decode(state, (&(*result)._cert_subject))))
	&& ((bstrx_decode(state, (&(*result)._cert_pk))))
	&& ((intx32_decode(state, (&(*result)._cert_extensions))))
	&& ((bstrx_decode(state, (&(*result)._cert_signature)))))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



bool cbor_decode_cert(
		const uint8_t *payload, uint32_t payload_len,
		struct cert *result,
		uint32_t *payload_len_out)
{
	cbor_state_t states[2];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 9);

	bool ret = decode_cert(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
