#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define HIGH 1
#define LOW 0

void digitalWrite_A(int pin, int level);
void digitalWrite_B(int pin, int level);

#ifdef __cplusplus
}
#endif
