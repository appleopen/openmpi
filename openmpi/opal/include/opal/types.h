/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef OPAL_TYPES_H
#define OPAL_TYPES_H

#include "opal_config.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#if OMPI_ENABLE_DEBUG
#include "opal/util/output.h"
#endif

#ifndef __WINDOWS__
/*
 *  Increase FD_SETSIZE
 */

#ifndef OMPI_FD_SETSIZE
#define OMPI_FD_SETSIZE          4096
#endif

#if !defined(NFDBITS) && defined(__NFDBITS)
/* Linux doesn't expose NFDBITS if -ansi unless there is another #define
 * so use the internal version
 */
#define NFDBITS __NFDBITS
#endif

struct ompi_fd_set_t {
    int i;
    uint32_t fds_bits[OMPI_FD_SETSIZE / NFDBITS];
};
typedef struct ompi_fd_set_t ompi_fd_set_t;

#define OMPI_FD_ZERO(fds)     FD_ZERO((fd_set*)(fds))
#define OMPI_FD_SET(fd,fds)   FD_SET((fd),(fd_set*)(fds))
#define OMPI_FD_CLR(fd,fds)   FD_CLR((fd),(fd_set*)(fds))
#define OMPI_FD_ISSET(fd,fds) FD_ISSET((fd),(fd_set*)(fds))

#else /* if we are on windows */
#include <Winsock2.h>    
typedef fd_set ompi_fd_set_t;
#define OMPI_FD_ZERO(fds)     FD_ZERO((fds))
#define OMPI_FD_SET(fd,fds)   FD_SET((fd),(fds))
#define OMPI_FD_CLR(fd,fds)   FD_CLR((fd),(fds))
#define OMPI_FD_ISSET(fd,fds) FD_ISSET((fd),(fds))

#endif /* wIN32 */
    

/*
 * portable assignment of pointer to int
 */

typedef union {
   uint64_t lval;
   uint32_t ival;
   void*    pval;
   struct {
       uint32_t uval;
       uint32_t lval;
   } sval;
} ompi_ptr_t;

/*
 * handle differences in iovec
 */

#if defined(__APPLE__) || defined(__WINDOWS__)
typedef char* ompi_iov_base_ptr_t;
#else
typedef void* ompi_iov_base_ptr_t;
#endif

/*
 * handle differences in socklen_t
 */

#if defined(HAVE_SOCKLEN_T)
typedef socklen_t opal_socklen_t;
#else
typedef int opal_socklen_t;
#endif


/*
 * Convert a 64 bit value to network byte order.
 */
static inline uint64_t hton64(uint64_t val)
{
    union { uint64_t ll;
            uint32_t l[2];
    } w, r;

    /* platform already in network byte order? */
    if(htonl(1) == 1L)
        return val;
    w.ll = val;
    r.l[0] = htonl(w.l[1]);
    r.l[1] = htonl(w.l[0]);
    return r.ll;
}

/*
 * Convert a 64 bit value from network to host byte order.
 */

static inline uint64_t ntoh64(uint64_t val)
{
    union { uint64_t ll;
            uint32_t l[2];
    } w, r;

    /* platform already in network byte order? */
    if(htonl(1) == 1L)
        return val;
    w.ll = val;
    r.l[0] = ntohl(w.l[1]);
    r.l[1] = ntohl(w.l[0]);
    return r.ll;
}


/**
 * Convert between a local representation of pointer and a 64 bits value.
 */
static inline uint64_t ompi_ptr_ptol( void* ptr )
{
    return (uint64_t)(uintptr_t) ptr;
}

static inline void* ompi_ptr_ltop( uint64_t value )
{
#if SIZEOF_VOID_P == 4 && OMPI_ENABLE_DEBUG
    if (value > ((1ULL << 32) - 1ULL)) {
        opal_output(0, "Warning: truncating value in ompi_ptr_ltop");
    }
#endif
    return (void*)(uintptr_t) value;
}

#ifdef WORDS_BIGENDIAN
static inline uint16_t opal_swap_bytes2(uint16_t val) 
{
    union { uint16_t bigval;
            uint8_t  arrayval[2];
    } w, r;

    w.bigval = val;
    r.arrayval[0] = w.arrayval[1];
    r.arrayval[1] = w.arrayval[0];

    return r.bigval;
}

static inline uint32_t opal_swap_bytes4(uint32_t val) 
{
    union { uint32_t bigval;
            uint8_t  arrayval[4];
    } w, r;

    w.bigval = val;
    r.arrayval[0] = w.arrayval[3];
    r.arrayval[1] = w.arrayval[2];
    r.arrayval[2] = w.arrayval[1];
    r.arrayval[3] = w.arrayval[0];

    return r.bigval;
}

static inline uint64_t opal_swap_bytes8(uint64_t val)
{
    union { uint64_t bigval;
            uint8_t  arrayval[8];
    } w, r;

    w.bigval = val;
    r.arrayval[0] = w.arrayval[7];
    r.arrayval[1] = w.arrayval[6];
    r.arrayval[2] = w.arrayval[5];
    r.arrayval[3] = w.arrayval[4];
    r.arrayval[4] = w.arrayval[3];
    r.arrayval[5] = w.arrayval[2];
    r.arrayval[6] = w.arrayval[1];
    r.arrayval[7] = w.arrayval[0];
    
    return r.bigval;
}

#else
#define opal_swap_bytes2 htons
#define opal_swap_bytes4 htonl
#define opal_swap_bytes8 hton64
#endif

#endif