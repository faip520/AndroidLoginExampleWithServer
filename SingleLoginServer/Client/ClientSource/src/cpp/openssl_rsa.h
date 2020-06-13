/*
 * @author  Batuhan AVLAYAN - iletisim@batuhanavlayan.com
 * @version V1.0
 * @brief   This file declares a programming API for OpenSSL RSA encryption and decryption operations.
 */

#ifndef RSA_ALGORITHM_H
#define RSA_ALGORITHM_H

#define KEY_LENGTH       2048
#define PUBLIC_EXPONENT  59     //Public exponent should be a prime number.
#define PUBLIC_KEY_PEM   1
#define PRIVATE_KEY_PEM  0

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#define LOG(x)               \
        cout << x << endl;   \


RSA* createPublicRSA(unsigned char * key);
int public_encrypt(unsigned char * data,int data_len, unsigned char * key, unsigned char *encrypted);
RSA * createPrivateRSA(unsigned char * key);
int private_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted);

#endif //RSA_ALGORITHM_H