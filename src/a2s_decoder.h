#ifndef A2S_CHUNK_DECODER_H
#define A2S_CHUNK_DECODER_H

#include <stdint.h>
#include <stddef.h>


bool decode_a2s_escapes(uint8_t* buffer, size_t size, uint8_t* output, size_t output_max);




#endif
