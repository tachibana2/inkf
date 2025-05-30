/******************************************************************************/
/*! @file strmidi.h
    @brief Header file of strmidi.cc.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#ifndef _STRMIDI_H_
#define _STRMIDI_H_

namespace apolloron {

/*----------------------------------------------------------------------------*/
/* String class                                                               */
/*----------------------------------------------------------------------------*/
/*! @brief MIDI data processing functions.
 */

String *strmidi(const char *tracks_text);
String *strtmidi(const char *tracks[], const int max_track);

} // apolloron

#endif
