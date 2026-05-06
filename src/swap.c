#include "swap.h"

void int_swap(int* a , int* b ){
    int temp = *a;
    *a = *b;
    *b = temp;

}

void float_swap(float* a , float* b ){
    float temp = *a;
    *a = *b;
    *b = temp;

}
