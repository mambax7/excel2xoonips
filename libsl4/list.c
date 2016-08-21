/*
 * Copyright (c) 2003-2007 RIKEN Japan, All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY RIKEN AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL RIKEN OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/* $Id: list.c,v 1.1.1.1 2007/10/31 04:26:32 orrisroot Exp $ */

#define LIBSL4_EXPORTS

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef WIN32
# define WIN32_LEAN_AND_MEAN
# ifdef __MINGW32__
#  define WINVER 0x0500         /* WindowsME or Windows2000 */
#  define _WIN32_IE 0x0501      /* IE55 */
# endif
# include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libsl4.h"

LIBSL4_API sl4_list_t *sl4_list_new(  )
{
  sl4_list_t *lst;
  lst = ( sl4_list_t * ) malloc( sizeof( sl4_list_t ) );
  if ( lst == NULL )
    return NULL;
  lst->top = NULL;
  lst->tail = NULL;
  lst->size = 0;
  return lst;
}

LIBSL4_API void sl4_list_delete( sl4_list_t *lst )
{
  while ( lst->size != 0 )
    sl4_list_pop_front( lst );
  free( lst );
}

LIBSL4_API int sl4_list_push_front( sl4_list_t *lst, void *dat )
{
  sl4_list_node_t *node;
  node = ( sl4_list_node_t * ) malloc( sizeof( sl4_list_node_t ) );
  if ( node == NULL )
    return -1;
  node->data = dat;
  node->prev = NULL;
  if ( lst->size == 0 ) {
    node->next = NULL;
    lst->top = node;
    lst->tail = node;
  } else {
    lst->top->prev = node;
    node->next = lst->top;
    lst->top = node;
  }
  lst->size++;
  return 0;
}

LIBSL4_API int sl4_list_push_back( sl4_list_t *lst, void *dat )
{
  sl4_list_node_t *node;
  node = ( sl4_list_node_t * ) malloc( sizeof( sl4_list_node_t ) );
  if ( node == NULL )
    return -1;
  node->data = dat;
  node->next = NULL;
  if ( lst->size == 0 ) {
    node->prev = NULL;
    lst->top = node;
    lst->tail = node;
  } else {
    lst->tail->next = node;
    node->prev = lst->tail;
    lst->tail = node;
  }
  lst->size++;
  return 0;
}

LIBSL4_API int sl4_list_pop_front( sl4_list_t *lst )
{
  sl4_list_node_t *tmp;
  if ( lst->size == 0 )
    return -1;
  lst->size--;
  tmp = lst->top;
  lst->top = tmp->next;
  if ( lst->size == 0 ) {
    lst->tail = NULL;
  } else {
    lst->top->prev = NULL;
  }
  free( tmp );
  return 0;
}

LIBSL4_API int sl4_list_pop_back( sl4_list_t *lst )
{
  sl4_list_node_t *tmp;
  if ( lst->size == 0 )
    return -1;
  lst->size--;
  tmp = lst->tail;
  lst->tail = tmp->prev;
  if ( lst->size == 0 ) {
    lst->top = NULL;
  } else {
    lst->tail->next = NULL;
  }
  free( tmp );
  return 0;
}

LIBSL4_API int sl4_list_erase( sl4_list_t *lst, sl4_list_iterator_t *it )
{
  static const char *errmes = "invalid iterator";
  if ( lst != it->lst ) {
    sl4_error_fatal( errmes, __FILE__, __LINE__ );
  }
  if ( it->status != SL4_LIST_ITERATOR_STAT_FINE ) {
    return -1;
  }
  if ( it->cur->prev == NULL ) {
    /* top of sl4_list */
    sl4_list_pop_front( lst );
  } else if ( it->cur->next == NULL ) {
    /* tail of sl4_list */
    sl4_list_pop_back( lst );
  } else {
    /* middle of sl4_list */
    it->cur->prev->next = it->cur->next;
    it->cur->next->prev = it->cur->prev;
    free( it->cur );
    lst->size--;
  }
  it->cur = NULL;
  it->status = SL4_LIST_ITERATOR_STAT_ILL;
  return 0;
}

LIBSL4_API int sl4_list_begin( sl4_list_t *lst, sl4_list_iterator_t *it )
{
  it->lst = lst;
  if ( lst->size == 0 ) {
    it->cur = NULL;
    it->status = SL4_LIST_ITERATOR_STAT_ILL;
    return -1;
  }
  it->cur = lst->top;
  it->status = SL4_LIST_ITERATOR_STAT_FINE;
  return 0;
}

LIBSL4_API int sl4_list_end( sl4_list_t *lst, sl4_list_iterator_t *it )
{
  it->lst = lst;
  it->cur = NULL;
  if ( lst->size == 0 ) {
    it->status = SL4_LIST_ITERATOR_STAT_ILL;
    return -1;
  }
  it->status = SL4_LIST_ITERATOR_STAT_END;
  return 0;
}

LIBSL4_API int sl4_list_find( sl4_list_t *lst, void *dat,
                              sl4_list_iterator_t *it,
                              sl4_list_eq_func_t func )
{
  sl4_list_node_t *node;
  for ( node = lst->top; node != NULL; node = node->next ) {
    if ( func != NULL ) {
      if ( func( dat, node->data ) != 0 )
        break;
    } else {
      if ( dat == node->data )
        break;
    }
  }
  it->lst = lst;
  it->cur = node;
  if ( node == NULL ) {
    it->status = SL4_LIST_ITERATOR_STAT_ILL;
    return -1;
  }
  it->status = SL4_LIST_ITERATOR_STAT_FINE;
  return 0;
}

LIBSL4_API int sl4_list_rfind( sl4_list_t *lst, void *dat,
                               sl4_list_iterator_t *it,
                               sl4_list_eq_func_t func )
{
  sl4_list_node_t *node;
  for ( node = lst->tail; node != NULL; node = node->prev ) {
    if ( func != NULL ) {
      if ( func( dat, node->data ) != 0 )
        break;
    } else {
      if ( dat == node->data )
        break;
    }
  }
  it->lst = lst;
  it->cur = node;
  if ( node == NULL ) {
    it->status = SL4_LIST_ITERATOR_STAT_ILL;
    return -1;
  }
  it->status = SL4_LIST_ITERATOR_STAT_FINE;
  return 0;
}

LIBSL4_API void sl4_list_it_next( sl4_list_iterator_t *it )
{
  static const char *errmes = "invalid memory access";
  switch ( it->status ) {
  case SL4_LIST_ITERATOR_STAT_FINE:
    it->cur = it->cur->next;
    if ( it->cur == NULL )
      it->status = SL4_LIST_ITERATOR_STAT_END;
    break;
  case SL4_LIST_ITERATOR_STAT_END:
    it->status = SL4_LIST_ITERATOR_STAT_ILL;
  case SL4_LIST_ITERATOR_STAT_ILL:
    sl4_error_fatal( errmes, __FILE__, __LINE__ );
    break;
  }
}

LIBSL4_API void sl4_list_it_prev( sl4_list_iterator_t *it )
{
  static const char *errmes = "invalid memory access";
  switch ( it->status ) {
  case SL4_LIST_ITERATOR_STAT_FINE:
    it->cur = it->cur->prev;
    if ( it->cur == NULL )
      it->status = SL4_LIST_ITERATOR_STAT_ILL;
    break;
  case SL4_LIST_ITERATOR_STAT_END:
    it->cur = it->lst->top;
    it->status = SL4_LIST_ITERATOR_STAT_FINE;
    break;
  case SL4_LIST_ITERATOR_STAT_ILL:
    sl4_error_fatal( errmes, __FILE__, __LINE__ );
    break;
  }
}

LIBSL4_API void *sl4_list_it_data( sl4_list_iterator_t *it )
{
  static const char *errmes = "invalid memory access";
  if ( it->status != SL4_LIST_ITERATOR_STAT_FINE ) {
    sl4_error_fatal( errmes, __FILE__, __LINE__ );
  }
  return it->cur->data;
}
