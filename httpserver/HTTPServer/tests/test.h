#pragma once
#include "my.h" //�Լ������ͷ�ļ���д��define���ɣ���.gitignore���Ѻ���

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