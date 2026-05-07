// SPDX-License-Identifier: GPL-3.0-or-later

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "context/string_interner.h"
#include "support/string_constant.h"

// strings_hash_map
// strings_index_map

struct _pluto_StringInternerString {
  uint32_t             _index;
  uint32_t             _hash;
  pluto_StringConstant _string;
};

struct _pluto_StringInterner {
  uint32_t                            _strings_length;
  uint32_t                            _strings_capacity;
  struct _pluto_StringInternerString *_strings_buffer;
  uint32_t                            _views_length;
  uint32_t                            _views_capacity;
  pluto_StringView                   *_views_buffer;
};

static bool _pluto_string_interner_strings_full(pluto_StringInterner si);
static uint32_t _pluto_string_interner_hash(pluto_StringView sv);
static struct _pluto_StringInternerString *
_pluto_string_interner_strings_search(struct _pluto_StringInternerString *buffer, 
                                      uint32_t capacity,
                                      pluto_StringView sv);
static void _pluto_string_interner_strings_insert(struct _pluto_StringInternerString *buffer,
                                                  uint32_t capacity,
                                                  struct _pluto_StringInternerString *string);
static void _pluto_string_interner_strings_grow(pluto_StringInterner si);
static bool _pluto_string_interner_views_full(pluto_StringInterner si);
static void _pluto_string_interner_views_grow(pluto_StringInterner si);
static uint32_t _pluto_string_interner_views_append(pluto_StringInterner si, 
                                                    pluto_StringView sv);

pluto_StringInterner pluto_string_interner_construct() {
  return calloc(1, sizeof(struct _pluto_StringInterner));
}

void pluto_string_interner_destruct(pluto_StringInterner *si) {
  assert(si != NULL);
  assert(*si != NULL);

  for (size_t i = 0; i < (*si)->_strings_capacity; ++i) {
    struct _pluto_StringInternerString *s = (*si)->_strings_buffer + i;
    if (!s->_string) { continue; }
    pluto_string_constant_destruct(&s->_string);
  }

  free((*si)->_strings_buffer);
  free((*si)->_views_buffer);
  free(*(void**)si);
  *si = NULL;
}

uint32_t pluto_string_interner_intern(pluto_StringInterner si, pluto_StringView sv) {
  assert(sv.data != NULL);

  // since lookup always succeeds, we just return the 
  // correct position for the new string to be constructed 
  // at, we have to check for growing the array before 
  // we search.
  if (_pluto_string_interner_strings_full(si)) {
    _pluto_string_interner_strings_grow(si);
  }
  // lookup view in strings, if !NULL return index
  // otherwise construct new string at position
  // and return the bound index. 
  // search has to fill in the hash,
  // search cannot fill in the index.
  struct _pluto_StringInternerString *found = 
    _pluto_string_interner_strings_search(si->_strings_buffer, si->_strings_capacity, sv);

  if (found->_string != NULL) {
    return found->_index;
  }

  // we are inserting a new string
  if (_pluto_string_interner_views_full(si)) {
    _pluto_string_interner_views_grow(si);
  }

  found->_string = pluto_string_constant_from_view(sv);
  found->_index  = _pluto_string_interner_views_append(si, sv); 
  return found->_index;
}

pluto_StringView pluto_string_interner_at(pluto_StringInterner si, uint32_t tag) {
  assert(tag < si->_views_length);
  return si->_views_buffer[tag];
}

/*!
 * \brief returns is the hash map is more than 3/4 full
 */
static bool _pluto_string_interner_strings_full(pluto_StringInterner si) {
  // since we are doing an integer division by 4, we can expect that 
  // the compiler can replace with a right shift by 2. We are leaving 
  // it like it is for readability. and in debug builds perf isnt a 
  // high priority.
  size_t limit = ((size_t)(si->_strings_capacity) * 3) / 4;
  return (si->_strings_length + 1) >= limit;
}

static uint32_t _pluto_string_interner_hash(pluto_StringView sv) {
  uint32_t hash = 5381;
  size_t i = 0;
  while (i < sv.length) {
    hash = ((hash << 5) + hash) + sv.data[i++];
  }
  return hash;
}

static struct _pluto_StringInternerString *
_pluto_string_interner_strings_search(struct _pluto_StringInternerString *buffer, 
                                      uint32_t capacity,
                                      pluto_StringView sv) {
  uint32_t hash = _pluto_string_interner_hash(sv);
  struct _pluto_StringInternerString *dst = buffer + (hash % capacity);
  dst->_hash = hash;
  return dst;
}

// since we store the hash value along with the string data.
// (we had some extra bits that were just padding anyways)
// we can optimize the reinsert procedure to make use of 
// precomputed hashes, which should speed up growing the 
// table by a factor of (N * sum(E, time(hash(e)))), that 
// is the number of elements we have to reinsert (N) multiplied
// by the sum over all elements of the time it takes to 
// hash each element.
static void _pluto_string_interner_strings_insert(struct _pluto_StringInternerString *buffer,
                                                  uint32_t capacity,
                                                  struct _pluto_StringInternerString *src) {
  struct _pluto_StringInternerString *dst = buffer + (src->_hash % capacity);
  memcpy(dst, src, sizeof(struct _pluto_StringInternerString));
}

static void _pluto_string_interner_strings_grow(pluto_StringInterner si) {
  size_t capacity = si->_strings_capacity == 0 ? 8 : si->_strings_capacity * 2;
  assert(capacity < UINT32_MAX);
  struct _pluto_StringInternerString *buffer = 
    calloc(capacity, sizeof(struct _pluto_StringInternerString));

  for (size_t i = 0; i < si->_strings_capacity; ++i) {
    struct _pluto_StringInternerString *s = si->_strings_buffer + i;
    if (!s->_string) { continue; }
    _pluto_string_interner_strings_insert(buffer, capacity, s);
  }

  free((void*)si->_strings_buffer);
  si->_strings_capacity = capacity;
  si->_strings_buffer = buffer;
}

static bool _pluto_string_interner_views_full(pluto_StringInterner si) {
  return (si->_views_length + 1) >= si->_views_capacity;
}

static void _pluto_string_interner_views_grow(pluto_StringInterner si) {
  size_t capacity = si->_views_capacity == 0 ? 8 : si->_views_capacity * 2;
  pluto_StringView *buffer = calloc(capacity, sizeof(pluto_StringView));
  memcpy(buffer, si->_views_buffer, si->_views_length * sizeof(pluto_StringView));
  free(si->_views_buffer);
  si->_views_capacity = capacity;
  si->_views_buffer   = buffer;
}

static uint32_t _pluto_string_interner_views_append(pluto_StringInterner si, 
                                                    pluto_StringView sv) {
  memcpy(si->_views_buffer + si->_views_length, &sv, sizeof(pluto_StringView));
  return si->_views_length++;
}

















