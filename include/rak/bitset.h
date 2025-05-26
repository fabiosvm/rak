//
// bitset.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_BITSET_H
#define RAK_BITSET_H

#include <stdint.h>
#include "error.h"

#define RAK_BITSET_BITS_PER_WORD (sizeof(uint32_t) << 3)

#define rak_bitset_nbits_to_len(n) ((n) / RAK_BITSET_BITS_PER_WORD + ((n) % RAK_BITSET_BITS_PER_WORD ? 1 : 0))

typedef struct
{
  int       nbits;
  uint32_t *data;
} RakBitSet;

void rak_bitset_init(RakBitSet *bs, int nbits, RakError *err);
void rak_bitset_deinit(RakBitSet *bs);
void rak_bitset_ensure_bits(RakBitSet *bs, int nbits, RakError *err);
void rak_bitset_set_bit(RakBitSet *bs, int idx, RakError *err);
int rak_bitset_is_bit_set(const RakBitSet *bs, int idx);
void rak_bitset_clear_bit(RakBitSet *bs, int idx, RakError *err);
void rak_bitset_toggle_bit(RakBitSet *bs, int idx, RakError *err);
void rak_bitset_clear(RakBitSet *bs);

#endif // RAK_BITSET_H
