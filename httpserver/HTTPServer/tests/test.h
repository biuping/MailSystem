#pragma once
#include "my.h" //自己定义的头文件，写上define即可，在.gitignore中已忽略

#ifdef MAO_TEST
void start_mao();
#define TEST_START start_mao
#endif

#ifdef ZHANG_TEST
void start_zhang();
#define TEST_START start_zhang
#endif

#ifdef HUANG_TEST
void start_huang();
#define TEST_START start_huang
#endif