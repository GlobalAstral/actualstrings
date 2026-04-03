#include "actualstrings.h"

#define long long long
#define BASE_CAP 64
#define ceildiv(a, b) (((a) + (b) - 1) / (b))
#define INTEGRITY_FLAG 0xCAFEBABEDEADBEEF

DynamicString ds_from(char* init) {
  size_t sz = strlen(init);
  size_t needed_cap = ceildiv(sz - 1, BASE_CAP);
  char* buf = (char*)malloc(needed_cap*sizeof(char));
  memcpy(buf, init, sz);
  buf[sz] = 0;
  return (DynamicString) {
    .__capacity = needed_cap,
    .length = sz,
    .__internal = buf,
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

AS_Result ds_insert(DynamicString* ds, size_t index, const char* str) {
  if (!check_ds(ds)) {
    return AS_NOT_SANE;
  }
  if (str == NULL) {
    return AS_BAD_ARG;
  }
  if (index > ds->length) {
    return AS_OUT_OF_BOUNDS;
  }
  size_t size = strlen(str);
  if (ds->length + size + 1 > ds->__capacity) {
    size_t factor = 2;
    while (ds->length + size + 1 > factor * ds->__capacity) {
      factor *= 2;
    }
    AS_Result r = ds_reserve(ds, factor * ds->__capacity);
    if (r) {
      return r;
    }
  }
  memmove(ds->__internal + index + size, ds->__internal + index, ds->length - index + 1);
  memmove(ds->__internal + index, str, size);
  ds->length += size;
  return AS_SUCCESS;
}

Slice trim(Slice s) {
  size_t start = 0;
  size_t end = s.length;

  while (start < end && isspace((unsigned char)s.bytes[start]))
    start++;

  while (end > start && isspace((unsigned char)s.bytes[end - 1]))
    end--;

  Slice out = { s.bytes + start, end - start };
  return out;
}

size_t split(DynamicString* ds, char sep, Slice** buffer) {
  size_t token_amount = 1;
  for (size_t i = 0; i < ds->length; i++) {
    if (str_of_dyn(ds)[i] == sep) {
      token_amount++;
    }
  }
  *buffer = (Slice*)malloc(token_amount * sizeof(Slice));

  char* str = str_of_dyn(ds);

  size_t base = 0;
  size_t slice_index = 0;
  for (size_t i = 0; i < ds->length; i++) {
    char ch = str[i];
    if (ch == sep) {
      Slice sl = {.bytes = str + base, .length = i - base};
      base = i + 1;
      (*buffer)[slice_index++] = sl;
    }
  }
  Slice sl = {.bytes = str + base, .length = ds->length - base};
  (*buffer)[slice_index++] = sl;
  return slice_index;
}
