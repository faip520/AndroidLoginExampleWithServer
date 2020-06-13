#include <iostream>

extern "C" {
#include <openssl/ssl.h>
}

extern "C" {
#include <openssl/rsa.h>
}

extern "C" {
#include <openssl/err.h>
}

#include <openssl/bio.h>

//#include <openssl/rsa.h>
//#include <openssl/pem.h>
//#include <openssl/err.h>

#include <android/log.h>

#define LOG_TAG "Faip"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

#include "openssl_rsa.h"

using namespace std;

RSA* createPublicRSA(unsigned char * key)
{
    LOGD("2222 RSA public key = %s", key);

    RSA *rsa = RSA_new();;
    BIO *keybio ;
    keybio = BIO_new_mem_buf(key, strlen((char*)key));
    if (keybio == NULL)
    {
        printf( "Failed to create key BIO");
        return 0;
    }
    rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);

    BIO_free_all(keybio);

    return rsa;
}

RSA * createPrivateRSA(unsigned char * key)
{
    RSA *rsa= NULL;
    BIO *keybio ;
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio==NULL)
    {
        printf( "Failed to create key BIO");
        return 0;
    }

    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);

    BIO_free_all(keybio);

    return rsa;
}

int private_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted)
{
    RSA * rsa = createPrivateRSA(key);
    int  result = RSA_private_decrypt(data_len,enc_data,decrypted,rsa,RSA_PKCS1_OAEP_PADDING);
    RSA_free(rsa);
    return result;
}

int public_encrypt(unsigned char * data,int data_len, unsigned char * key, unsigned char *encrypted)
{
    RSA * rsa = createPublicRSA(key );

    int result = RSA_public_encrypt(data_len, data, encrypted, rsa, RSA_PKCS1_OAEP_PADDING);

    RSA_free(rsa);
    return result;
}
