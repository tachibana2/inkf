/******************************************************************************/
/*! @file fcgimisc.h
    @brief Header file of fcgi.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

/*
 * fcgimisc.h --
 *
 *      Miscellaneous definitions
 *
 *
 * Copyright (c) 1996 Open Market, Inc.
 *
 * See the file "LICENSE.FCGI" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * $Id: fcgimisc.h,v 1.3 2001/06/18 14:25:47 robs Exp $
 */

#ifndef _FCGIMISC_H
#define _FCGIMISC_H

namespace apolloron {

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE  (1)
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

} // namespace apolloron

#endif  /* _FCGIMISC_H */
