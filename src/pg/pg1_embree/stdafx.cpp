#include <stdafx.h>

void *operator new(size_t size) {
  totalAllocations++;
  return malloc(size);
}

void operator delete(void *p) {
  totalDelete++;
  free(p);
}