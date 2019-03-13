// Tencent is pleased to support the open source community by making Mars available.
// Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

// Licensed under the MIT License (the "License"); you may not use this file except in 
// compliance with the License. You may obtain a copy of the License at
// http://opensource.org/licenses/MIT

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and
// limitations under the License.


/*
 * assert.h
 *
 *  Created on: 2012-8-6
 *      Author: yerungui
 */

#ifndef TXG_ASSERT_H
#define TXG_ASSERT_H

#ifdef NO_ASSERT

#define ASSERT(e)
#define ASSERT2(e, fmt, ...) 
#define ASSERTV2(e, fmt, valist)

#else

#include <sys/cdefs.h>
#include <stdarg.h>

# if (!__ISO_C_VISIBLE >= 1999)
#error "C Version < C99"
# endif

#  define    ASSERT(e)                 ((e) ? (void)0 : txf_assert(__FILE__, __LINE__, __func__, #e))
#  define    ASSERT2(e, fmt, ...)     ((e) ? (void)0 : txf_assert2(__FILE__, __LINE__, __func__, #e, fmt, ##__VA_ARGS__))
#  define    ASSERTV2(e, fmt, valist) ((e) ? (void)0 : txf_assertv2(__FILE__, __LINE__, __func__, #e, fmt, valist))

__BEGIN_DECLS
void ENABLE_ASSERT();
void DISABLE_ASSERT();
int IS_ASSERT_ENABLE();

__attribute__((__nonnull__(1, 3, 4)))
void txf_assert(const char *, int, const char *, const char *);
__attribute__((__nonnull__(1, 3, 4, 5))) __attribute__((__format__(printf, 5, 6)))
void txf_assert2(const char *, int, const char *, const char *, const char *, ...);
__attribute__((__nonnull__(1, 3, 4, 5))) __attribute__((__format__(printf, 5, 0)))
void txf_assertv2(const char *, int, const char *, const char *, const char *, va_list);
__END_DECLS

#endif

#endif /* TXG_ASSERT_H */
