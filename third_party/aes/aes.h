/*
 *  FIPS-197 compliant AES implementation
 *
 *  Copyright (C) 2006-2007  Christophe Devine
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License, version 2.1 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

/*
 * This file was taken from xyssl-0.7 and modified by Luminary Micro.
 */

/**
 * \file aes.h
 */
#ifndef XYSSL_AES_H
#define XYSSL_AES_H

// configuration is important
#include "aes_config_opts.h"
#include "aes_config.h"

#define AES_ENCRYPT     0
#define AES_DECRYPT     1

#if KEY_FORM == KEY_SET
# define ROUND_KEYS     ctx->buf // expanded key
# define NUM_ROUNDS     ctx->nr // number of rounds
# define ECB_CONTEXT    aes_context *ctx
# define ECB_ARG        ctx
/**
 * \brief          AES context structure, not used for pre-set keys
 */
typedef struct
{
    int nr;                     /*!<  number of rounds  */
    unsigned buf[AESCONSZ];     /*!<  key after processing for rounds    */
} aes_context;
#else
# define ROUND_KEYS     key_expand
# define NUM_ROUNDS     FIXED_NUM_ROUNDS
# define ECB_CONTEXT    const unsigned *key_expand
# define ECB_ARG        key_expand
#endif


#ifdef __cplusplus
extern "C" {
#endif

#if KEY_FORM == KEY_SET
/**
 * \brief          AES key schedule (encryption) - used when not pre-set keys
 *
 * \param ctx      AES context to be initialized
 * \param key      encryption key
 * \param keysize  must be 128, 192 or 256
 */
void aes_setkey_enc( aes_context *ctx, const unsigned char *key, int keysize );

/**
 * \brief          AES key schedule (decryption) - used when not pre-set keys
 *
 * \param ctx      AES context to be initialized
 * \param key      decryption key
 * \param keysize  must be 128, 192 or 256
 */
void aes_setkey_dec( aes_context *ctx, const unsigned char *key, int keysize );

#endif                          // KEY_FORM != KEY_PRESET

/**
 * \brief          AES-ECB block encryption/decryption
 *
 * \param ctx      AES context
 * \param mode     AES_ENCRYPT or AES_DECRYPT
 * \param input    16-byte input block
 * \param output   16-byte output block
 */
void aes_crypt_ecb( ECB_CONTEXT,
                    int mode,
                    const unsigned char input[16],
                    unsigned char output[16] );

#if PROCESSING_MODE&MODE_CBC
/**
 * \brief          AES-CBC buffer encryption/decryption
 *
 * \param ctx      AES context
 * \param mode     AES_ENCRYPT or AES_DECRYPT
 * \param length   length of the input data
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 */
void aes_crypt_cbc( ECB_CONTEXT,
                    int mode,
                    int length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output );
#endif                  // MODE_CBC

#if PROCESSING_MODE&MODE_CFB
/**
 * \brief          AES-CFB buffer encryption/decryption
 *
 * \param ctx      AES context
 * \param mode     AES_ENCRYPT or AES_DECRYPT
 * \param length   length of the input data
 * \param iv_off   offset in IV (updated after use)
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 */
void aes_crypt_cfb( ECB_CONTEXT,
                    int mode,
                    int length,
                    int *iv_off,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output );
#endif                  // MODE_CFB

#if PROCESSING_MODE&MODE_CTR
/**
 * \brief          AES-CTR buffer encryption/decryption
 *
 * \param ctx      AES context
 * \param mode     AES_ENCRYPT or AES_DECRYPT
 * \param length   length of the input data (must be multiple of 16)
 * \param iv       initialization vector (bottom two bytes modified)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 * \param count    current count to use (for counter), modified after
 */
void aes_crypt_ctr( ECB_CONTEXT,
                    int mode,
                    int length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output,
                    unsigned short *count );
#endif                  // MODE_CTR


/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int aes_self_test( int verbose );

#ifdef __cplusplus
}
#endif

#endif /* aes.h */
