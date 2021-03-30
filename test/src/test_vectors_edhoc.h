/*
   Copyright (c) 2021 Fraunhofer AISEC. See the COPYRIGHT
   file at the top-level directory of this distribution.

   Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
   http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
   <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
   option. This file may not be copied, modified, or distributed
   except according to those terms.
*/

#ifndef TEST_VECTORS_EDHOC
#define TEST_VECTORS_EDHOC

#include <stdint.h>

/*
 * Test 1
 * |mode                          | RPK/Cert | suite | Ref [1]
 * |INITIATOR_SK_RESPONDER_SK     | RPK x5t  | 0     | 1-290
 *
 * [1]:https://github.com/lake-wg/edhoc/blob/master/test-vectors/vectors.txt
 */

/******************************************************************************/
/*                              Outputs                                       */
/******************************************************************************/

extern uint8_t T1_PRK_4X3M[];
extern uint32_t T1_PRK_4X3M_LEN;

extern uint8_t T1_TH4[];
extern uint32_t T1_TH4_LEN;

extern uint8_t T1_OSCORE_MSECRET[];
extern uint32_t T1_OSCORE_MSECRET_LEN;

extern uint8_t T1_OSCORE_MSALT[];
extern uint32_t T1_OSCORE_MSALT_LEN;

/******************************************************************************/
/*                              Messages                                      */
/******************************************************************************/

extern uint8_t T1_MSG_1[];
extern uint32_t T1_MSG_1_LEN;

extern uint8_t T1_MSG_2[];
extern uint32_t T1_MSG_2_LEN;

extern uint8_t T1_MSG_3[];
extern uint32_t T1_MSG_3_LEN;

/******************************************************************************/
/*                              Initiator specific                            */
/******************************************************************************/

extern enum method_type T1I__METHOD_TYPE;
extern uint8_t T1I__CORR;

extern uint8_t T1I__SUITES_I[];
extern uint32_t T1I__SUITES_I_LEN;

extern uint8_t T1I__G_X[];
extern uint32_t T1I__G_X_LEN;

extern uint8_t T1I__X[];
extern uint32_t T1I__X_LEN;

extern uint8_t T1I__C_I[];
extern uint32_t T1I__C_I_LEN;

extern uint8_t T1I__AD_1[];
extern uint32_t T1I__AD_1_LEN;

extern uint8_t T1I__AD_3[];
extern uint32_t T1I__AD_3_LEN;

extern uint8_t T1I__I[];
extern uint32_t T1I__I_LEN;

extern uint8_t T1I__G_I[];
extern uint32_t T1I__G_I_LEN;

extern uint8_t T1I__ID_CRED_I[];
extern uint32_t T1I__ID_CRED_I_LEN;

extern uint8_t T1I__CRED_I[];
extern uint32_t T1I__CRED_I_LEN;

extern uint8_t T1I__SK_I[];
extern uint32_t T1I__SK_I_LEN;

extern uint8_t T1I__PK_I[];
extern uint32_t T1I__PK_I_LEN;

/*other party credentials*/
extern uint8_t T1I__ID_CRED_R[];
extern uint32_t T1I__ID_CRED_R_LEN;

extern uint8_t T1I__CRED_R[];
extern uint32_t T1I__CRED_R_LEN;

extern uint8_t T1I__PK_R[];
extern uint32_t T1I__PK_R_LEN;

extern uint8_t T1I__G_R[];
extern uint32_t T1I__G_R_LEN;

extern uint8_t T1I__CA[];
extern uint32_t T1I__CA_LEN;

extern uint8_t T1I__CA_PK[];
extern uint32_t T1I__CA_PK_LEN;

/******************************************************************************/
/*                              Responder specific                            */
/******************************************************************************/

extern uint8_t T1R__SUITES_R[];
extern uint32_t T1R__SUITES_R_LEN;

extern uint8_t T1R__G_Y[];
extern uint32_t T1R__G_Y_LEN;

extern uint8_t T1R__Y[];
extern uint32_t T1R__Y_LEN;

extern uint8_t T1R__C_R[];
extern uint32_t T1R__C_R_LEN;

extern uint8_t T1R__AD_2[];
extern uint32_t T1R__AD_2_LEN;

extern uint8_t T1R__ID_CRED_R[];
extern uint32_t T1R__ID_CRED_R_LEN;

extern uint8_t T1R__CRED_R[];
extern uint32_t T1R__CRED_R_LEN;

extern uint8_t T1R__SK_R[];
extern uint32_t T1R__SK_R_LEN;

extern uint8_t T1R__PK_R[];
extern uint32_t T1R__PK_R_LEN;

extern uint8_t T1R__R[];
extern uint32_t T1R__R_LEN;

extern uint8_t T1R__G_R[];
extern uint32_t T1R__G_R_LEN;

/*other party credentials*/
extern uint8_t T1R__ID_CRED_I[];
extern uint32_t T1R__ID_CRED_I_LEN;

extern uint8_t T1R__CRED_I[];
extern uint32_t T1R__CRED_I_LEN;

extern uint8_t T1R__PK_I[];
extern uint32_t T1R__PK_I_LEN;

extern uint8_t T1R__G_I[];
extern uint32_t T1R__G_I_LEN;

extern uint8_t T1R__CA[];
extern uint32_t T1R__CA_LEN;

extern uint8_t T1R__CA_PK[];
extern uint32_t T1R__CA_PK_LEN;

/*
 * Test 2
 * |mode                          | RPK/Cert | suite    | Ref [1]
 * |INITIATOR_SDHK_RESPONDER_SDHK | RPK kid  | 0        | 292-540
 *
 * [1]:https://github.com/lake-wg/edhoc/blob/master/test-vectors/vectors.txt
 */
/******************************************************************************/
/*                              Outputs                                       */
/******************************************************************************/

extern uint8_t T2_PRK_4X3M[];
extern uint32_t T2_PRK_4X3M_LEN;

extern uint8_t T2_TH4[];
extern uint32_t T2_TH4_LEN;

extern uint8_t T2_OSCORE_MSECRET[];
extern uint32_t T2_OSCORE_MSECRET_LEN;

extern uint8_t T2_OSCORE_MSALT[];
extern uint32_t T2_OSCORE_MSALT_LEN;

/******************************************************************************/
/*                              Messages                                      */
/******************************************************************************/

extern uint8_t T2_MSG_1[];
extern uint32_t T2_MSG_1_LEN;

extern uint8_t T2_MSG_2[];
extern uint32_t T2_MSG_2_LEN;

extern uint8_t T2_MSG_3[];
extern uint32_t T2_MSG_3_LEN;

/******************************************************************************/
/*                              Initiator specific                            */
/******************************************************************************/

extern enum method_type T2I__METHOD_TYPE;
extern uint8_t T2I__CORR;

extern uint8_t T2I__SUITES_I[];
extern uint32_t T2I__SUITES_I_LEN;

extern uint8_t T2I__G_X[];
extern uint32_t T2I__G_X_LEN;

extern uint8_t T2I__X[];
extern uint32_t T2I__X_LEN;

extern uint8_t T2I__C_I[];
extern uint32_t T2I__C_I_LEN;

extern uint8_t T2I__AD_1[];
extern uint32_t T2I__AD_1_LEN;

extern uint8_t T2I__AD_3[];
extern uint32_t T2I__AD_3_LEN;

extern uint8_t T2I__I[];
extern uint32_t T2I__I_LEN;

extern uint8_t T2I__G_I[];
extern uint32_t T2I__G_I_LEN;

extern uint8_t T2I__ID_CRED_I[];
extern uint32_t T2I__ID_CRED_I_LEN;

extern uint8_t T2I__CRED_I[];
extern uint32_t T2I__CRED_I_LEN;

extern uint8_t T2I__SK_I[];
extern uint32_t T2I__SK_I_LEN;

extern uint8_t T2I__PK_I[];
extern uint32_t T2I__PK_I_LEN;

/*other party credentials*/
extern uint8_t T2I__ID_CRED_R[];
extern uint32_t T2I__ID_CRED_R_LEN;

extern uint8_t T2I__CRED_R[];
extern uint32_t T2I__CRED_R_LEN;

extern uint8_t T2I__PK_R[];
extern uint32_t T2I__PK_R_LEN;

extern uint8_t T2I__G_R[];
extern uint32_t T2I__G_R_LEN;

extern uint8_t T2I__CA[];
extern uint32_t T2I__CA_LEN;

extern uint8_t T2I__CA_PK[];
extern uint32_t T2I__CA_PK_LEN;

/******************************************************************************/
/*                              Responder specific                            */
/******************************************************************************/

extern uint8_t T2R__SUITES_R[];
extern uint32_t T2R__SUITES_R_LEN;

extern uint8_t T2R__G_Y[];
extern uint32_t T2R__G_Y_LEN;

extern uint8_t T2R__Y[];
extern uint32_t T2R__Y_LEN;

extern uint8_t T2R__C_R[];
extern uint32_t T2R__C_R_LEN;

extern uint8_t T2R__AD_2[];
extern uint32_t T2R__AD_2_LEN;

extern uint8_t T2R__ID_CRED_R[];
extern uint32_t T2R__ID_CRED_R_LEN;

extern uint8_t T2R__CRED_R[];
extern uint32_t T2R__CRED_R_LEN;

extern uint8_t T2R__SK_R[];
extern uint32_t T2R__SK_R_LEN;

extern uint8_t T2R__PK_R[];
extern uint32_t T2R__PK_R_LEN;

extern uint8_t T2R__R[];
extern uint32_t T2R__R_LEN;

extern uint8_t T2R__G_R[];
extern uint32_t T2R__G_R_LEN;

/*other party credentials*/
extern uint8_t T2R__ID_CRED_I[];
extern uint32_t T2R__ID_CRED_I_LEN;

extern uint8_t T2R__CRED_I[];
extern uint32_t T2R__CRED_I_LEN;

extern uint8_t T2R__PK_I[];
extern uint32_t T2R__PK_I_LEN;

extern uint8_t T2R__G_I[];
extern uint32_t T2R__G_I_LEN;

extern uint8_t T2R__CA[];
extern uint32_t T2R__CA_LEN;

extern uint8_t T2R__CA_PK[];
extern uint32_t T2R__CA_PK_LEN;

/*
 * Test 3
 * |mode                          | RPK/Cert | suite    | Ref [1]
 * |INITIATOR_SK_RESPONDER_SK     | x5chain  | 0        | none
 *
 * [1]:https://github.com/lake-wg/edhoc/blob/master/test-vectors/vectors.txt
 */
/******************************************************************************/
/*                              Outputs                                       */
/******************************************************************************/

extern uint8_t T3_PRK_4X3M[];
extern uint32_t T3_PRK_4X3M_LEN;

extern uint8_t T3_TH4[];
extern uint32_t T3_TH4_LEN;

extern uint8_t T3_OSCORE_MSECRET[];
extern uint32_t T3_OSCORE_MSECRET_LEN;

extern uint8_t T3_OSCORE_MSALT[];
extern uint32_t T3_OSCORE_MSALT_LEN;

/******************************************************************************/
/*                              Messages                                      */
/******************************************************************************/

extern uint8_t T3_MSG_1[];
extern uint32_t T3_MSG_1_LEN;

extern uint8_t T3_MSG_2[];
extern uint32_t T3_MSG_2_LEN;

extern uint8_t T3_MSG_3[];
extern uint32_t T3_MSG_3_LEN;

/******************************************************************************/
/*                              Initiator specific                            */
/******************************************************************************/

extern enum method_type T3I__METHOD_TYPE;
extern uint8_t T3I__CORR;

extern uint8_t T3I__SUITES_I[];
extern uint32_t T3I__SUITES_I_LEN;

extern uint8_t T3I__G_X[];
extern uint32_t T3I__G_X_LEN;

extern uint8_t T3I__X[];
extern uint32_t T3I__X_LEN;

extern uint8_t T3I__C_I[];
extern uint32_t T3I__C_I_LEN;

extern uint8_t T3I__AD_1[];
extern uint32_t T3I__AD_1_LEN;

extern uint8_t T3I__AD_3[];
extern uint32_t T3I__AD_3_LEN;

extern uint8_t T3I__I[];
extern uint32_t T3I__I_LEN;

extern uint8_t T3I__G_I[];
extern uint32_t T3I__G_I_LEN;

extern uint8_t T3I__ID_CRED_I[];
extern uint32_t T3I__ID_CRED_I_LEN;

extern uint8_t T3I__CRED_I[];
extern uint32_t T3I__CRED_I_LEN;

extern uint8_t T3I__SK_I[];
extern uint32_t T3I__SK_I_LEN;

extern uint8_t T3I__PK_I[];
extern uint32_t T3I__PK_I_LEN;

/*other party credentials*/
extern uint8_t T3I__ID_CRED_R[];
extern uint32_t T3I__ID_CRED_R_LEN;

extern uint8_t T3I__CRED_R[];
extern uint32_t T3I__CRED_R_LEN;

extern uint8_t T3I__PK_R[];
extern uint32_t T3I__PK_R_LEN;

extern uint8_t T3I__G_R[];
extern uint32_t T3I__G_R_LEN;

extern uint8_t T3I__CA[];
extern uint32_t T3I__CA_LEN;

extern uint8_t T3I__CA_PK[];
extern uint32_t T3I__CA_PK_LEN;

/******************************************************************************/
/*                              Responder specific                            */
/******************************************************************************/

extern uint8_t T3R__SUITES_R[];
extern uint32_t T3R__SUITES_R_LEN;

extern uint8_t T3R__G_Y[];
extern uint32_t T3R__G_Y_LEN;

extern uint8_t T3R__Y[];
extern uint32_t T3R__Y_LEN;

extern uint8_t T3R__C_R[];
extern uint32_t T3R__C_R_LEN;

extern uint8_t T3R__AD_2[];
extern uint32_t T3R__AD_2_LEN;

extern uint8_t T3R__ID_CRED_R[];
extern uint32_t T3R__ID_CRED_R_LEN;

extern uint8_t T3R__CRED_R[];
extern uint32_t T3R__CRED_R_LEN;

extern uint8_t T3R__SK_R[];
extern uint32_t T3R__SK_R_LEN;

extern uint8_t T3R__PK_R[];
extern uint32_t T3R__PK_R_LEN;

extern uint8_t T3R__R[];
extern uint32_t T3R__R_LEN;

extern uint8_t T3R__G_R[];
extern uint32_t T3R__G_R_LEN;

/*other party credentials*/
extern uint8_t T3R__ID_CRED_I[];
extern uint32_t T3R__ID_CRED_I_LEN;

extern uint8_t T3R__CRED_I[];
extern uint32_t T3R__CRED_I_LEN;

extern uint8_t T3R__PK_I[];
extern uint32_t T3R__PK_I_LEN;

extern uint8_t T3R__G_I[];
extern uint32_t T3R__G_I_LEN;

extern uint8_t T3R__CA[];
extern uint32_t T3R__CA_LEN;

extern uint8_t T3R__CA_PK[];
extern uint32_t T3R__CA_PK_LEN;

/*
 * Test 4
 * |mode                          | RPK/Cert | suite    | Ref [1]
 * |INITIATOR_SDHK_RESPONDER_SDHK | x5chain  | 0        | none
 *
 * [1]:https://github.com/lake-wg/edhoc/blob/master/test-vectors/vectors.txt
 */
/******************************************************************************/
/*                              Outputs                                       */
/******************************************************************************/

extern uint8_t T4_PRK_4X3M[];
extern uint32_t T4_PRK_4X3M_LEN;

extern uint8_t T4_TH4[];
extern uint32_t T4_TH4_LEN;

extern uint8_t T4_OSCORE_MSECRET[];
extern uint32_t T4_OSCORE_MSECRET_LEN;

extern uint8_t T4_OSCORE_MSALT[];
extern uint32_t T4_OSCORE_MSALT_LEN;

/******************************************************************************/
/*                              Messages                                      */
/******************************************************************************/

extern uint8_t T4_MSG_1[];
extern uint32_t T4_MSG_1_LEN;

extern uint8_t T4_MSG_2[];
extern uint32_t T4_MSG_2_LEN;

extern uint8_t T4_MSG_3[];
extern uint32_t T4_MSG_3_LEN;

/******************************************************************************/
/*                              Initiator specific                            */
/******************************************************************************/

extern enum method_type T4I__METHOD_TYPE;
extern uint8_t T4I__CORR;

extern uint8_t T4I__SUITES_I[];
extern uint32_t T4I__SUITES_I_LEN;

extern uint8_t T4I__G_X[];
extern uint32_t T4I__G_X_LEN;

extern uint8_t T4I__X[];
extern uint32_t T4I__X_LEN;

extern uint8_t T4I__C_I[];
extern uint32_t T4I__C_I_LEN;

extern uint8_t T4I__AD_1[];
extern uint32_t T4I__AD_1_LEN;

extern uint8_t T4I__AD_3[];
extern uint32_t T4I__AD_3_LEN;

extern uint8_t T4I__I[];
extern uint32_t T4I__I_LEN;

extern uint8_t T4I__G_I[];
extern uint32_t T4I__G_I_LEN;

extern uint8_t T4I__ID_CRED_I[];
extern uint32_t T4I__ID_CRED_I_LEN;

extern uint8_t T4I__CRED_I[];
extern uint32_t T4I__CRED_I_LEN;

extern uint8_t T4I__SK_I[];
extern uint32_t T4I__SK_I_LEN;

extern uint8_t T4I__PK_I[];
extern uint32_t T4I__PK_I_LEN;

/*other party credentials*/
extern uint8_t T4I__ID_CRED_R[];
extern uint32_t T4I__ID_CRED_R_LEN;

extern uint8_t T4I__CRED_R[];
extern uint32_t T4I__CRED_R_LEN;

extern uint8_t T4I__PK_R[];
extern uint32_t T4I__PK_R_LEN;

extern uint8_t T4I__G_R[];
extern uint32_t T4I__G_R_LEN;

extern uint8_t T4I__CA[];
extern uint32_t T4I__CA_LEN;

extern uint8_t T4I__CA_PK[];
extern uint32_t T4I__CA_PK_LEN;

/******************************************************************************/
/*                              Responder specific                            */
/******************************************************************************/

extern uint8_t T4R__SUITES_R[];
extern uint32_t T4R__SUITES_R_LEN;

extern uint8_t T4R__G_Y[];
extern uint32_t T4R__G_Y_LEN;

extern uint8_t T4R__Y[];
extern uint32_t T4R__Y_LEN;

extern uint8_t T4R__C_R[];
extern uint32_t T4R__C_R_LEN;

extern uint8_t T4R__AD_2[];
extern uint32_t T4R__AD_2_LEN;

extern uint8_t T4R__ID_CRED_R[];
extern uint32_t T4R__ID_CRED_R_LEN;

extern uint8_t T4R__CRED_R[];
extern uint32_t T4R__CRED_R_LEN;

extern uint8_t T4R__SK_R[];
extern uint32_t T4R__SK_R_LEN;

extern uint8_t T4R__PK_R[];
extern uint32_t T4R__PK_R_LEN;

extern uint8_t T4R__R[];
extern uint32_t T4R__R_LEN;

extern uint8_t T4R__G_R[];
extern uint32_t T4R__G_R_LEN;

/*other party credentials*/
extern uint8_t T4R__ID_CRED_I[];
extern uint32_t T4R__ID_CRED_I_LEN;

extern uint8_t T4R__CRED_I[];
extern uint32_t T4R__CRED_I_LEN;

extern uint8_t T4R__PK_I[];
extern uint32_t T4R__PK_I_LEN;

extern uint8_t T4R__G_I[];
extern uint32_t T4R__G_I_LEN;

extern uint8_t T4R__CA[];
extern uint32_t T4R__CA_LEN;

extern uint8_t T4R__CA_PK[];
extern uint32_t T4R__CA_PK_LEN;
#endif /*TEST_VECTORS_EDHOC*/
