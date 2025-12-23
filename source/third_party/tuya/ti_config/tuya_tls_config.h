#ifndef TUYA_TLS_CONFIG_H
#define TUYA_TLS_CONFIG_H

/* Basic mbedTLS configuration for Tuya */
#define MBEDTLS_RSA_C
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_MD5_C
#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_AES_C
#define MBEDTLS_OID_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C

/* Optimization for embedded */
#define MBEDTLS_MPI_MAX_SIZE 256

#endif /* TUYA_TLS_CONFIG_H */