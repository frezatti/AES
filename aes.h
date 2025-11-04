#ifndef AES_H
#define AES_H

#include <stddef.h>
#include <stdint.h>

#define AES_BLOCK_SIZE 16
#define AES_MAX_ROUNDS 14
#define AES_MAX_ROUND_WORDS (4 * (AES_MAX_ROUNDS + 1))

typedef struct {
  int Nk;
  int Nr;
  uint8_t w[AES_MAX_ROUND_WORDS][4];
} AES_ctx;

int aes_init(AES_ctx *ctx, const uint8_t *key, int key_bits);

void aes_encrypt_block(const AES_ctx *ctx, const uint8_t in[AES_BLOCK_SIZE],
                       uint8_t out[AES_BLOCK_SIZE]);

void aes_decrypt_block(const AES_ctx *ctx, const uint8_t in[AES_BLOCK_SIZE],
                       uint8_t out[AES_BLOCK_SIZE]);

#endif
