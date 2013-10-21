/*
 * Copyright (c) 2013 Kevin Birch <kmb@pobox.com>.  All rights reserved.
 * 
 * Distributed under an [MIT-style][license] license.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal with
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimers.
 * - Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimers in the documentation and/or
 *   other materials provided with the distribution.
 * - Neither the names of the copyright holders, nor the names of the authors, nor
 *   the names of other contributors may be used to endorse or promote products
 *   derived from this Software without specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE CONTRIBUTORS
 * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE SOFTWARE.
 *
 * [license]: http://www.opensource.org/licenses/ncsa
 */

#pragma once

#include <stdbool.h>

#include "hash.h"

typedef struct hashtable_s Hashtable;

typedef bool (*compare_function)(void *key1, void *key2);
typedef bool (*hashtable_map_function)(void *key, void *value, void *context);
typedef bool (*hashtable_map_item_function)(void *item, void *context);

Hashtable *make_hashtable(compare_function comparitor);
Hashtable *make_hashtable_with_function(compare_function comparitor,
                                        hash_function function);
Hashtable *make_hashtable_with_capacity(compare_function comparitor,
                                        size_t capacity_hint);
Hashtable *make_hashtable_with_capacity_function(compare_function comparitor,
                                                 size_t capacity_hint,
                                                 hash_function function);
Hashtable *make_hashtable_with_capacity_factor(compare_function comparitor,
                                               size_t capacity_hint,
                                               float load_factor);
Hashtable *make_hashtable_with_capacity_factor_function(compare_function comparitor,
                                                        size_t capacity_hint, 
                                                        float load_factor, 
                                                        hash_function function);

void hashtable_free(Hashtable *hashtable);

size_t hashtable_size(Hashtable *hashtable);
size_t hashtable_capacity(Hashtable *hashtable);
bool hashtable_is_empty(Hashtable *hashtable);
float hashtable_load_factor(Hashtable *hashtable);
bool hashtable_is_mutable(Hashtable *hashtable);
void hashtable_set_mutable(Hashtable *hashtable);
void hashtable_set_immutable(Hashtable *hashtable);
bool hashtable_is_immutable(Hashtable *hashtable);

bool hashtable_equals(Hashtable *hashtable1, Hashtable *hashtable2, compare_function comparitor);

bool hashtable_contains(Hashtable *hashtable, void *key);
void *hashtable_get(Hashtable *hashtable, void *key);
void *hashtable_get_if_absent(Hashtable *hashtable, void *key, void *value);
void *hashtable_get_if_absent_put(Hashtable *hashtable, void *key, void *value);
void *hashtable_put(Hashtable *hashtable, void *key, void *value);
void hashtable_put_all(Hashtable *to, Hashtable *from);
Hashtable *hashtable_copy(Hashtable *hashtable);
void *hashtable_remove(Hashtable *hashtable, void *key);
void hashtable_clear(Hashtable *hashtable);

bool hashtable_map(Hashtable *hashtable, hashtable_map_function function, void *context);
bool hashtable_map_keys(Hashtable *hashtable, hashtable_map_item_function function, void *context);
bool hashtable_map_values(Hashtable *hashtable, hashtable_map_item_function function, void *context);

void hashtable_summary(Hashtable *hashtable, FILE *stream);
