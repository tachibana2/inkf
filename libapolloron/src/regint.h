/******************************************************************************/
/*! @file regint.h
    @brief Regular expression library.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

/**********************************************************************

  regint.c -  OniGuruma (regular expression library)

  Copyright (C) 2002  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/
#ifndef _REGINT_H_
#define _REGINT_H_

namespace apolloron {

/* for debug */
/* #define REG_DEBUG_PARSE_TREE */
/* #define REG_DEBUG_COMPILE */
/* #define REG_DEBUG_SEARCH */
/* #define REG_DEBUG_MATCH */
/* #define REG_DONT_OPTIMIZE */

/* for byte-code statistical data. */
/* #define REG_DEBUG_STATISTICS */

#if defined(REG_DEBUG_PARSE_TREE) || defined(REG_DEBUG_MATCH) || \
    defined(REG_DEBUG_COMPILE) || defined(REG_DEBUG_STATISTICS)
#ifndef REG_DEBUG
#define REG_DEBUG
#endif
#endif

#if defined(__i386) || defined(__i386__) || defined(_M_IX86) || \
    (defined(__ppc__) && defined(__APPLE__)) || \
    defined(__mc68020__)
#define UNALIGNED_WORD_ACCESS
#endif

/* config */
#define USE_RECYCLE_NODE
#define USE_ERROR_MESSAGE_STRING
/* convert [push, exact...] --> [push_or_jump_exact, exact...] */
#define USE_OP_PUSH_OR_JUMP_EXACT
#define USE_QUALIFIER_PEEK_NEXT
#define INIT_MATCH_STACK_SIZE                  160
#define POSITION_SEARCH_MIN_THRESHOLD_LENGTH     7

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a short.  */
#define SIZEOF_SHORT 2

/* interface to external system */
#define THREAD_ATOMIC_START
#define THREAD_ATOMIC_END
#define xmalloc     malloc
#define xrealloc    realloc
#define xfree       free

/* non Ruby platform default */
/* #define USE_ONIGURUMA_EXTENSION */
#define USE_WORD_BEGIN_END          /* "\<": word-begin, "\>": word-end */
#define DEFAULT_TRANSTABLE_EXIST    1

#define xmemset     memset
#define xmemcpy     memcpy
#define xmemmove    memmove
#define xalloca     alloca

} // namespace apolloron

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>

#ifdef REG_DEBUG
# include <stdio.h>
#endif

#include "regex.h"


namespace apolloron {

#ifndef UNALIGNED_WORD_ACCESS
#define WORD_ALIGNMENT_SIZE       SIZEOF_INT

#define GET_ALIGNMENT_PAD_SIZE(addr,pad_size) do {\
  (pad_size) = WORD_ALIGNMENT_SIZE - ((long)(addr) % WORD_ALIGNMENT_SIZE);\
  if ((pad_size) == WORD_ALIGNMENT_SIZE) (pad_size) = 0;\
} while (0)

#define ALIGNMENT_RIGHT(addr) do {\
  (addr) += (WORD_ALIGNMENT_SIZE - 1);\
  (addr) -= ((long)(addr) % WORD_ALIGNMENT_SIZE);\
} while (0)


#define B_SHIFT  8
#define B_MASK   0xff

#define SERIALIZE_2BYTE_INT(i,p) do {\
  *(p)     = ((i) >> B_SHIFT) & B_MASK;\
  *((p)+1) = (i) & B_MASK;\
} while (0)

#define SERIALIZE_4BYTE_INT(i,p) do {\
  *(p)     = ((i) >> B_SHIFT*3) & B_MASK;\
  *((p)+1) = ((i) >> B_SHIFT*2) & B_MASK;\
  *((p)+2) = ((i) >> B_SHIFT  ) & B_MASK;\
  *((p)+3) = (i) & B_MASK;\
} while (0)

#define SERIALIZE_8BYTE_INT(i,p) do {\
  *(p)     = ((i) >> B_SHIFT*7) & B_MASK;\
  *((p)+1) = ((i) >> B_SHIFT*6) & B_MASK;\
  *((p)+2) = ((i) >> B_SHIFT*5) & B_MASK;\
  *((p)+3) = ((i) >> B_SHIFT*4) & B_MASK;\
  *((p)+4) = ((i) >> B_SHIFT*3) & B_MASK;\
  *((p)+5) = ((i) >> B_SHIFT*2) & B_MASK;\
  *((p)+6) = ((i) >> B_SHIFT  ) & B_MASK;\
  *((p)+7) = (i) & B_MASK;\
} while (0)

#define GET_2BYTE_INT_INC(type,i,p) do {\
  (i) = (type )(((unsigned int )(*(p)) << B_SHIFT) | (unsigned int )((p)[1]));\
  (p) += 2;\
} while (0)

#define GET_4BYTE_INT_INC(type,i,p) do {\
  (i) = (type )(((unsigned int )((p)[0]) << B_SHIFT*3) | \
        ((unsigned int )((p)[1]) << B_SHIFT*2) | \
        ((unsigned int )((p)[2]) << B_SHIFT  ) | \
        ((unsigned int )((p)[3])             )); \
  (p) += 4;\
} while (0)

#define GET_8BYTE_INT_INC(type,i,p) do {\
  (i) = (type )(((unsigned long )((p)[0]) << B_SHIFT*7) | \
        ((unsigned long )((p)[1]) << B_SHIFT*6) | \
        ((unsigned long )((p)[2]) << B_SHIFT*5) | \
        ((unsigned long )((p)[3]) << B_SHIFT*4) | \
        ((unsigned long )((p)[4]) << B_SHIFT*3) | \
        ((unsigned long )((p)[5]) << B_SHIFT*2) | \
        ((unsigned long )((p)[6]) << B_SHIFT  ) | \
        ((unsigned long )((p)[7])             )); \
  (p) += 8;\
} while (0)

#if SIZEOF_SHORT == 2
#define GET_SHORT_INC(i,p)     GET_2BYTE_INT_INC(short,i,p)
#define SERIALIZE_SHORT(i,p)   SERIALIZE_2BYTE_INT(i,p)
#elif SIZEOF_SHORT == 4
#define GET_SHORT_INC(i,p)     GET_4BYTE_INT_INC(short,i,p)
#define SERIALIZE_SHORT(i,p)   SERIALIZE_4BYTE_INT(i,p)
#elif SIZEOF_SHORT == 8
#define GET_SHORT_INC(i,p)     GET_8BYTE_INT_INC(short,i,p)
#define SERIALIZE_SHORT(i,p)   SERIALIZE_8BYTE_INT(i,p)
#endif

#if SIZEOF_INT == 2
#define GET_INT_INC(i,p)       GET_2BYTE_INT_INC(int,i,p)
#define GET_UINT_INC(i,p)      GET_2BYTE_INT_INC(unsigned,i,p)
#define SERIALIZE_INT(i,p)     SERIALIZE_2BYTE_INT(i,p)
#define SERIALIZE_UINT(i,p)    SERIALIZE_2BYTE_INT(i,p)
#elif SIZEOF_INT == 4
#define GET_INT_INC(i,p)       GET_4BYTE_INT_INC(int,i,p)
#define GET_UINT_INC(i,p)      GET_4BYTE_INT_INC(unsigned,i,p)
#define SERIALIZE_INT(i,p)     SERIALIZE_4BYTE_INT(i,p)
#define SERIALIZE_UINT(i,p)    SERIALIZE_4BYTE_INT(i,p)
#elif SIZEOF_INT == 8
#define GET_INT_INC(i,p)       GET_8BYTE_INT_INC(int,i,p)
#define GET_UINT_INC(i,p)      GET_8BYTE_INT_INC(unsigned,i,p)
#define SERIALIZE_INT(i,p)     SERIALIZE_8BYTE_INT(i,p)
#define SERIALIZE_UINT(i,p)    SERIALIZE_8BYTE_INT(i,p)
#endif

#endif /* UNALIGNED_WORD_ACCESS */


/* optimize flags */
#define REG_OPTIMIZE_NONE         0
#define REG_OPTIMIZE_EXACT        1     /* Slow Search */
#define REG_OPTIMIZE_EXACT_BM     2     /* Boyer Moore Search */
#define REG_OPTIMIZE_EXACT_IC     3     /* Slow Search (ignore case) */
#define REG_OPTIMIZE_MAP          5     /* char map */

/* mem stats */
#define MEM_STAT_BITS_NUM      (SIZEOF_INT * 8)
#define MEM_STATS_ON_AT(stats,n) do {\
  if ((n) < MEM_STAT_BITS_NUM)\
    (stats) |= (1 << (n));\
  else\
    (stats) |= 1;\
} while (0)

#define MEM_STATS_ON_ALL(stats)  (stats) = ~((unsigned int)0)

#define GET_MEM_STATS_AT(stats,n) \
  ((n) < MEM_STAT_BITS_NUM ?  ((stats) & (1 << n)) : ((stats) & 1))


#define INT_MAX_LIMIT           ((1UL << (SIZEOF_INT * 8 - 1)) - 1)

typedef unsigned int   WCInt;
#define SIZE_WCINT     sizeof(WCInt)
#define GET_WCINT(wc,p)   (wc) = *((WCInt* )(p))

#define INFINITE_DISTANCE  ~((RegDistance )0)

#if defined STDC_HEADERS || (!defined isascii && !defined HAVE_ISASCII)
# define IS_ASCII(c) 1
#else
# define IS_ASCII(c) isascii(c)
#endif

#ifdef isblank
# define IS_BLANK(c) (IS_ASCII(c) && isblank(c))
#else
# define IS_BLANK(c) ((c) == ' ' || (c) == '\t')
#endif
#ifdef isgraph
# define IS_GRAPH(c) (IS_ASCII(c) && isgraph(c))
#else
# define IS_GRAPH(c) (IS_ASCII(c) && isprint(c) && !isspace(c))
#endif

#define IS_PRINT(c)  (isprint(c)  && IS_ASCII(c))
#define IS_ALNUM(c)  (isalnum(c)  && IS_ASCII(c))
#define IS_ALPHA(c)  (isalpha(c)  && IS_ASCII(c))
#define IS_LOWER(c)  (islower(c)  && IS_ASCII(c))
#define IS_UPPER(c)  (isupper(c)  && IS_ASCII(c))
#define IS_CNTRL(c)  (iscntrl(c)  && IS_ASCII(c))
#define IS_PUNCT(c)  (ispunct(c)  && IS_ASCII(c))
#define IS_SPACE(c)  (isspace(c)  && IS_ASCII(c))
#define IS_DIGIT(c)  (isdigit(c)  && IS_ASCII(c))
#define IS_XDIGIT(c) (isxdigit(c) && IS_ASCII(c))
#define IS_ODIGIT(c) (IS_DIGIT(c) && (c) < '8')

#define DIGITVAL(c)    ((c) - '0')
#define ODIGITVAL(c)   DIGITVAL(c)
#define XDIGITVAL(c) \
  (IS_DIGIT(c) ? DIGITVAL(c) : (IS_UPPER(c) ? (c) - 'A' + 10 : (c) - 'a' + 10))

#define IS_SINGLELINE(option)     ((option) & REG_OPTION_SINGLELINE)
#define IS_MULTILINE(option)      ((option) & REG_OPTION_MULTILINE)
#define IS_IGNORECASE(option)     ((option) & REG_OPTION_IGNORECASE)
#define IS_EXTEND(option)         ((option) & REG_OPTION_EXTEND)
#define IS_FIND_LONGEST(option)   ((option) & REG_OPTION_FIND_LONGEST)
#define IS_FIND_NOT_EMPTY(option) ((option) & REG_OPTION_FIND_NOT_EMPTY)
#define IS_POSIXLINE(option)      (IS_SINGLELINE(option) && IS_MULTILINE(option))
#define IS_FIND_CONDITION(option) ((option) & \
          (REG_OPTION_FIND_LONGEST | REG_OPTION_FIND_NOT_EMPTY))
#define IS_NOTBOL(option)         ((option) & REG_OPTION_NOTBOL)
#define IS_NOTEOL(option)         ((option) & REG_OPTION_NOTEOL)

#ifdef NEWLINE
#undef NEWLINE
#endif
#define NEWLINE     '\n'
#define IS_NULL(p)                    (((void*)(p)) == (void*)0)
#define IS_NOT_NULL(p)                (((void*)(p)) != (void*)0)
#define IS_NEWLINE(c)                 ((c) == NEWLINE)
#define CHECK_NULL_RETURN(p)          if (IS_NULL(p)) return NULL
#define CHECK_NULL_RETURN_VAL(p,val)  if (IS_NULL(p)) return (val)

/* bitset */
#define BITS_PER_BYTE      8
#define SINGLE_BYTE_SIZE   (1 << BITS_PER_BYTE)
#define BITS_IN_ROOM       (sizeof(Bits) * BITS_PER_BYTE)
#define BITSET_SIZE        (SINGLE_BYTE_SIZE / BITS_IN_ROOM)

#ifdef UNALIGNED_WORD_ACCESS
typedef unsigned int   Bits;
#else
typedef unsigned char  Bits;
#endif
typedef Bits           BitSet[BITSET_SIZE];
typedef Bits*          BitSetRef;

#define NULL_UCHARP       ((UChar* )0)

#define BITSET_CLEAR(bs) do {\
  unsigned int i;\
  for (i = 0; i < BITSET_SIZE; i++) { (bs)[i] = 0; }\
} while (0)

#define BS_ROOM(bs,pos)            (bs)[pos / BITS_IN_ROOM]
#define BS_BIT(pos)                (1 << (pos % BITS_IN_ROOM))

#define BITSET_AT(bs, pos)         (BS_ROOM(bs,pos) & BS_BIT(pos))
#define BITSET_SET_BIT(bs, pos)     BS_ROOM(bs,pos) |= BS_BIT(pos)
#define BITSET_CLEAR_BIT(bs, pos)   BS_ROOM(bs,pos) &= ~(BS_BIT(pos))

#define SIZE_BITSET        sizeof(BitSet)

/* bytes buffer */
typedef struct _BBuf {
    UChar* p;
    unsigned int used;
    unsigned int alloc;
} BBuf;

#define BBUF_INIT(buf,size)    regex_bbuf_init((BBuf* )(buf), (size))

#define BBUF_SIZE_INC(buf,inc) do{\
  (buf)->alloc += (inc);\
  (buf)->p = (UChar* )xrealloc((buf)->p, (buf)->alloc);\
  if (IS_NULL((buf)->p)) return(REGERR_MEMORY);\
} while (0)

#define BBUF_EXPAND(buf,low) do{\
  do { (buf)->alloc *= 2; } while ((long)((buf)->alloc) < low);\
  (buf)->p = (UChar* )xrealloc((buf)->p, (buf)->alloc);\
  if (IS_NULL((buf)->p)) return(REGERR_MEMORY);\
} while (0)

#define BBUF_ENSURE_SIZE(buf,size) do{\
  int new_alloc = (buf)->alloc;\
  while (new_alloc < (long)(size)) { new_alloc *= 2; }\
  if ((long)((buf)->alloc) != new_alloc) {\
    (buf)->p = (UChar* )xrealloc((buf)->p, new_alloc);\
    if (IS_NULL((buf)->p)) return(REGERR_MEMORY);\
    (buf)->alloc = new_alloc;\
  }\
} while (0)

#define BBUF_WRITE(buf,pos,bytes,n) do{\
  int used = (pos) + (n);\
  if ((long)((buf))->alloc < used) BBUF_EXPAND((buf),used);\
  xmemcpy((buf)->p + (pos), (bytes), (n));\
  if ((long)((buf)->used) < used) (buf)->used = used;\
} while (0)

#define BBUF_WRITE1(buf,pos,byte) do{\
  int used = (pos) + 1;\
  if ((long)((buf)->alloc) < used) BBUF_EXPAND((buf),used);\
  (buf)->p[(pos)] = (byte);\
  if ((long)((buf)->used) < used) (buf)->used = used;\
} while (0)

#define BBUF_ADD(buf,bytes,n)       BBUF_WRITE((buf),(buf)->used,(bytes),(n))
#define BBUF_ADD1(buf,byte)         BBUF_WRITE1((buf),(buf)->used,(byte))
#define BBUF_GET_ADD_POSITION(buf)  ((buf)->p + (buf)->used)

/* from < to */
#define BBUF_MOVE_RIGHT(buf,from,to,n) do {\
  if ((to) + (n) > (long)((buf)->alloc)) BBUF_EXPAND((buf),(to) + (n));\
  xmemmove((buf)->p + (to), (buf)->p + (from), (n));\
  if ((to) + (n) > (long)((buf)->used)) (buf)->used = (to) + (n);\
} while (0)

/* from > to */
#define BBUF_MOVE_LEFT(buf,from,to,n) do {\
  xmemmove((buf)->p + (to), (buf)->p + (from), (n));\
} while (0)

/* from > to */
#define BBUF_MOVE_LEFT_REDUCE(buf,from,to) do {\
  xmemmove((buf)->p + (to), (buf)->p + (from), (buf)->used - (from));\
  (buf)->used -= (from - to);\
} while (0)

#define BBUF_INSERT(buf,pos,bytes,n) do {\
  if (pos >= (buf)->used) {\
    BBUF_WRITE(buf,pos,bytes,n);\
  }\
  else {\
    BBUF_MOVE_RIGHT((buf),(pos),(pos) + (n),((buf)->used - (pos)));\
    xmemcpy((buf)->p + (pos), (bytes), (n));\
  }\
} while (0)

#define BBUF_GET_BYTE(buf, pos) (buf)->p[(pos)]

#define TTRANS(table,c)        ((table)[c])

/* methods for support multi-byte code, */
#define ismb(code,c)             (mblen((code),(c)) != 1)
#define MB2WC(p,end,code)        mb2wc((p),(end),(code))
#define MBBACK(code,start,s,n)   step_backward_char((code),(start),(s),(n))

#ifdef REG_RUBY_M17N

#define MB2WC_AVAILABLE(enc)     1
#define WC2MB_FIRST(enc, wc)          m17n_firstbyte((enc),(wc))

#define mbmaxlen(enc)                 m17n_mbmaxlen(enc)
#define mblen(enc,c)                  m17n_mbclen(enc,c)
#define mbmaxlen_dist(enc) \
      (mbmaxlen(enc) > 0 ? mbmaxlen(enc) : INFINITE_DISTANCE)

#define IS_SINGLEBYTE_CODE(enc)    (m17n_mbmaxlen(enc) == 1)
/* #define IS_INDEPENDENT_TRAIL(enc)  m17n_independent_trail(enc) */
#define IS_INDEPENDENT_TRAIL(enc)  IS_SINGLEBYTE_CODE(enc)

#define IS_CODE_ASCII(enc,c)     IS_ASCII(c)
#define IS_CODE_GRAPH(enc,c)     IS_GRAPH(c)
#define IS_CODE_PRINT(enc,c)     m17n_isprint(enc,c)
#define IS_CODE_ALNUM(enc,c)     m17n_isalnum(enc,c)
#define IS_CODE_ALPHA(enc,c)     m17n_isalpha(enc,c)
#define IS_CODE_LOWER(enc,c)     m17n_islower(enc,c)
#define IS_CODE_UPPER(enc,c)     m17n_isupper(enc,c)
#define IS_CODE_CNTRL(enc,c)     m17n_iscntrl(enc,c)
#define IS_CODE_PUNCT(enc,c)     m17n_ispunct(enc,c)
#define IS_CODE_SPACE(enc,c)     m17n_isspace(enc,c)
#define IS_CODE_BLANK(enc,c)     IS_BLANK(c)
#define IS_CODE_DIGIT(enc,c)     m17n_isdigit(enc,c)
#define IS_CODE_XDIGIT(enc,c)    m17n_isxdigit(enc,c)

#define IS_CODE_WORD(enc,c)      m17n_iswchar(enc,c)
#define ISNOT_CODE_WORD(enc,c)  (!m17n_iswchar(enc,c))

#define IS_WORD_STR(code,s,end) \
  (ismb((code),*(s)) ? (s + mblen((code),*(s)) <= (end)) : \
                       m17n_iswchar(code,*(s)))
#define IS_WORD_STR_INC(code,s,end) \
  (ismb((code),*(s)) ? ((s) += mblen((code),*(s)), (s) <= (end)) : \
                       (s++, m17n_iswchar(code,s[-1])))

#define IS_WORD_HEAD(enc,c) (ismb(enc,c) ? 1 : IS_CODE_WORD(enc,c))

#define IS_SB_WORD(code,c)  (mblen(code,c) == 1 && IS_CODE_WORD(code,c))
#define IS_MB_WORD(code,c)  ismb(code,c)

#define mb2wc(p,e,enc)        m17n_codepoint((enc),(p),(e))

#else  /* REG_RUBY_M17N */

#define mb2wc(p,e,code)              regex_mb2wc((p),(e),(code))

#define MB2WC_AVAILABLE(code)       1
#define WC2MB_FIRST(code, wc)       regex_wc2mb_first(code, wc)

#define mbmaxlen_dist(code)         mbmaxlen(code)
#define mbmaxlen(code)              regex_mb_max_length(code)
#define mblen(code,c)               (code)[(c)]

#define IS_SINGLEBYTE_CODE(code)    ((code) == REGCODE_ASCII)
#define IS_INDEPENDENT_TRAIL(code) \
        ((code) == REGCODE_ASCII || (code) == REGCODE_UTF8)

#define IS_CODE_ASCII(code,c)        IS_ASCII(c)
#define IS_CODE_GRAPH(code,c)        IS_GRAPH(c)
#define IS_CODE_PRINT(code,c)        IS_PRINT(c)
#define IS_CODE_ALNUM(code,c)        IS_ALNUM(c)
#define IS_CODE_ALPHA(code,c)        IS_ALPHA(c)
#define IS_CODE_LOWER(code,c)        IS_LOWER(c)
#define IS_CODE_UPPER(code,c)        IS_UPPER(c)
#define IS_CODE_CNTRL(code,c)        IS_CNTRL(c)
#define IS_CODE_PUNCT(code,c)        IS_PUNCT(c)
#define IS_CODE_SPACE(code,c)        IS_SPACE(c)
#define IS_CODE_BLANK(code,c)        IS_BLANK(c)
#define IS_CODE_DIGIT(code,c)        IS_DIGIT(c)
#define IS_CODE_XDIGIT(code,c)       IS_XDIGIT(c)

#define IS_SB_WORD(code,c)  (IS_CODE_ALNUM(code,c) || (c) == '_')
#define IS_MB_WORD(code,c)  ismb(code,c)

#define IS_CODE_WORD(code,c) \
  (IS_SB_WORD(code,c) && ((c) < 0x80 || (code) == REGCODE_ASCII))
#define ISNOT_CODE_WORD(code,c) \
  ((!IS_SB_WORD(code,c)) && !ismb(code,c))

#define IS_WORD_STR(code,s,end) \
  (ismb((code),*(s)) ? (s + mblen((code),*(s)) <= (end)) : \
                       IS_SB_WORD(code,*(s)))
#define IS_WORD_STR_INC(code,s,end) \
  (ismb((code),*(s)) ? ((s) += mblen((code),*(s)), (s) <= (end)) : \
                       (s++, IS_SB_WORD(code,s[-1])))

#define IS_WORD_HEAD(code,c) (ismb(code,c) ? 1 : IS_SB_WORD(code,c))

#ifdef __STDC__
extern int   regex_mb_max_length(RegCharEncoding code);
extern WCInt regex_mb2wc(UChar* p, UChar* end, RegCharEncoding code);
extern int   regex_wc2mb_first(RegCharEncoding code, WCInt wc);
#else
extern int   regex_mb_max_length();
extern WCInt regex_mb2wc();
extern int   regex_wc2mb_first();
#endif

#endif /* not REG_RUBY_M17N */


#define ANCHOR_BEGIN_BUF        (1<<0)
#define ANCHOR_BEGIN_LINE       (1<<1)
#define ANCHOR_BEGIN_POSITION   (1<<2)
#define ANCHOR_END_BUF          (1<<3)
#define ANCHOR_SEMI_END_BUF     (1<<4)
#define ANCHOR_END_LINE         (1<<5)
#define ANCHOR_ANYCHAR_STAR     (1<<6)   /* ".*" optimize info */
#define ANCHOR_ANYCHAR_STAR_PL  (1<<7)   /* ".*" optimize info (posix-line) */

/* operation code */
enum OpCode {
    OP_FINISH = 0,        /* matching process terminator (no more alternative) */
    OP_END    = 1,        /* pattern code terminator (success end) */

    OP_EXACT1 = 2,        /* single byte, N = 1 */
    OP_EXACT2,            /* single byte, N = 2 */
    OP_EXACT3,            /* single byte, N = 3 */
    OP_EXACT4,            /* single byte, N = 4 */
    OP_EXACT5,            /* single byte, N = 5 */
    OP_EXACTN,            /* single byte */
    OP_EXACTMB2N1,        /* mb-length = 2 N = 1 */
    OP_EXACTMB2N2,        /* mb-length = 2 N = 2 */
    OP_EXACTMB2N3,        /* mb-length = 2 N = 3 */
    OP_EXACTMB2N,         /* mb-length = 2 */
    OP_EXACTMB3N,         /* mb length = 3 */
    OP_EXACTMBN,          /* other length */

    OP_EXACT1_IC,         /* single byte, N = 1, ignore case */
    OP_EXACTN_IC,         /* single byte,        ignore case */

    OP_CCLASS,
    OP_CCLASS_MB,
    OP_CCLASS_MIX,
    OP_CCLASS_NOT,
    OP_CCLASS_MB_NOT,
    OP_CCLASS_MIX_NOT,

    OP_ANYCHAR,                /* "."  */
    OP_ANYCHAR_STAR,           /* ".*" */
    OP_ANYCHAR_STAR_PEEK_NEXT,

    OP_WORD,
    OP_NOT_WORD,
    OP_WORD_SB,
    OP_WORD_MB,
    OP_WORD_BOUND,
    OP_NOT_WORD_BOUND,
    OP_WORD_BEGIN,
    OP_WORD_END,

    OP_BEGIN_BUF,
    OP_END_BUF,
    OP_BEGIN_LINE,
    OP_END_LINE,
    OP_SEMI_END_BUF,
    OP_BEGIN_POSITION,

    OP_BACKREF1,
    OP_BACKREF2,
    OP_BACKREF3,
    OP_BACKREFN,

    OP_MEMORY_START_PUSH,  /* push stack */
    OP_MEMORY_END_PUSH,    /* push stack */
    OP_MEMORY_END,

    OP_SET_OPTION_PUSH,    /* set option and push recover option */
    OP_SET_OPTION,         /* set option */

    OP_FAIL,               /* pop stack and move */
    OP_JUMP,
    OP_PUSH,
    OP_PUSH_OR_JUMP_EXACT1, /* if match exact then push, else jump. */
    OP_PUSH_IF_PEEK_NEXT,   /* if match exact then push, else none. */
    OP_REPEAT,             /* {n,m} */
    OP_REPEAT_NG,          /* {n,m}? (non greedy) */
    OP_REPEAT_INC,
    OP_REPEAT_INC_NG,      /* non greedy */
    OP_NULL_CHECK_START,   /* null loop checker start */
    OP_NULL_CHECK_END,     /* null loop checker end   */

    OP_PUSH_POS,             /* (?=...)  start */
    OP_POP_POS,              /* (?=...)  end   */
    OP_PUSH_POS_NOT,         /* (?!...)  start */
    OP_FAIL_POS,             /* (?!...)  end   */
    OP_PUSH_STOP_BT,         /* (?>...)  start */
    OP_POP_STOP_BT,          /* (?>...)  end   */
    OP_LOOK_BEHIND,          /* (?<=...) start (no needs end opcode) */
    OP_PUSH_LOOK_BEHIND_NOT, /* (?<!...) start */
    OP_FAIL_LOOK_BEHIND_NOT  /* (?<!...) end   */
};

/* arguments type */
#define ARG_SPECIAL     -1
#define ARG_NON          0
#define ARG_RELADDR      1
#define ARG_LENGTH       2
#define ARG_MEMNUM       3
#define ARG_OPTION       4

typedef short int   RelAddrType;
typedef short int   LengthType;
typedef short int   MemNumType;
typedef int         RepeatNumType;

#define SIZE_OPCODE        1
#define SIZE_RELADDR       sizeof(RelAddrType)
#define SIZE_LENGTH        sizeof(LengthType)
#define SIZE_MEMNUM        sizeof(MemNumType)
#define SIZE_REPEATNUM     sizeof(RepeatNumType)
#define SIZE_OPTION        sizeof(RegOptionType)

#ifdef UNALIGNED_WORD_ACCESS
#define GET_RELADDR_INC(addr,p) do{\
  addr = *((RelAddrType* )(p));\
  (p) += SIZE_RELADDR;\
} while(0)

#define GET_LENGTH_INC(len,p) do{\
  len = *((LengthType* )(p));\
  (p) += SIZE_LENGTH;\
} while(0)

#define GET_MEMNUM_INC(num,p) do{\
  num = *((MemNumType* )(p));\
  (p) += SIZE_MEMNUM;\
} while(0)

#define GET_REPEATNUM_INC(num,p) do{\
  num = *((RepeatNumType* )(p));\
  (p) += SIZE_REPEATNUM;\
} while(0)

#define GET_OPTION_INC(option,p) do{\
  option = *((RegOptionType* )(p));\
  (p) += SIZE_OPTION;\
} while(0)
#else

#define GET_RELADDR_INC(addr,p)      GET_SHORT_INC(addr,p)
#define GET_LENGTH_INC(len,p)        GET_SHORT_INC(len,p)
#define GET_MEMNUM_INC(num,p)        GET_SHORT_INC(num,p)
#define GET_REPEATNUM_INC(num,p)     GET_INT_INC(num,p)
#define GET_OPTION_INC(option,p)     GET_UINT_INC(option,p)

#define SERIALIZE_RELADDR(addr,p)    SERIALIZE_SHORT(addr,p)
#define SERIALIZE_LENGTH(len,p)      SERIALIZE_SHORT(len,p)
#define SERIALIZE_MEMNUM(num,p)      SERIALIZE_SHORT(num,p)
#define SERIALIZE_REPEATNUM(num,p)   SERIALIZE_INT(num,p)
#define SERIALIZE_OPTION(option,p)   SERIALIZE_UINT(option,p)

#define SERIALIZE_BUFSIZE            SIZEOF_INT

#endif  /* UNALIGNED_WORD_ACCESS */

#define GET_BYTE_INC(byte,p) do{\
  byte = *(p);\
  (p)++;\
} while(0)


/* op-code + arg size */
#define SIZE_OP_ANYCHAR_STAR            SIZE_OPCODE
#define SIZE_OP_ANYCHAR_STAR_PEEK_NEXT (SIZE_OPCODE + 1)
#define SIZE_OP_JUMP                   (SIZE_OPCODE + SIZE_RELADDR)
#define SIZE_OP_PUSH                   (SIZE_OPCODE + SIZE_RELADDR)
#define SIZE_OP_PUSH_OR_JUMP_EXACT1    (SIZE_OPCODE + SIZE_RELADDR + 1)
#define SIZE_OP_PUSH_IF_PEEK_NEXT      (SIZE_OPCODE + SIZE_RELADDR + 1)
#define SIZE_OP_REPEAT_INC             (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_REPEAT_INC_NG          (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_PUSH_POS                SIZE_OPCODE
#define SIZE_OP_PUSH_POS_NOT           (SIZE_OPCODE + SIZE_RELADDR)
#define SIZE_OP_POP_POS                 SIZE_OPCODE
#define SIZE_OP_FAIL_POS                SIZE_OPCODE
#define SIZE_OP_SET_OPTION             (SIZE_OPCODE + SIZE_OPTION)
#define SIZE_OP_SET_OPTION_PUSH        (SIZE_OPCODE + SIZE_OPTION)
#define SIZE_OP_FAIL                    SIZE_OPCODE
#define SIZE_OP_MEMORY_START_PUSH      (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_MEMORY_END_PUSH        (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_MEMORY_END             (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_PUSH_STOP_BT            SIZE_OPCODE
#define SIZE_OP_POP_STOP_BT             SIZE_OPCODE
#define SIZE_OP_NULL_CHECK_START       (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_NULL_CHECK_END         (SIZE_OPCODE + SIZE_MEMNUM)
#define SIZE_OP_LOOK_BEHIND            (SIZE_OPCODE + SIZE_LENGTH)
#define SIZE_OP_PUSH_LOOK_BEHIND_NOT   (SIZE_OPCODE + SIZE_RELADDR + SIZE_LENGTH)
#define SIZE_OP_FAIL_LOOK_BEHIND_NOT    SIZE_OPCODE


#ifdef __STDC__
extern int regex_bbuf_init(BBuf* buf, int size);
#else
extern int regex_bbuf_init();
#endif


} // namespace apolloron
#endif /* REGINT_H */
