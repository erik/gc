#pragma once

#ifndef _DEBUG_H_
#define _DEBUG_H_

/* Some useful features to use in debug builds */

#ifdef DEBUG
#  undef NDEBUG
#  define LOG(s)                                                    \
    printf(">>\t%s:%d:%s >> ", __FILE__, __LINE__, __FUNCTION__);   \
    puts(s)
#  define LOGF(...)                                                 \
    printf(">>\t%s:%d:%s >> ", __FILE__, __LINE__, __FUNCTION__);   \
    printf(__VA_ARGS__)

#else

#  define NDEBUG 1
#  define LOG(...)    (void)0
#  define LOGF(...)   (void)0

#endif /* ifdef DEBUG */

/* for assert() */
#include <assert.h>

#endif /* _DEBUG_H_ */
