/******************************************************************************/
/*! @file table_euckr_unicode.cc
    @brief EUC-KR/UCS-2 table.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include "table_charwidth.h"

namespace apolloron {

const char* HALF_ASCII[] = {
    "A", "B", "C", "D", "E", "F", "G", "H",
    "I", "J", "K", "L", "M", "N", "O", "P",
    "Q", "R", "S", "T", "U", "V", "W", "X",
    "Y", "Z", "a", "b", "c", "d", "e", "f",
    "g", "h", "i", "j", "k", "l", "m", "n",
    "o", "p", "q", "r", "s", "t", "u", "v",
    "w", "x", "y", "z", "0", "1", "2", "3",
    "4", "5", "6", "7", "8", "9", " ", "!",
    "\"", "#", "$", "%", "&", "'", "(", ")",
    "*", "+", ",", "-", ".", "/", ":", ";",
    "<", "=", ">", "?", "@", "[", "\\", "]",
    "^", "_", "`", "{", "|", "}",
    "N", "R", "L", "n", "K", "L", "C", "D",
    "M", NULL
};

// UTF-8 Full Width ASCII
const char* FULL_ASCII[] = {
    "\xEF\xBC\xA1", "\xEF\xBC\xA2", "\xEF\xBC\xA3", "\xEF\xBC\xA4",
    "\xEF\xBC\xA5", "\xEF\xBC\xA6", "\xEF\xBC\xA7", "\xEF\xBC\xA8",
    "\xEF\xBC\xA9", "\xEF\xBC\xAA", "\xEF\xBC\xAB", "\xEF\xBC\xAC",
    "\xEF\xBC\xAD", "\xEF\xBC\xAE", "\xEF\xBC\xAF", "\xEF\xBC\xB0",
    "\xEF\xBC\xB1", "\xEF\xBC\xB2", "\xEF\xBC\xB3", "\xEF\xBC\xB4",
    "\xEF\xBC\xB5", "\xEF\xBC\xB6", "\xEF\xBC\xB7", "\xEF\xBC\xB8",
    "\xEF\xBC\xB9", "\xEF\xBC\xBA", "\xEF\xBD\x81", "\xEF\xBD\x82",
    "\xEF\xBD\x83", "\xEF\xBD\x84", "\xEF\xBD\x85", "\xEF\xBD\x86",
    "\xEF\xBD\x87", "\xEF\xBD\x88", "\xEF\xBD\x89", "\xEF\xBD\x8A",
    "\xEF\xBD\x8B", "\xEF\xBD\x8C", "\xEF\xBD\x8D", "\xEF\xBD\x8E",
    "\xEF\xBD\x8F", "\xEF\xBD\x90", "\xEF\xBD\x91", "\xEF\xBD\x92",
    "\xEF\xBD\x93", "\xEF\xBD\x94", "\xEF\xBD\x95", "\xEF\xBD\x96",
    "\xEF\xBD\x97", "\xEF\xBD\x98", "\xEF\xBD\x99", "\xEF\xBD\x9A",
    "\xEF\xBC\x90", "\xEF\xBC\x91", "\xEF\xBC\x92", "\xEF\xBC\x93",
    "\xEF\xBC\x94", "\xEF\xBC\x95", "\xEF\xBC\x96", "\xEF\xBC\x97",
    "\xEF\xBC\x98", "\xEF\xBC\x99", "\xE3\x80\x80", "\xEF\xBC\x81",
    "\xE2\x80\x9D", "\xEF\xBC\x83", "\xEF\xBC\x84", "\xEF\xBC\x85",
    "\xEF\xBC\x86", "\xE2\x80\x99", "\xEF\xBC\x88", "\xEF\xBC\x89",
    "\xEF\xBC\x8A", "\xEF\xBC\x8B", "\xEF\xBC\x8C", "\xEF\xBC\x8D",
    "\xEF\xBC\x8E", "\xEF\xBC\x8F", "\xEF\xBC\x9A", "\xEF\xBC\x9B",
    "\xEF\xBC\x9C", "\xEF\xBC\x9D", "\xEF\xBC\x9E", "\xEF\xBC\x9F",
    "\xEF\xBC\xA0", "\xEF\xBC\xBB", "\xEF\xBF\xA5", "\xEF\xBC\xBD",
    "\xEF\xBC\xBE", "\xEF\xBC\xBF", "\xE2\x80\x98", "\xEF\xBD\x9B",
    "\xEF\xBD\x9C", "\xEF\xBD\x9D",
    "\xC9\xB4", "\xCA\x80", "\xCA\x9F", "\xE2\x81\xBF",
    "\xE2\x84\xAA", "\xE2\x85\xAC", "\xE2\x85\xAD", "\xE2\x85\xAE",
    "\xE2\x85\xAF", NULL
};

// UTF-8 Japanese Katakana
const char* FULL_KANA[] = { // starting with '\xE3'
    "\x80\x82", "\x80\x8C", "\x80\x8D", "\x80\x81",
    "\x83\xBB", "\x83\xB2", "\x82\xA1", "\x82\xA3",
    "\x82\xA5", "\x82\xA7", "\x82\xA9", "\x83\xA3",
    "\x83\xA5", "\x83\xA7", "\x83\x83", "\x83\xBC",
    "\x82\xA2", "\x82\xA4", "\x82\xA6", "\x82\xA8",
    "\x82\xAA", "\x82\xAB", "\x82\xAD", "\x82\xAF",
    "\x82\xB1", "\x82\xB3", "\x82\xB5", "\x82\xB7",
    "\x82\xB9", "\x82\xBB", "\x82\xBD", "\x82\xBF",
    "\x83\x81", "\x83\x84", "\x83\x86", "\x83\x88",
    "\x83\x8A", "\x83\x8B", "\x83\x8C", "\x83\x8D",
    "\x83\x8E", "\x83\x8F", "\x83\x92", "\x83\x95",
    "\x83\x98", "\x83\x9B", "\x83\x9E", "\x83\x9F",
    "\x83\xA0", "\x83\xA1", "\x83\xA2", "\x83\xA4",
    "\x83\xA6", "\x83\xA8", "\x83\xA9", "\x83\xAA",
    "\x83\xAB", "\x83\xAC", "\x83\xAD", "\x83\xAF",
    "\x83\xB3", "\x83\xB4", "\x82\xAC", "\x82\xAE",
    "\x82\xB0", "\x82\xB2", "\x82\xB4", "\x82\xB6",
    "\x82\xB8", "\x82\xBA", "\x82\xBC", "\x82\xBE",
    "\x83\x80", "\x83\x82", "\x83\x85", "\x83\x87",
    "\x83\x89", "\x83\x90", "\x83\x93", "\x83\x96",
    "\x83\x99", "\x83\x9C", "\x83\x91", "\x83\x94",
    "\x83\x97", "\x83\x9A", "\x83\x9D", "\x82\x9B",
    "\x82\x9C", NULL
};

// UTF-8 Japanese Hiragana
const char* FULL_HIRA[] = { // starting with '\xE3'
    "\x80\x82", "\x80\x8C", "\x80\x8D", "\x80\x81",
    "\x83\xBB", "\x82\x92", "\x81\x81", "\x81\x83",
    "\x81\x85", "\x81\x87", "\x81\x89", "\x82\x83",
    "\x82\x85", "\x82\x87", "\x81\xA3", "\x83\xBC",
    "\x81\x82", "\x81\x84", "\x81\x86", "\x81\x88",
    "\x81\x8A", "\x81\x8B", "\x81\x8D", "\x81\x8F",
    "\x81\x91", "\x81\x93", "\x81\x95", "\x81\x97",
    "\x81\x99", "\x81\x9B", "\x81\x9D", "\x81\x9F",
    "\x81\xA1", "\x81\xA4", "\x81\xA6", "\x81\xA8",
    "\x81\xAA", "\x81\xAB", "\x81\xAC", "\x81\xAD",
    "\x81\xAE", "\x81\xAF", "\x81\xB2", "\x81\xB5",
    "\x81\xB8", "\x81\xBB", "\x81\xBE", "\x81\xBF",
    "\x82\x80", "\x82\x81", "\x82\x82", "\x82\x84",
    "\x82\x86", "\x82\x88", "\x82\x89", "\x82\x8A",
    "\x82\x8B", "\x82\x8C", "\x82\x8D", "\x82\x8F",
    "\x82\x93", "\x83\xB4", "\x81\x8C", "\x81\x8E",
    "\x81\x90", "\x81\x92", "\x81\x94", "\x81\x96",
    "\x81\x98", "\x81\x9A", "\x81\x9C", "\x81\x9E",
    "\x81\xA0", "\x81\xA2", "\x81\xA5", "\x81\xA7",
    "\x81\xA9", "\x81\xB0", "\x81\xB3", "\x81\xB6",
    "\x81\xB9", "\x81\xBC", "\x81\xB1", "\x81\xB4",
    "\x81\xB7", "\x81\xBA", "\x81\xBD", "\x82\x9B",
    "\x82\x9C", NULL
};

// UTF-8 Japanese Hankaku Katakana (half width katakana)
const char* HALF_KANA[] = { // starting with '\xEF'
    "\xBD\xA1", "\xBD\xA2", "\xBD\xA3", "\xBD\xA4",
    "\xBD\xA5", "\xBD\xA6", "\xBD\xA7", "\xBD\xA8",
    "\xBD\xA9", "\xBD\xAA", "\xBD\xAB", "\xBD\xAC",
    "\xBD\xAD", "\xBD\xAE", "\xBD\xAF", "\xBD\xB0",
    "\xBD\xB1", "\xBD\xB2", "\xBD\xB3", "\xBD\xB4",
    "\xBD\xB5", "\xBD\xB6", "\xBD\xB7", "\xBD\xB8",
    "\xBD\xB9", "\xBD\xBA", "\xBD\xBB", "\xBD\xBC",
    "\xBD\xBD", "\xBD\xBE", "\xBD\xBF", "\xBE\x80",
    "\xBE\x81", "\xBE\x82", "\xBE\x83", "\xBE\x84",
    "\xBE\x85", "\xBE\x86", "\xBE\x87", "\xBE\x88",
    "\xBE\x89", "\xBE\x8A", "\xBE\x8B", "\xBE\x8C",
    "\xBE\x8D", "\xBE\x8E", "\xBE\x8F", "\xBE\x90",
    "\xBE\x91", "\xBE\x92", "\xBE\x93", "\xBE\x94",
    "\xBE\x95", "\xBE\x96", "\xBE\x97", "\xBE\x98",
    "\xBE\x99", "\xBE\x9A", "\xBE\x9B", "\xBE\x9C",
    "\xBE\x9D", "\xBD\xB3\xEF\xBE\x9E", "\xBD\xB6\xEF\xBE\x9E", "\xBD\xB7\xEF\xBE\x9E",
    "\xBD\xB8\xEF\xBE\x9E", "\xBD\xB9\xEF\xBE\x9E", "\xBD\xBA\xEF\xBE\x9E", "\xBD\xBB\xEF\xBE\x9E",
    "\xBD\xBC\xEF\xBE\x9E", "\xBD\xBD\xEF\xBE\x9E", "\xBD\xBE\xEF\xBE\x9E", "\xBD\xBF\xEF\xBE\x9E",
    "\xBE\x80\xEF\xBE\x9E", "\xBE\x81\xEF\xBE\x9E", "\xBE\x82\xEF\xBE\x9E", "\xBE\x83\xEF\xBE\x9E",
    "\xBE\x84\xEF\xBE\x9E", "\xBE\x8A\xEF\xBE\x9E", "\xBE\x8B\xEF\xBE\x9E", "\xBE\x8C\xEF\xBE\x9E",
    "\xBE\x8D\xEF\xBE\x9E", "\xBE\x8E\xEF\xBE\x9E", "\xBE\x8A\xEF\xBE\x9F", "\xBE\x8B\xEF\xBE\x9F",
    "\xBE\x8C\xEF\xBE\x9F", "\xBE\x8D\xEF\xBE\x9F", "\xBE\x8E\xEF\xBE\x9F", "\xBE\x9E",
    "\xBE\x9F", NULL
};


} // namespace apolloron
