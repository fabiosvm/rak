//
// bitset.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/bitset.h"
#include "rak/memory.h"

void rak_bitset_init(RakBitSet *bs, int nbits, RakError *err)
{
  int len = rak_bitset_nbits_to_len(nbits);
  int size = sizeof(*(bs->data)) * len;
  uint32_t *data = rak_memory_alloc(size, err);
  if (!rak_is_ok(err)) return;
  for (int i = 0; i < len; ++i)
    data[i] = 0;
  bs->nbits = len * RAK_BITSET_BITS_PER_WORD;
  bs->data = data;
}

void rak_bitset_deinit(RakBitSet *bs)
{
  rak_memory_free(bs->data);
}

void rak_bitset_ensure_bits(RakBitSet *bs, int nbits, RakError *err)
{
  int _nbits = bs->nbits;
  if (nbits <= _nbits) return;
  int len = _nbits / RAK_BITSET_BITS_PER_WORD;
  int _len = rak_bitset_nbits_to_len(nbits);
  int size = sizeof(*(bs->data)) * _len;
  uint32_t *data = rak_memory_realloc(bs->data, size, err);
  if (!rak_is_ok(err)) return;
  for (int i = len; i < _len; ++i)
    data[i] = 0;
  bs->data = data;
  bs->nbits = _len * RAK_BITSET_BITS_PER_WORD;
}

void rak_bitset_set_bit(RakBitSet *bs, int idx, RakError *err)
{
  rak_bitset_ensure_bits(bs, idx + 1, err);
  if (!rak_is_ok(err)) return;
  int _idx = idx / RAK_BITSET_BITS_PER_WORD;
  uint32_t word = bs->data[_idx];
  bs->data[_idx] = word | (1U << (idx % RAK_BITSET_BITS_PER_WORD));
}

int rak_bitset_is_bit_set(const RakBitSet *bs, int idx)
{
  int _idx = idx / RAK_BITSET_BITS_PER_WORD;
  uint32_t word = bs->data[_idx];
  return (word & (1U << (idx % RAK_BITSET_BITS_PER_WORD))) ? 1 : 0;
}

void rak_bitset_clear_bit(RakBitSet *bs, int idx, RakError *err)
{
  rak_bitset_ensure_bits(bs, idx + 1, err);
  if (!rak_is_ok(err)) return;
  int _idx = idx / RAK_BITSET_BITS_PER_WORD;
  bs->data[_idx] &= ~(1U << (idx % RAK_BITSET_BITS_PER_WORD));
}

void rak_bitset_toggle_bit(RakBitSet *bs, int idx, RakError *err)
{
  rak_bitset_ensure_bits(bs, idx + 1, err);
  if (!rak_is_ok(err)) return;
  int _idx = idx / RAK_BITSET_BITS_PER_WORD;
  bs->data[_idx] ^= (1U << (idx % RAK_BITSET_BITS_PER_WORD));
}

void rak_bitset_clear(RakBitSet *bs)
{
  int len = rak_bitset_nbits_to_len(bs->nbits);
  for (int i = 0; i < len; ++i)
    bs->data[i] = 0;
}
