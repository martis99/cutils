#ifndef C_MD5_H
#define C_MD5_H

#include "type.h"

int c_md5(const char *msg, size_t msg_len, byte *buf, size_t buf_size, char *out, size_t out_size);

#endif
