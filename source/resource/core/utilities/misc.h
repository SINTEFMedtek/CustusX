#ifndef _SW_MISC_H
#define _SW_MISC_H

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/**
 * We can use gcc to check things like variadic macros for correctness by using
 * this attribute hint. This will most likely not work with other compilers.
 */
#if defined(__GNUC__)
#define sw__attribute(x)  __attribute__(x)
#else
#define sw__attribute(x)
#endif

#if ( !defined  __cplusplus ) && ( __STDC_VERSION__ >= 199901L )
#define RESTRICT restrict
#else
#define RESTRICT
#endif

/** strlcpy and strlcat are available in BSD (Mac). Remove these custom versions
 *  for Mac.
 */
#ifdef __linux__

/**
 *	Copy src to string dst of size size.  At most size-1 characters
 *	will be copied.  Always NUL terminates (unless size == 0).
 *	Returns strlen(src); if retval >= size, truncation occurred.
 */
static inline size_t strlcpy( char *RESTRICT dst, const char *RESTRICT src, size_t size )
{
	if (size > 0)
	{
		strncpy(dst, src, size - 1);
		dst[size - 1] = '\0';
	}
	return strlen(src);
}

/**
 *	Appends src to string dst of size siz (unlike strncat, siz is the
 *	full size of dst, not space left).  At most siz-1 characters
 *	will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
 *	Returns strlen(src) + MIN(siz, strlen(initial dst)).
 *	If retval >= siz, truncation occurred.
 */
static inline size_t strlcat( char *RESTRICT dst, const char *RESTRICT src, size_t siz )
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;
	size_t dlen;

	/* Find the end of dst and adjust bytes left but don't go past end */
	while ( n-- != 0 && *d != '\0' )
	{
		d++;
	}
	dlen = d - dst;
	n = siz - dlen;

	if ( n == 0 )
	{
		return ( dlen + strlen( s ) );
	}
	while ( *s != '\0' )
	{
		if ( n != 1 )
		{
			*d++ = *s;
			n--;
		}
		s++;
	}
	*d = '\0';

	return ( dlen + ( s - src ) );        /* count does not include NUL */
}

#endif // __linux__

/* Safe MIN and MAX macros that only evaluate their expressions once. */
#undef MAX
#define MAX(a, b) \
	({ typeof (a) _a = (a); \
	   typeof (b) _b = (b); \
	 _a > _b ? _a : _b; })

#undef MIN
#define MIN(a, b) \
	({ typeof (a) _a = (a); \
	   typeof (b) _b = (b); \
	 _a < _b ? _a : _b; })

/** Asserts that two types are equal */
#define ASSERT_STATIC_STRING(_var) assert(__builtin_types_compatible_p(typeof(_var), char[]))

/* Static array versions of common string functions. Safer because one less parameter to screw up. 
 * Can only be used on strings longer than the length of a pointer, because we use this for debugging. */
#if !defined( DEBUG ) || defined ( __cplusplus )
#define sstrcpy(dest, src) strlcpy((dest), (src), sizeof(dest))
#define sstrcat(dest, src) strlcat((dest), (src), sizeof(dest))
#define ssprintf(dest, ...) snprintf((dest), sizeof(dest), __VA_ARGS__)
#define sstrcmp(str1, str2) strncmp((str1), (str2), sizeof(str1) > sizeof(str2) ? sizeof(str2) : sizeof(str1))
#else
#define sstrcpy(dest, src) (ASSERT_STATIC_STRING(dest), strlcpy((dest), (src), sizeof(dest)))
#define sstrcat(dest, src) (ASSERT_STATIC_STRING(dest), strlcat((dest), (src), sizeof(dest)))
#define ssprintf(dest, ...) (ASSERT_STATIC_STRING(dest), snprintf((dest), sizeof(dest), __VA_ARGS__))
#define sstrcmp(str1, str2) (ASSERT_STATIC_STRING(str1), ASSERT_STATIC_STRING(str2), strncmp((str1), (str2), sizeof(str1) > sizeof(str2) ? sizeof(str2) : sizeof(str1)))
#endif

#endif
