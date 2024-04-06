// bzlib2.h: interface for libbzip2.
//
//////////////////////////////////////////////////////////////////////

#ifndef BZLIB2_H__INCLUDED_
#define BZLIB2_H__INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

//////////////////////////////////////////////////////////////////////
// 

#define BZ_RUN               0
#define BZ_FLUSH             1
#define BZ_FINISH            2

#define BZ_OK                0
#define BZ_RUN_OK            1
#define BZ_FLUSH_OK          2
#define BZ_FINISH_OK         3
#define BZ_STREAM_END        4
#define BZ_SEQUENCE_ERROR    (-1)
#define BZ_PARAM_ERROR       (-2)
#define BZ_MEM_ERROR         (-3)
#define BZ_DATA_ERROR        (-4)
#define BZ_DATA_ERROR_MAGIC  (-5)
#define BZ_IO_ERROR          (-6)
#define BZ_UNEXPECTED_EOF    (-7)
#define BZ_OUTBUFF_FULL      (-8)
#define BZ_CONFIG_ERROR      (-9)

//////////////////////////////////////////////////////////////////////
//

typedef struct {
	char *next_in;
	unsigned int avail_in;
	unsigned int total_in_lo32;
	unsigned int total_in_hi32;

	char *next_out;
	unsigned int avail_out;
	unsigned int total_out_lo32;
	unsigned int total_out_hi32;

	void *state;

	void* (__cdecl *bzalloc)(void *, int, int);
	void  (__cdecl *bzfree)(void*, void*);
	void *opaque;
} 
bz_stream;

//////////////////////////////////////////////////////////////////////
// Core (low-level) library functions

typedef int (__stdcall *fBZ2_bzCompressInit)(bz_stream* strm, int blockSize100k, int nVerbosity, int workFactor);
typedef int (__stdcall *fBZ2_bzCompress)(bz_stream* strm, int action);
typedef int (__stdcall *fBZ2_bzCompressEnd)(bz_stream* strm);

typedef int (__stdcall *fBZ2_bzDecompressInit)(bz_stream *strm, int nSerbosity, int nSmall);
typedef int (__stdcall *fBZ2_bzDecompress)(bz_stream* strm);
typedef int (__stdcall *fBZ2_bzDecompressEnd)(bz_stream *strm);

//////////////////////////////////////////////////////////////////////
// Utility functions

typedef int (__stdcall *fBZ2_bzBuffToBuffCompress)(
	char*         dest,
	unsigned int* destLen,
	char*         source,
	unsigned int  sourceLen,
	int           blockSize100k,
	int           nVerbosity,
	int           workFactor);

typedef int (__stdcall *fBZ2_bzBuffToBuffDecompress)(
	char*         dest,
	unsigned int* destLen,
	char*         source,
	unsigned int  sourceLen,
	int           nSmall,
	int           nVerbosity);

//////////////////////////////////////////////////////////////////////
// Compatibility functions

typedef const char* (__stdcall *fBZ2_bzlibVersion)(void);

#endif // BZLIB2_H__INCLUDED_
