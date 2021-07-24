#ifndef TINYGRAPH_VBYTE_H
#define TINYGRAPH_VBYTE_H

#include <stdint.h>

#include "tinygraph-utils.h"


TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_vbyte_encode(const uint32_t *data, unsigned char *out, uint32_t n);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_vbyte_decode(const unsigned char *data, uint32_t *out, uint32_t n);


#endif
