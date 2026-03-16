#include "actualstrings.h"

#define long long long
#define BASE_CAP 64
#define ceildiv(a, b) (((a) + (b) - 1) / (b))
#define INTEGRITY_FLAG 0xCAFEBABEDEADBEEFFEEBDAEDEBABEFAC

DynamicString ds_from(char* init) {
  size_t sz = strlen(init);
  size_t needed_cap = ceildiv(sz - 1, BASE_CAP);
  return (DynamicString) {
    .__capacity = needed_cap,
    .length = sz,
    .__internal = (char*)malloc(needed_cap*sizeof(char)),
    .__integrity = INTEGRITY_FLAG,
  };
}

DynamicString ds_new() {
  return (DynamicString) {
    .__capacity = BASE_CAP,
    .length = 0,
    .__internal = (char*)malloc(BASE_CAP*sizeof(char)),
    .__integrity = INTEGRITY_FLAG,
  };
}

bool check_ds(DynamicString* ds) {
  return ds->__integrity == INTEGRITY_FLAG;
}

AS_Result ds_reserve(DynamicString* ds, size_t size) {
  char* new_ptr = (char*)realloc(ds->__internal, size * sizeof(char));
  if (new_ptr == NULL) {
    return AS_CANNOT_ALLOCATE;
  }
  ds->__internal = new_ptr;
  ds->__capacity = size;
  return AS_SUCCESS;
}

AS_Result ds_append(DynamicString* ds, const char* str) {
  if (!check_ds(ds)) {
    return AS_NOT_SANE;
  }
  if (str == NULL) {
    return AS_BAD_ARG;
  }
  size_t str_size = strlen(str);

  if (ds->length + str_size + 1 > ds->__capacity) {
    size_t factor = 2;
    while (ds->length + str_size + 1 > factor * ds->__capacity) {
      factor *= 2;
    }
    AS_Result r = ds_reserve(ds, factor * ds->__capacity);
    if (r) {
      return r;
    }
  }

  memcpy(ds->__internal + ds->length, str, str_size);
  ds->length += str_size;
  ds->__internal[ds->length] = 0;

  return AS_SUCCESS;
}
