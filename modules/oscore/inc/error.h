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
 * Error type used throughout the whole oscore implementation.
 *
 * Every function that might error returns an OscoreError and writes its 
 * return value into an out-parameter.
 */
typedef enum OscoreError {
	OscoreNoError = 0,
	OscoreTinyCryptError = 1,
	OscoreUnknownHkdf = 2,
	OscoreOutTooLong = 3,
	OscoreInvalidAlgorithmAEAD = 4,
	OscoreInvalidAlgorithmHKDF = 5,
	OscoreKidRecipentIdMismatch = 6,
	OscoreAuthenticationError = 7,
	OscoreValueLenToLongError = 8,
	OscoreInPktInvalidTKL = 9,
	OscoreInPktInvalidOptionDelta = 10,
	OscoreInPktInvalidOptionLen = 11,
	OscoreInPktInvalidPiv = 12,
	OscoreInfoToLong = 13,
	DestBufferToSmall = 14,
	DeltaExtraByteError = 15,
	LenExtraByteError = 16,
	NotValidInputPacket = 17,
} OscoreError;

#endif
