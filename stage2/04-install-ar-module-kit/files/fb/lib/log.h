#pragma once
#include <stdio.h>
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG(str, ...) printf("[%s:%d] " str, __FILENAME__, __LINE__, ##__VA_ARGS__)
