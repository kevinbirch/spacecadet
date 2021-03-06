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

#ifdef __clang__
#define __STDC_WANT_LIB_EXT1__ 1
#endif

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "vector.h"

static const rsize_t DEFAULT_CAPACITY = 4;

struct vector_s
{
    rsize_t    length;
    rsize_t    capacity;
    uint8_t **items;
};

static inline bool ensure_capacity(Vector *vector, rsize_t min_capacity);
static inline rsize_t calculate_new_capacity(rsize_t capacity);
static inline bool reallocate(Vector *vector, rsize_t capacity);


static bool add_to_vector_iterator(void *each, void *context);

Vector *make_vector(void)
{
    return make_vector_with_capacity(DEFAULT_CAPACITY);
}

Vector *make_vector_with_capacity(rsize_t capacity)
{
    if(0 == capacity)
    {
        errno = EINVAL;
        return NULL;
    }
    Vector *result = (Vector *)calloc(1, sizeof(Vector));
    if(NULL == result)
    {
        return NULL;
    }

    result->items = (uint8_t **)calloc(1, sizeof(uint8_t *) * capacity);
    if(NULL == result->items)
    {
        free(result);
        return NULL;
    }

    result->length = 0;
    result->capacity = capacity;
    return result;
}

Vector *make_vector_of(rsize_t count, ...)
{
    if(0 == count)
    {
        errno = EINVAL;
        return NULL;
    }

    Vector *result = make_vector_with_capacity(count);
    if(NULL == result)
    {
        return NULL;
    }

    va_list rest;
    va_start(rest, count);
    for(rsize_t i = 0; i < count; i++)
    {
        void *element = va_arg(rest, void *);
        result->items[i] = element;
    }
    va_end(rest);
    result->length = count;

    return result;
}

Vector *vector_copy(const Vector *vector)
{
    if(NULL == vector)
    {
        errno = EINVAL;
        return NULL;
    }

    if(0 == vector->length)
    {
        return make_vector_with_capacity(1);
    }

    Vector *result = make_vector_with_capacity(vector->length);
    if(NULL == result)
    {
        return NULL;
    }
    memcpy(result->items, vector->items, sizeof(uint8_t *) * vector->length);
    result->length = vector->length;

    return result;
}

Vector *vector_with(const Vector *vector, void *value)
{
    if(NULL == vector || NULL == value)
    {
        errno = EINVAL;
        return NULL;
    }

    Vector *result = vector_copy(vector);
    if(NULL == result)
    {
        return NULL;
    }

    vector_add(result, value);

    return result;
}

Vector *vector_with_all(const Vector *vector, const Vector *from)
{
    if(NULL == vector || NULL == from)
    {
        errno = EINVAL;
        return NULL;
    }

    Vector *result = vector_copy(vector);
    if(NULL == result)
    {
        return NULL;
    }

    vector_add_all(result, from);

    return result;
}

void vector_free(Vector *vector)
{
    if(NULL == vector)
    {
        return;
    }

    if(NULL != vector->items)
    {
        free(vector->items);
        vector->items = NULL;
    }
    free(vector);
}

void vector_destroy(Vector *vector, vector_item_destructor destructor)
{
    if(NULL == vector || NULL == destructor)
    {
        errno = EINVAL;
        return;
    }

    for(rsize_t i = 0; i < vector->length; i++)
    {
        destructor(vector->items[i]);
    }

    vector_free(vector);
}

rsize_t vector_length(const Vector *vector)
{
    if(NULL == vector)
    {
        errno = EINVAL;
        return 0;
    }

    return vector->length;
}

rsize_t vector_capacity(const Vector *vector)
{
    if(NULL == vector)
    {
        errno = EINVAL;
        return 0;
    }

    return vector->capacity;
}

bool vector_is_empty(const Vector *vector)
{
    if(NULL == vector)
    {
        errno = EINVAL;
        return true;
    }

    return 0 == vector->length;
}

void *vector_get(const Vector *vector, rsize_t index)
{
    if(NULL == vector || index >= vector->length)
    {
        errno = EINVAL;
        return NULL;
    }

    return vector->items[index];
}

void *vector_first(const Vector *vector)
{
    return vector_get(vector, 0);
}

void *vector_last(const Vector *vector)
{
    if(NULL == vector || 0 == vector->length)
    {
        errno = EINVAL;
        return NULL;
    }

    return vector_get(vector, vector->length - 1);
}

bool vector_add(Vector *vector, void *value)
{
    if(NULL == vector || NULL == value)
    {
        errno = EINVAL;
        return false;
    }

    if(!ensure_capacity(vector, vector->length + 1))
    {
        return false;
    }
    vector->items[vector->length++] = value;
    return true;
}

static bool add_to_vector_iterator(void *each, void *context)
{
    Vector *vector = (Vector *)context;
    return vector_add(vector, each);
}

bool vector_add_all(Vector *vector, const Vector *from)
{
    if(NULL == vector || NULL == from)
    {
        errno = EINVAL;
        return false;
    }

    if(!ensure_capacity(vector, vector->length + from->length))
    {
        return false;
    }

    return vector_iterate(from, add_to_vector_iterator, vector);
}

bool vector_insert(Vector *vector, void *value, rsize_t index)
{
    if(NULL == vector || NULL == value || index > vector->length)
    {
        errno = EINVAL;
        return NULL;
    }

    if(vector->length == index)
    {
        return vector_add(vector, value);
    }

    uint8_t **target = vector->items;
    if(vector->capacity < vector->length + 1)
    {
        rsize_t new_capacity = calculate_new_capacity(vector->capacity);
        target = calloc(1, sizeof(uint8_t *) * new_capacity);
        if(NULL == vector->items)
        {
            return false;
        }
        vector->capacity = new_capacity;
    }

    if(0 == index)
    {
        memmove(target + 1, vector->items, sizeof(uint8_t *) * vector->length);
        target[0] = value;
    }
    else
    {
        if(target != vector->items)
        {
            memcpy(target, vector->items, sizeof(uint8_t *) * index);
        }
        memmove(target + index + 1, vector->items + index,
                sizeof(uint8_t *) * (vector->length - index));
        target[index] = value;
    }

    if(target != vector->items)
    {
        free(vector->items);
        vector->items = target;
    }
    vector->length++;

    return true;
}

void *vector_set(Vector *vector, void *value, rsize_t index)
{
    if(NULL == vector || NULL == value || index >= vector->length)
    {
        errno = EINVAL;
        return NULL;
    }

    void *previous = vector->items[index];
    vector->items[index] = value;
    return previous;
}

void *vector_remove(Vector *vector, rsize_t index)
{
    if(NULL == vector || index >= vector->length)
    {
        errno = EINVAL;
        return NULL;
    }

    void *previous = vector->items[index];
    if(index == vector->length - 1)
    {
        vector->items[index] = NULL;
    }
    else
    {
        memmove(vector->items + index, vector->items + index + 1, sizeof(uint8_t *) * (vector->length - (index + 1)));
        vector->items[vector->length - 1] = NULL;
    }
    vector->length--;
    return previous;
}

bool vector_remove_item(Vector *vector, vector_comparitor comparitor, void *item)
{
    if(NULL == vector || NULL == comparitor || NULL == item)
    {
        errno = EINVAL;
        return false;
    }

    for(rsize_t i = 0; i < vector->length; i++)
    {
        if(comparitor(vector->items[i], item))
        {
            return NULL != vector_remove(vector, i);
        }
    }
    return false;
}

void vector_clear(Vector *vector)
{
    if(NULL == vector || 0 == vector->length)
    {
        return;
    }

    memset(vector->items, 0, sizeof(uint8_t *) * vector->length);
    vector->length = 0;
}

bool vector_trim(Vector *vector)
{
    if(NULL == vector || 0 == vector->length)
    {
        errno = EINVAL;
        return NULL;
    }

    if(vector->length == vector->capacity)
    {
        return false;
    }

    return reallocate(vector, vector->length);
}

void *vector_find(const Vector *vector, vector_iterator iterator, void *context)
{
    if(NULL == vector || NULL == iterator)
    {
        errno = EINVAL;
        return NULL;
    }

    for(rsize_t i = 0; i < vector->length; i++)
    {
        if(iterator(vector->items[i], context))
        {
            return vector->items[i];
        }
    }
    return NULL;
}

bool vector_contains(const Vector *vector, vector_comparitor comparitor, void *item)
{
    if(NULL == vector || NULL == comparitor || NULL == item)
    {
        errno = EINVAL;
        return false;
    }

    for(rsize_t i = 0; i < vector->length; i++)
    {
        if(comparitor(vector->items[i], item))
        {
            return true;
        }
    }
    return false;
}

bool vector_any(const Vector *vector, vector_iterator iterator, void *context)
{
    if(NULL == vector || NULL == iterator)
    {
        errno = EINVAL;
        return false;
    }

    for(rsize_t i = 0; i < vector->length; i++)
    {
        if(iterator(vector->items[i], context))
        {
            return true;
        }
    }
    return false;
}

bool vector_all(const Vector *vector, vector_iterator iterator, void *context)
{
    if(NULL == vector || NULL == iterator)
    {
        errno = EINVAL;
        return false;
    }

    for(rsize_t i = 0; i < vector->length; i++)
    {
        if(!iterator(vector->items[i], context))
        {
            return false;
        }
    }
    return true;
}

bool vector_none(const Vector *vector, vector_iterator iterator, void *context)
{
    if(NULL == vector || NULL == iterator)
    {
        errno = EINVAL;
        return false;
    }

    for(rsize_t i = 0; i < vector->length; i++)
    {
        if(iterator(vector->items[i], context))
        {
            return false;
        }
    }
    return true;
}

rsize_t vector_count(const Vector *vector, vector_iterator iterator, void *context)
{
    if(NULL == vector || NULL == iterator)
    {
        errno = EINVAL;
        return 0;
    }

    rsize_t count = 0;
    for(rsize_t i = 0; i < vector->length; i++)
    {
        if(iterator(vector->items[i], context))
        {
            count++;
        }
    }
    return count;
}

bool vector_equals(const Vector *one, const Vector *two, vector_comparitor comparitor)
{
    if(NULL == one || NULL == two || NULL == comparitor)
    {
        errno = EINVAL;
        return false;
    }

    if(one == two)
    {
        return true;
    }
    if(0 == one->length && 0 == two->length)
    {
        return true;
    }
    if(one->length != two->length)
    {
        return false;
    }
    for(rsize_t i = 0; i < vector_length(one); i++)
    {
        if(!comparitor(one->items[i], two->items[i]))
        {
            return false;
        }
    }
    return true;
}

bool vector_iterate(const Vector *vector, vector_iterator iterator, void *context)
{
    if(NULL == vector || NULL == iterator)
    {
        errno = EINVAL;
        return false;
    }

    for(rsize_t i = 0; i < vector->length; i++)
    {
        if(!iterator(vector->items[i], context))
        {
            return false;
        }
    }
    return true;
}

Vector *vector_map(const Vector *vector, vector_mapper function, void *context)
{
    if(NULL == vector || NULL == function)
    {
        errno = EINVAL;
        return false;
    }

    if(0 == vector->length)
    {
        return make_vector_with_capacity(1);
    }

    Vector *target = make_vector_with_capacity(vector->length);
    if(NULL == target)
    {
        return NULL;
    }
    Vector *result = vector_map_into(vector, function, context, target);
    if(NULL == result)
    {
        vector_free(target);
        target = NULL;
        return NULL;
    }
    return target;
}

Vector *vector_map_into(const Vector *vector, vector_mapper function, void *context, Vector *target)
{
    if(NULL == vector || NULL == function || NULL == target)
    {
        errno = EINVAL;
        return false;
    }

    for(rsize_t i = 0; i < vector->length; i++)
    {
        if(!function(vector->items[i], context, target))
        {
            return NULL;
        }
    }

    return target;
}

void *vector_reduce(const Vector *vector, vector_reducer function, void *context)
{
    if(NULL == vector || NULL == function || 0 == vector->length)
    {
        errno = EINVAL;
        return NULL;
    }

    if(1 == vector->length)
    {
        return vector->items[0];
    }

    void *result = function(vector->items[0], vector->items[1], context);
    for(rsize_t i = 2; i < vector->length; i++)
    {
        result = function(result, vector->items[i], context);
    }

    return result;
}

Vector *vector_filter(const Vector *vector, vector_iterator function, void *context)
{
    if(NULL == vector || NULL == function)
    {
        errno = EINVAL;
        return NULL;
    }

    if(0 == vector->length)
    {
        return make_vector_with_capacity(1);
    }

    Vector *result = make_vector_with_capacity(vector->length);
    if(NULL == result)
    {
        return NULL;
    }
    for(rsize_t i = 0; i < vector->length; i++)
    {
        if(function(vector->items[i], context))
        {
            result->items[result->length++] = vector->items[i];
        }
    }

    return result;
}

Vector *vector_filter_not(const Vector *vector, vector_iterator function, void *context)
{
    if(NULL == vector || NULL == function)
    {
        errno = EINVAL;
        return NULL;
    }

    if(0 == vector->length)
    {
        return make_vector_with_capacity(1);
    }

    Vector *result = make_vector_with_capacity(vector->length);
    if(NULL == result)
    {
        return NULL;
    }
    for(rsize_t i = 0; i < vector->length; i++)
    {
        if(!function(vector->items[i], context))
        {
            result->items[result->length++] = vector->items[i];
        }
    }

    return result;
}

static inline rsize_t calculate_new_capacity(rsize_t capacity)
{
    return (capacity * 3) / 2 + 1;
}

static inline bool ensure_capacity(Vector *vector, rsize_t min_capacity)
{
    if(vector->capacity < min_capacity)
    {
        rsize_t new_capacity = calculate_new_capacity(min_capacity);
        return reallocate(vector, new_capacity);
    }
    return true;
}

static inline bool reallocate(Vector *vector, rsize_t capacity)
{
    uint8_t **cache = vector->items;
    vector->items = realloc(vector->items, sizeof(uint8_t *) * capacity);
    if(NULL == vector->items)
    {
        vector->items = cache;
        return false;
    }
    vector->capacity = capacity;
    return true;
}
