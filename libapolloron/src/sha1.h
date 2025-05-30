/******************************************************************************/
/*! @file sha1.h
    @brief Header file of sha1.cc.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/
/*
 * This file comes from RFC 3174.
 */

#ifndef _SHA1_H_
#define _SHA1_H_

namespace apolloron {

#ifndef SHA1_DIGESTSIZE
#define SHA1_DIGESTSIZE  20
#endif

#ifndef SHA1_BLOCKSIZE
#define SHA1_BLOCKSIZE   64
#endif

typedef struct {
    unsigned int state[5];
    unsigned int count[2];  /* stores the number of bits */
    unsigned char buffer[SHA1_BLOCKSIZE];
} SHA1_CTX;

void SHA1Transform(unsigned int state[5], const unsigned char buffer[SHA1_BLOCKSIZE]);
void SHA1Init(SHA1_CTX *context);
void SHA1Update(SHA1_CTX *context, const unsigned char *data, unsigned int len);
void SHA1Final(unsigned char digest[SHA1_DIGESTSIZE], SHA1_CTX *context);

} // namespace apolloron

#endif /* _SHA1_H_ */
