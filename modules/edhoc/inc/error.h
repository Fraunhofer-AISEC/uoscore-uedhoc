/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

#ifndef ERROR_H
#define ERROR_H

/**
 * Error type used throughout the whole edhoc implementation.
 *
 * Every function that might error returns an EdhocError and writes its 
 * return value into an out-parameter.
 */
typedef enum EdhocError {
	EdhocNoError = 0,
	ErrorMessageReceived = 1,
	ErrorMessageSent = 2,
	ErrorDuringHKDFCalculation = 3,
	UnsupportedCipherSuite = 4,
	ErrorDuringAEAD = 5,
	CredentialNotFound = 6,
	ResponderAuthenticationFailed = 7,
	ErrorDuringA3aeEncoding = 8,
	AEADAuthenticationFailed = 9,
	UnknownIdCredMapLabel = 10,
	CertificateAuthenticationFailed = 11,
	NoSuchCA = 12,
	C_R_lengthtosmall = 13,
	MessageBuffToSmall = 14,
	DestBufferToSmall = 15,
	CborEncodingError = 16,
	UnsupportedCborType = 17,
	CborEncodingBufferToSmall = 18,
	CborByteStringBufferToSmall = 19,
	ErrorDuringCborDecoding = 20,
	UnsupportedEcdhCurve = 21,
	UnsupportedAEADAlgorithm = 22,
} EdhocError;

#endif
