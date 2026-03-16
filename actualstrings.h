#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define str_of_dyn(dynptr) ((dynptr)->__internal)

#define long long long

typedef enum Result {
  AS_SUCCESS,
  AS_NOT_SANE,
  AS_CANNOT_ALLOCATE,
  AS_OUT_OF_BOUNDS,
  AS_EMPTY,
  AS_BAD_ARG,
} Result;

typedef struct DynamicString {
  char* __internal;
  size_t __capacity;
  size_t length;
  unsigned long __integrity;
} DynamicString;

DynamicString ds_from(char* init);
DynamicString ds_new();
bool check_ds(DynamicString* ds);
Result ds_reserve(DynamicString* ds, size_t size);
Result ds_append(DynamicString* ds, const char* str);

#undef long
