#ifndef MD5_H
#define MD5_H

#include "type.h"

int md5(const char *msg, size_t msg_len, byte *buf, size_t buf_size, char *out, size_t out_size);

#endif
