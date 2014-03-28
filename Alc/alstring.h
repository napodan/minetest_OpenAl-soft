#ifndef ALSTRING_H
#define ALSTRING_H

#include "vector.h"

typedef char al_string_char_type;
DECL_VECTOR(al_string_char_type)

typedef vector_al_string_char_type al_string;
typedef const_vector_al_string_char_type const_al_string;

#define AL_STRING_INIT(_x)   do { (_x) = calloc(1, sizeof(*(_x)) + sizeof((_x)->Data[0])); } while(0)
#define AL_STRING_DEINIT(_x) VECTOR_DEINIT(_x)

inline ALsizei al_string_length(const_al_string str)
{ return VECTOR_SIZE(str); }

inline ALsizei al_string_empty(const_al_string str)
{ return al_string_length(str) == 0; }

inline const al_string_char_type *al_string_get_cstr(const_al_string str)
{ return &VECTOR_FRONT(str); }

void al_string_clear(al_string *str);

void al_string_copy(al_string *str, const_al_string from);
void al_string_copy_cstr(al_string *str, const al_string_char_type *from);

void al_string_append_char(al_string *str, const al_string_char_type c);
void al_string_append_range(al_string *str, const al_string_char_type *from, const al_string_char_type *to);

#endif /* ALSTRING_H */