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

#include <string.h>

#include "hash.h"

hashcode identity_hash(void *key)
{
    return (hashcode)key;
}

#define LEFT_MAGNITUDE 5
#define RIGHT_MAGNATUDE 2

/**
 * M. V. Ramakrishna and J. Zobel. Performance in practice of string hashing functions. In DASFAA, pages 215–224, 1997.
 * http://citeseer.ist.psu.edu/viewdoc/summary?doi=10.1.1.18.7520
 */
hashcode string_hash(void *key)
{
    char *string = (char *)key;
    size_t length = strlen(string);
    hashcode result = (hashcode)key;
    for(size_t i = 0; i < length; i++)
    {
        result ^= (result << LEFT_MAGNITUDE) + (result >> RIGHT_MAGNATUDE) + (unsigned long)string[i];
    }

    return result;
}
