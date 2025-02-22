/* Copyright (c) 2002, Alexander Popov (sasho@vip.bg)
   Copyright (c) 2002,2004,2005 Joerg Wunsch
   Copyright (c) 2005, Helmut Wallner
   Copyright (c) 2007, Dmitry Xmelkov
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

/* From: Id: printf_p_new.c,v 1.1.1.9 2002/10/15 20:10:28 joerg_wunsch Exp */
/* $Id: vfprintf.c 2191 2010-11-05 13:45:57Z arcanum $ */

#define _DEFAULT_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "stdio_private.h"
#include "../../libm/common/math_config.h"

#ifdef PICOLIBC_FLOAT_PRINTF_SCANF
static inline float
printf_float_get(uint32_t u)
{
    return asfloat(u);
}

#define PRINTF_FLOAT_ARG(ap) (printf_float_get(va_arg(ap, uint32_t)))
typedef float printf_float_t;
typedef int32_t printf_float_int_t;
#define ASUINT(a) asuint(a)
#define EXP_BIAS 127

#define dtoa ftoa
#define DTOA_MINUS FTOA_MINUS
#define DTOA_ZERO  FTOA_ZERO
#define DTOA_INF   FTOA_INF
#define DTOA_NAN   FTOA_NAN
#define DTOA_CARRY FTOA_CARRY
#define DTOA_MAX_DIG FTOA_MAX_DIG
#define __dtoa_engine(x,dtoa,dig,dec) __ftoa_engine(x,dtoa,dig,dec)
#include "ftoa_engine.h"

#else

#define PRINTF_FLOAT_ARG(ap) va_arg(ap, double)
typedef double printf_float_t;
typedef int64_t printf_float_int_t;
#define ASUINT(a) asuint64(a)
#define EXP_BIAS 1023
#include "dtoa_engine.h"

#endif

/*
 * This file can be compiled into more than one flavour.  The default
 * is to offer the usual modifiers and integer formatting support
 * (level 1).  Level 2 includes floating point support.
 */

#ifndef PRINTF_LEVEL
#  define PRINTF_LEVEL PRINTF_FLT
#  ifndef FORMAT_DEFAULT_DOUBLE
#    define vfprintf __d_vfprintf
#  endif
#endif

#if PRINTF_LEVEL == PRINTF_STD || PRINTF_LEVEL == PRINTF_FLT

#if ((PRINTF_LEVEL >= PRINTF_FLT) || defined(_WANT_IO_LONG_LONG))
#define PRINTF_LONGLONG
#endif

#ifdef PRINTF_LONGLONG
typedef unsigned long long ultoa_unsigned_t;
typedef long long ultoa_signed_t;
#define SIZEOF_ULTOA __SIZEOF_LONG_LONG__
#define arg_to_t(flags, _s_, _result_)                  \
    if ((flags) & FL_LONG) {                            \
        if ((flags) & FL_REPD_TYPE)                     \
            (_result_) = va_arg(ap, _s_ long long);     \
        else                                            \
            (_result_) = va_arg(ap, _s_ long);          \
    } else {                                            \
        (_result_) = va_arg(ap, _s_ int);               \
        if ((flags) & FL_SHORT) {                       \
            if ((flags) & FL_REPD_TYPE)                 \
                (_result_) = (_s_ char) (_result_);     \
            else                                        \
                (_result_) = (_s_ short) (_result_);    \
        }                                               \
    }
#else
typedef unsigned long ultoa_unsigned_t;
typedef long ultoa_signed_t;
#define SIZEOF_ULTOA __SIZEOF_LONG__
#define arg_to_t(flags, _s_, _result_)                  \
    if ((flags) & FL_LONG) {                            \
        (_result_) = va_arg(ap, _s_ long);              \
    } else {                                            \
        (_result_) = va_arg(ap, _s_ int);               \
        if ((flags) & FL_SHORT) {                       \
            if ((flags) & FL_REPD_TYPE)                 \
                (_result_) = (_s_ char) (_result_);     \
            else                                        \
                (_result_) = (_s_ short) (_result_);    \
        }                                               \
    }
#endif

#if SIZEOF_ULTOA <= 4
#define PRINTF_BUF_SIZE 11
#else
#define PRINTF_BUF_SIZE 22
#endif

// At the call site the address of the result_var is taken (e.g. "&ap")
// That way, it's clear that these macros *will* modify that variable
#define arg_to_unsigned(flags, result_var) arg_to_t((flags), unsigned, (result_var))
#define arg_to_signed(flags, result_var) arg_to_t((flags), signed, (result_var))

#include "ultoa_invert.c"

/* Order is relevant here and matches order in format string */

#define FL_ZFILL	0x0001
#define FL_PLUS		0x0002
#define FL_SPACE	0x0004
#define FL_LPAD		0x0008
#define FL_ALT		0x0010

#define FL_WIDTH	0x0020
#define FL_PREC		0x0040

#define FL_LONG		0x0080
#define FL_SHORT	0x0100
#define FL_REPD_TYPE	0x0200

#define FL_NEGATIVE	0x0400

#define FL_ALTUPP	0x0800
#define FL_ALTHEX	0x1000

#ifdef _WANT_IO_C99_FORMATS
#define FL_FLTHEX       0x2000
#endif
#define FL_FLTEXP	0x4000
#define	FL_FLTFIX	0x8000

int vfprintf (FILE * stream, const char *fmt, va_list ap)
{
    unsigned char c;		/* holds a char from the format string */
    uint16_t flags;
    int width;
    int prec;
    union {
	char __buf[PRINTF_BUF_SIZE];	/* size for -1 in octal, without '\0'	*/
#if PRINTF_LEVEL >= PRINTF_FLT
	struct dtoa __dtoa;
#endif
    } u;
    const char * pnt;
    size_t size;

#define buf	(u.__buf)
#define _dtoa	(u.__dtoa)

    int stream_len = 0;

#define my_putc(c, stream) do { ++stream_len; putc(c, stream); } while(0)

    if ((stream->flags & __SWR) == 0)
	return EOF;

    for (;;) {

	for (;;) {
	    c = *fmt++;
	    if (!c) goto ret;
	    if (c == '%') {
		c = *fmt++;
		if (c != '%') break;
	    }
	    my_putc (c, stream);
	}

	flags = 0;
	width = 0;
	prec = 0;

	do {
	    if (flags < FL_WIDTH) {
		switch (c) {
		  case '0':
		    flags |= FL_ZFILL;
		    continue;
		  case '+':
		    flags |= FL_PLUS;
		    FALLTHROUGH;
		  case ' ':
		    flags |= FL_SPACE;
		    continue;
		  case '-':
		    flags |= FL_LPAD;
		    continue;
		  case '#':
		    flags |= FL_ALT;
		    continue;
		}
	    }

	    if (flags < FL_LONG) {
		if (c >= '0' && c <= '9') {
		    c -= '0';
		    if (flags & FL_PREC) {
			prec = 10*prec + c;
			continue;
		    }
		    width = 10*width + c;
		    flags |= FL_WIDTH;
		    continue;
		}
		if (c == '*') {
		    if (flags & FL_PREC) {
			prec = va_arg(ap, int);
		    } else {
			width = va_arg(ap, int);
			flags |= FL_WIDTH;
			if (width < 0) {
			    width = -width;
			    flags |= FL_LPAD;
			}
		    }
		    continue;
		}
		if (c == '.') {
		    if (flags & FL_PREC)
			goto ret;
		    flags |= FL_PREC;
		    continue;
		}
	    }

	    if (c == 'l') {
#ifdef PRINTF_LONGLONG
		if (flags & FL_LONG)
		    flags |= FL_REPD_TYPE;
#endif
		flags |= FL_LONG;
		continue;
	    }

	    if (c == 'h') {
		if (flags & FL_SHORT)
		    flags |= FL_REPD_TYPE;
		flags |= FL_SHORT;
		continue;
	    }

#ifdef _WANT_IO_C99_FORMATS

#ifdef PRINTF_LONGLONG
#define CHECK_LONGLONG(type) else if (sizeof(type) == sizeof(long long)) flags |= FL_LONG|FL_REPD_TYPE;
#else
#define CHECK_LONGLONG(type)
#endif

#define CHECK_INT_SIZE(letter, type)			\
	    if (c == letter) {				\
		if (sizeof(type) == sizeof(int))	\
		    ;                                   \
		else if (sizeof(type) == sizeof(long))	\
		    flags |= FL_LONG;                   \
                CHECK_LONGLONG(type)                    \
		else if (sizeof(type) == sizeof(short))	\
		    flags |= FL_SHORT;			\
                continue;                               \
	    }

	    CHECK_INT_SIZE('j', intmax_t);
	    CHECK_INT_SIZE('z', size_t);
	    CHECK_INT_SIZE('t', ptrdiff_t);
#endif

	    break;
	} while ( (c = *fmt++) != 0);

	/* This can happen only when prec is set via a '*'
	 * specifier, in which case it works as if no precision
	 * was specified. Set the precision to zero and clear the
	 * flag.
	 */
	if (prec < 0) {
	    prec = 0;
	    flags &= ~FL_PREC;
	}

	/* Only a format character is valid.	*/

#if	'F' != 'E'+1  ||  'G' != 'F'+1  ||  'f' != 'e'+1  ||  'g' != 'f'+1
# error
#endif

#define CASE_CONVERT    ('a' - 'A')
#define TOLOW(c)        ((c) | CASE_CONVERT)
#define TOCASE(c)       ((c) - case_convert)

	if ((TOLOW(c) >= 'e' && TOLOW(c) <= 'g')
#ifdef _WANT_IO_C99_FORMATS
            || TOLOW(c) == 'a'
#endif
            )
        {
#if PRINTF_LEVEL >= PRINTF_FLT
            printf_float_t fval;        /* value to print */
            uint8_t sign;		/* sign character (or 0)	*/
            uint8_t ndigs;		/* number of digits to convert */
            unsigned char case_convert; /* subtract to correct case */
	    int exp;			/* exponent of most significant decimal digit */
	    int n;                      /* total width */
	    uint8_t ndigs_exp;		/* number of digis in exponent */

            fval = PRINTF_FLOAT_ARG(ap);

            /* deal with upper vs lower case */
            case_convert = TOLOW(c) - c;
            c = TOLOW(c);

	    ndigs = 0;

#ifdef _WANT_IO_C99_FORMATS
            if (c == 'a') {
                printf_float_int_t fi;
                ultoa_signed_t      s;

                c = 'p';
                flags |= FL_FLTEXP | FL_FLTHEX;

#ifdef PICOLIBC_FLOAT_PRINTF_SCANF
                ndigs = 7;
#else
                ndigs = 14;
#endif
                fi = ASUINT(fval);

                _dtoa.digits[0] = '0';
#ifdef PICOLIBC_FLOAT_PRINTF_SCANF
#define EXP_SHIFT       23
#define EXP_MASK        0xff
#define SIG_SHIFT       1
#define SIG_MASK        0x7fffff
#else
#define EXP_SHIFT       52
#define EXP_MASK        0x7ff
#define SIG_SHIFT       0
#define SIG_MASK        0xfffffffffffffLL
#endif
                exp = ((fi >> EXP_SHIFT) & EXP_MASK);
                s = (fi & SIG_MASK) << SIG_SHIFT;
                if (s | exp) {
                    if (!exp)
                        exp = 1;
                    else
                        _dtoa.digits[0] = '1';
                    exp -= EXP_BIAS;
                }
                _dtoa.flags = 0;
                if (fi < 0)
                    _dtoa.flags = DTOA_MINUS;

                if (!(flags & FL_PREC))
                    prec = 0;
                else if (prec >= (ndigs - 1))
                    prec = ndigs - 1;
                else {
                    int                 bits = ((ndigs - 1) - prec) << 2;
                    ultoa_signed_t      half = ((ultoa_signed_t) 1) << (bits - 1);
                    ultoa_signed_t      mask = ~((half << 1) - 1);

                    s += half;
                    if (s > (SIG_MASK << SIG_SHIFT))
                        _dtoa.digits[0]++;
                    s &= mask;
                }

                if (exp == EXP_BIAS + 1) {
                    if (s)
                        _dtoa.flags |= DTOA_NAN;
                    else
                        _dtoa.flags |= DTOA_INF;
                } else {
                    uint8_t d;
                    for (d = ndigs - 1; d; d--) {
                        char dig = s & 0xf;
                        s >>= 4;
                        if (dig == 0 && d > prec)
                            continue;
                        if (dig <= 9)
                            dig += '0';
                        else
                            dig += TOCASE('a' - 10);
                        _dtoa.digits[d] = dig;
                        if (prec < d)
                            prec = d;
                    }
                }
                ndigs_exp = 1;
            } else
#endif /* _WANT_IO_C99_FORMATS */
            {
                int ndecimal;	        /* digits after decimal (for 'f' format), 0 if no limit */

                if (!(flags & FL_PREC))
                    prec = 6;
                if (c == 'e') {
                    ndigs = prec + 1;
                    ndecimal = 0;
                    flags |= FL_FLTEXP;
                } else if (c == 'f') {
                    ndigs = DTOA_MAX_DIG;
                    ndecimal = prec + 1;
                    flags |= FL_FLTFIX;
                } else {
                    c += 'e' - 'g';
                    ndigs = prec;
                    if (ndigs < 1) ndigs = 1;
                    ndecimal = 0;
                }

                if (ndigs > DTOA_MAX_DIG)
                    ndigs = DTOA_MAX_DIG;

                ndigs = __dtoa_engine (fval, &_dtoa, ndigs, ndecimal);
                exp = _dtoa.exp;
                ndigs_exp = 2;
            }
	    if (exp < -9 || 9 < exp)
		    ndigs_exp = 2;
	    if (exp < -99 || 99 < exp)
		    ndigs_exp = 3;
#ifndef PICOLIBC_FLOAT_PRINTF_SCANF
	    if (exp < -999 || 999 < exp)
		    ndigs_exp = 4;
#endif

	    sign = 0;
	    if (_dtoa.flags & DTOA_MINUS)
		sign = '-';
	    else if (flags & FL_PLUS)
		sign = '+';
	    else if (flags & FL_SPACE)
		sign = ' ';

	    if (_dtoa.flags & (DTOA_NAN | DTOA_INF))
	    {
		ndigs = sign ? 4 : 3;
		if (width > ndigs) {
		    width -= ndigs;
		    if (!(flags & FL_LPAD)) {
			do {
			    my_putc (' ', stream);
			} while (--width);
		    }
		} else {
		    width = 0;
		}
		if (sign)
		    my_putc (sign, stream);
		pnt = "inf";
		if (_dtoa.flags & DTOA_NAN)
		    pnt = "nan";
		while ( (c = *pnt++) )
		    my_putc (TOCASE(c), stream);
	    }
            else
            {

                if (!(flags & (FL_FLTEXP|FL_FLTFIX))) {

                    /* 'g(G)' format */

                    /*
                     * On entry to this block, prec is
                     * the number of digits to display.
                     *
                     * On exit, prec is the number of digits
                     * to display after the decimal point
                     */

                    /* Always show at least one digit */
                    if (prec == 0)
                        prec = 1;

                    /*
                     * Remove trailing zeros. The ryu code can emit them
                     * when rounding to fewer digits than required for
                     * exact output, the imprecise code often emits them
                     */
                    while (ndigs > 0 && _dtoa.digits[ndigs-1] == '0')
                        ndigs--;

                    /* Save requested precision */
                    int req_prec = prec;

                    /* Limit output precision to ndigs unless '#' */
                    if (!(flags & FL_ALT))
                        prec = ndigs;

                    /*
                     * Figure out whether to use 'f' or 'e' format. The spec
                     * says to use 'f' if the exponent is >= -4 and < requested
                     * precision.
                     */
                    if (-4 <= exp && exp < req_prec)
                    {
                        flags |= FL_FLTFIX;

                        /* Compute how many digits to show after the decimal.
                         *
                         * If exp is negative, then we need to show that
                         * many leading zeros plus the requested precision
                         *
                         * If exp is less than prec, then we need to show a
                         * number of digits past the decimal point,
                         * including (potentially) some trailing zeros
                         *
                         * (these two cases end up computing the same value,
                         * and are both caught by the exp < prec test,
                         * so they share the same branch of the 'if')
                         *
                         * If exp is at least 'prec', then we don't show
                         * any digits past the decimal point.
                         */
                        if (exp < prec)
                            prec = prec - (exp + 1);
                        else
                            prec = 0;
                    } else {
                        /* Compute how many digits to show after the decimal */
                        prec = prec - 1;
                    }
                }

                /* Conversion result length, width := free space length	*/
                if (flags & FL_FLTFIX)
                    n = (exp>0 ? exp+1 : 1);
                else {
                    n = 3;                  /* 1e+ */
#ifdef _WANT_IO_C99_FORMATS
                    if (flags & FL_FLTHEX)
                        n += 2;             /* or 0x1p+ */
#endif
                    n += ndigs_exp;		/* add exponent */
                }
                if (sign)
                    n += 1;
                if (prec)
                    n += prec + 1;
                else if (flags & FL_ALT)
                    n += 1;

                width = width > n ? width - n : 0;

                /* Output before first digit	*/
                if (!(flags & (FL_LPAD | FL_ZFILL))) {
                    while (width) {
                        my_putc (' ', stream);
                        width--;
                    }
                }
                if (sign)
                    my_putc (sign, stream);

                if (!(flags & FL_LPAD)) {
                    while (width) {
                        my_putc ('0', stream);
                        width--;
                    }
                }

                if (flags & FL_FLTFIX) {		/* 'f' format		*/
                    char out;

                    /* At this point, we should have
                     *
                     *	exp	exponent of leftmost digit in _dtoa.digits
                     *	ndigs	number of buffer digits to print
                     *	prec	number of digits after decimal
                     *
                     * In the loop, 'n' walks over the exponent value
                     */
                    n = exp > 0 ? exp : 0;		/* exponent of left digit */
                    do {

                        /* Insert decimal point at correct place */
                        if (n == -1)
                            my_putc ('.', stream);

                        /* Pull digits from buffer when in-range,
                         * otherwise use 0
                         */
                        if (0 <= exp - n && exp - n < ndigs)
                            out = _dtoa.digits[exp - n];
                        else
                            out = '0';
                        if (--n < -prec) {
                            break;
                        }
                        my_putc (out, stream);
                    } while (1);
                    if (n == exp
                        && (_dtoa.digits[0] > '5'
                            || (_dtoa.digits[0] == '5' && !(_dtoa.flags & DTOA_CARRY))) )
                    {
                        out = '1';
                    }
                    my_putc (out, stream);
                    if ((flags & FL_ALT) && n == -1)
			my_putc('.', stream);
                } else {				/* 'e(E)' format	*/

#ifdef _WANT_IO_C99_FORMATS
                    if ((flags & FL_FLTHEX)) {
                        my_putc('0', stream);
                        my_putc(TOCASE('x'), stream);
                    }
#endif

                    /* mantissa	*/
                    if (_dtoa.digits[0] != '1')
                        _dtoa.flags &= ~DTOA_CARRY;
                    my_putc (_dtoa.digits[0], stream);
                    if (prec > 0) {
                        my_putc ('.', stream);
                        uint8_t pos = 1;
                        for (pos = 1; pos < 1 + prec; pos++)
                            my_putc (pos < ndigs ? _dtoa.digits[pos] : '0', stream);
                    } else if (flags & FL_ALT)
                        my_putc ('.', stream);

                    /* exponent	*/
                    my_putc (TOCASE(c), stream);
                    sign = '+';
                    if (exp < 0 || (exp == 0 && (_dtoa.flags & DTOA_CARRY) != 0)) {
                        exp = -exp;
                        sign = '-';
                    }
                    my_putc (sign, stream);
#ifndef PICOLIBC_FLOAT_PRINTF_SCANF
                    if (ndigs_exp > 3) {
			my_putc(exp / 1000 + '0', stream);
			exp %= 1000;
                    }
#endif
                    if (ndigs_exp > 2) {
			my_putc(exp / 100 + '0', stream);
			exp %= 100;
                    }
                    if (ndigs_exp > 1) {
                        my_putc(exp / 10 + '0', stream);
                        exp %= 10;
                    }
                    my_putc ('0' + exp, stream);
                }
	    }
#else		/* to: PRINTF_LEVEL >= PRINTF_FLT */
	    (void) PRINTF_FLOAT_ARG(ap);
	    pnt = "*float*";
	    size = sizeof ("*float*") - 1;
	    goto str_lpad;
#endif
        } else {
            int len, buf_len;

            if (c == 'c') {
                buf[0] = va_arg (ap, int);
                pnt = buf;
                size = 1;
                goto str_lpad;
            } else if (c == 's') {
                pnt = va_arg (ap, char *);
                if (!pnt)
                    pnt = "(null)";
                size = strnlen (pnt, (flags & FL_PREC) ? prec : ~0);

            str_lpad:
                if (!(flags & FL_LPAD)) {
                    while ((size_t) width > size) {
                        my_putc (' ', stream);
                        width--;
                    }
                }
                width -= size;
                while (size--) {
                    my_putc (*pnt++, stream);
                }

            } else {
                if (c == 'd' || c == 'i') {
                    ultoa_signed_t x_s;

                    arg_to_signed(flags, x_s);

                    if (x_s < 0) {
                        x_s = -x_s;
                        flags |= FL_NEGATIVE;
                    }

                    flags &= ~FL_ALT;

                    if ((flags & FL_PREC) && prec == 0 && x_s == 0)
                        buf_len = 0;
                    else
                        buf_len = __ultoa_invert (x_s, buf, 10) - buf;
                } else {
                    int base;
                    ultoa_unsigned_t x;

                    if (c == 'u') {
                        flags &= ~FL_ALT;
                        base = 10;
                    } else if (c == 'o') {
                        base = 8;
                    } else if (c == 'p') {
                        flags |= FL_ALT | FL_ALTHEX;
                        base = 16;
                    } else if (TOLOW(c) == 'x') {
                        flags |= FL_ALTHEX;
                        base = 16;
                        if (c == 'X') {
                            base = 16 | XTOA_UPPER;
                            flags |= (FL_ALTHEX | FL_ALTUPP);
                        }
                    } else {
                        my_putc('%', stream);
                        my_putc(c, stream);
                        continue;
                    }

                    arg_to_unsigned(flags, x);

                    flags &= ~(FL_PLUS | FL_SPACE);

                    if ((flags & FL_PREC) && prec == 0 && x == 0) {
                        buf_len = 0;
                        flags &= ~FL_ALT;
                    }
                    else
                        buf_len = __ultoa_invert (x, buf, base) - buf;
                }

                len = buf_len;

                /* Specified precision */
                if (flags & FL_PREC) {

                    /* Zfill ignored when precision specified */
                    flags &= ~FL_ZFILL;

                    /* If the number is shorter than the precision, pad
                     * on the left with zeros */
                    if (len < prec) {
                        len = prec;

                        /* Don't add the leading '0' for alternate octal mode */
                        if (!(flags & FL_ALTHEX))
                            flags &= ~FL_ALT;
                    }
                }

                /* Alternate mode for octal/hex */
                if (flags & FL_ALT) {

                    /* Zero gets no special alternate treatment */
                    if (buf[buf_len-1] == '0') {
                        flags &= ~FL_ALT;
                    } else {
                        len += 1;
                        if (flags & FL_ALTHEX)
                            len += 1;
                    }
                } else if (flags & (FL_NEGATIVE | FL_PLUS | FL_SPACE)) {
                    len += 1;
                }

                /* Pad on the left ? */
                if (!(flags & FL_LPAD)) {

                    /* Pad with zeros, using the same loop as the
                     * precision modifier
                     */
                    if (flags & FL_ZFILL) {
                        prec = buf_len;
                        if (len < width) {
                            prec += width - len;
                            len = width;
                        }
                    }
                    while (len < width) {
                        my_putc (' ', stream);
                        len++;
                    }
                }

                /* Width remaining on right after value */
                width -= len;

                /* Output leading characters */
                if (flags & FL_ALT) {
                    my_putc ('0', stream);
                    if (flags & FL_ALTHEX)
                        my_putc (flags & FL_ALTUPP ? 'X' : 'x', stream);
                } else if (flags & (FL_NEGATIVE | FL_PLUS | FL_SPACE)) {
                    unsigned char z = ' ';
                    if (flags & FL_PLUS) z = '+';
                    if (flags & FL_NEGATIVE) z = '-';
                    my_putc (z, stream);
                }

                /* Output leading zeros */
                while (prec > buf_len) {
                    my_putc ('0', stream);
                    prec--;
                }

                /* Output value */
                while (buf_len)
                    my_putc (buf[--buf_len], stream);
            }
        }

	/* Tail is possible.	*/
	while (width-- > 0) {
	    my_putc (' ', stream);
	}
    } /* for (;;) */

  ret:
    return stream_len;
#undef my_putc
}

#if defined(FORMAT_DEFAULT_DOUBLE) && !defined(vfprintf)
#ifdef HAVE_ALIAS_ATTRIBUTE
__strong_reference(vfprintf, __d_vfprintf);
#else
int __d_vfprintf (FILE * stream, const char *fmt, va_list ap) { return vfprintf(stream, fmt, ap); }
#endif
#endif
#endif
