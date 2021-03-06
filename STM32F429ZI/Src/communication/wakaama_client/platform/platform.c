/* Copyright 2018 Maksymilian Wojczuk and Tomasz Michalec
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * NOTE: Comments over functions implementations are copied from liblw2m.h
 *       and should be on par with this header but if not, then liblw2m.h
 *       is right.
 */
#include <liblwm2m.h>
#include "cmsis_os.h"
#include <string.h>
#include "stm32f4xx_hal.h"
#include "mem.h"
#include <stdarg.h>

// Allocate a block of size bytes of memory, returning a pointer to the beginning of the block.
#ifdef LWM2M_MEMORY_TRACE
void * lwm2m_trace_malloc(size_t s, const char * file, const char * function, int lineno) {
  printf("lwm2m_malloc: \"%s\" : \"%s\" : %d \n", file, function, lineno);
  return pvPortMalloc(s);
}
#else 
void * lwm2m_malloc(size_t s){
  return pvPortMalloc(s);
}
#endif

// Deallocate a block of memory previously allocated by lwm2m_malloc() or lwm2m_strdup()
#ifdef LWM2M_MEMORY_TRACE
void lwm2m_trace_free(void * p, const char * file, const char * function, int lineno) {
  printf("lwm2m_free: \"%s\" : \"%s\" : %d \n", file, function, lineno);
  vPortFree(p);
}
#else 
void lwm2m_free(void * p) {
  vPortFree(p);
}
#endif

// Allocate a memory block, duplicate the string str in it and return a pointer to this new block.
#ifdef LWM2M_MEMORY_TRACE
char * lwm2m_trace_strdup(const char * str, const char * file, const char * function, int lineno) {
  printf("lwm2m_strdup: \"%s\" : \"%s\" : %d \n", file, function, lineno);
  char *dup = pvPortMalloc(strlen(str) + 1);
  return dup ? strcpy(dup, str) : dup;
}
#else
char * lwm2m_strdup(const char * str) {
  char *dup = pvPortMalloc(strlen(str) + 1);
  return dup ? strcpy(dup, str) : dup;
}
#endif

// Compare at most the n first bytes of s1 and s2, return 0 if they match
int lwm2m_strncmp(const char * s1, const char * s2, size_t n) {
  return strncmp(s1, s2, n);
}

// This function must return the number of seconds elapsed since origin.
// The origin (Epoch, system boot, etc...) does not matter as this
// function is used only to determine the elapsed time since the last
// call to it.
// In case of error, this must return a negative value.
// Per POSIX specifications, time_t is a signed integer.
time_t lwm2m_gettime(void) {
  // TODO: Now we use 32bit counter and it will overflow.
  //       Maybe export xNumOfOverflows from tasks.c and do something like that:
  //       return (xNumOfOverflows << 16) | (xTaskGetTickCount >> 16)
  return xTaskGetTickCount() / configTICK_RATE_HZ;
}

#ifdef LWM2M_WITH_LOGS
// Same usage as C89 printf()
void lwm2m_printf(const char * format, ...) {
  va_list arg;
  va_start(arg, format);
  printf(format, arg);
  fflush(stdout);
  va_end(arg);
}
#endif
