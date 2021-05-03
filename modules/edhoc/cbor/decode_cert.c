/*
 * Generated with cddl_gen.py (https://github.com/oyvindronningstad/cddl_gen)
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

	bool tmp_result = (((((intx32_decode(state, (&(*result)._cert_cert_type))))
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



__attribute__((unused)) static bool type_test_decode_cert(
		struct cert *result)
{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return decode_cert(NULL, result);
}


bool cbor_decode_cert(
		const uint8_t *payload, size_t payload_len,
		struct cert *result,
		size_t *payload_len_out)
{
	return entry_function(payload, payload_len, (const void *)result,
		payload_len_out, (void *)decode_cert,
		9, 0);
}
