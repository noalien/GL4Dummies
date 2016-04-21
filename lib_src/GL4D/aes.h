/*!\file aes.h
 *
 *  \brief Utilisée pour décrypter des shaders données en archive cryptée AES. 
 * 
 *  FIPS-197 compliant AES implementation\n
 *
 *  Copyright (C) 2003-2006  Christophe Devine\n
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License, version 2.1 as published by the Free Software Foundation.\n
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.\n
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA\n
 *
 *  The AES block cipher was designed by Vincent Rijmen and Joan Daemen.\n
 *
 *  http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf\n
 *  http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf\n
 *
 *  \author Christophe Devine
 */
#ifndef _AES_H
#define _AES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          AES context structure
 */
typedef struct
{
    unsigned long erk[64];     /*!< encryption round keys */
    unsigned long drk[64];     /*!< decryption round keys */
    int nr;                    /*!< number of rounds      */
}
aes_context;

/**
 * \brief          AES key schedule
 *
 * \param ctx      AES context to be initialized
 * \param key      the secret key
 * \param keysize  must be 128, 192 or 256
 */
void aes_set_key( aes_context *ctx, unsigned char *key, int keysize );

/**
 * \brief          AES block encryption (ECB mode)
 *
 * \param ctx      AES context
 * \param input    plaintext  block
 * \param output   ciphertext block
 */
void aes_encrypt( aes_context *ctx,
                  unsigned char input[16],
                  unsigned char output[16] );

/**
 * \brief          AES block decryption (ECB mode)
 *
 * \param ctx      AES context
 * \param input    ciphertext block
 * \param output   plaintext  block
 */
void aes_decrypt( aes_context *ctx,
                  unsigned char input[16],
                  unsigned char output[16] );

/**
 * \brief          AES-CBC buffer encryption
 *
 * \param ctx      AES context
 * \param iv       initialization vector (modified after use)
 * \param input    buffer holding the plaintext
 * \param output   buffer holding the ciphertext
 * \param len      length of the data to be encrypted
 */
void aes_cbc_encrypt( aes_context *ctx,
                      unsigned char iv[16],
                      unsigned char *input,
                      unsigned char *output,
                      int len );

/**
 * \brief          AES-CBC buffer decryption
 *
 * \param ctx      AES context
 * \param iv       initialization vector (modified after use)
 * \param input    buffer holding the ciphertext
 * \param output   buffer holding the plaintext
 * \param len      length of the data to be decrypted
 */
void aes_cbc_decrypt( aes_context *ctx,
                      unsigned char iv[16],
                      unsigned char *input,
                      unsigned char *output,
                      int len );

extern void vaetvient(unsigned char * data, int len, int vaouvient);
extern char * aes_from_tar(const char * file);

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int aes_self_test( void );

#ifdef __cplusplus
}
#endif

#endif /* aes.h */
