#ifndef _ENCODE_UTILS_H_
#define _ENCODE_UTILS_H_
#endif // !_ENCODE_UTILS_H_


const char* base64_encode(const unsigned char* text, int size, char* buf, int* out_len);
int base64_decode(const char* text, int size, char* buf);
