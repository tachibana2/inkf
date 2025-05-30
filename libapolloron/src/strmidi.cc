/******************************************************************************/
/*! @file strmidi.cc
    @brief MIDI data processing functions
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apolloron.h"
#include "strmidi.h"

namespace apolloron {

#define HEAD_LENGTH 116
#define HEAD_TEMPO 52
#define HEAD_TRACK 10

const unsigned char head_master[HEAD_LENGTH] = {
    // File header
    0x4d, 0x54, 0x68, 0x64, 0x00, 0x00, 0x00, 0x06,
    0x00, 0x01,
    0x00, 0x01, // tracks
    0x00, 0x78, 0x4d, 0x54, 0x72, 0x6B,
    0x00, 0x00, 0x00, 0x5e, // file size
    // Etc
    0x00, 0xf0, 0x0a, 0x41, 0x10, 0x42, 0x12, 0x40,
    0x00, 0x7f, 0x00, 0x41, 0xf7, 0x00, 0xff, 0x58,
    0x04, 0x04, 0x02, 0x18, 0x08, 0x00, 0xff, 0x59,
    0x02, 0x00, 0x00, 0x00,
    // tempo (micro sec)
    0xff, 0x51, 0x03,
    0x07, 0x27, 0x0e, // tempo 468,750=60,000,000/(128)
    0x00, 0xff, 0x2f, 0x00
};

const unsigned char trkhead_master[13]= {
    // Track header
    0x4d, 0x54, 0x72, 0x6b,
    // Track size
    0x00, 0x00, 0x00, 0x08,
    // Patch
    0x7f, 0xc0, 0x00,
    // n0
    0x00, // n4=0x78
    // MIDI Channel Number (0x90=ch1, 0x91=ch2, ... , 0x9f=ch16)
    0x90
};

const unsigned char eot[3]= {
    0xff, 0x2f, 0x00
};

typedef struct {
    int notepos, intervalpos, firstinterval;
    int def_O, def_L, def_V;
    int def_Patch[16];
    unsigned char *midi_channel, *patch;
    unsigned char *notes;
    int waon_kakko;
    unsigned char head[HEAD_LENGTH];
    unsigned char trkhead[13];
} TMIDISession;

#define isnote(c) ((('a'<=c && c<='g') || ('A'<=c && c<='G') || c=='n' || c=='N' || c=='r' || c=='R' || c=='o' || c=='O' || c=='l' || c=='L' || c=='v' || c=='V' || c=='m' || c=='M' || c=='@' || c=='t' || c=='T' || c=='<' || c=='>' || c=='(' || c==')')?1:0)

static int _setnote(TMIDISession *midi_session, const unsigned char *track, const int textpos, const int textlen);


/*! create MIDI data from MIDI text.
    @param tracks_text  MIDI text
             T1 - T255           ... tempo
             M1 - M16            ... MIDI channel
             @1 - @128           ... sound
             C, D, E, F, G, A, B ... note
             R (N)               ... silence note
             S (#)               ... sharp
             _ (-)               ... flat
             (..)                ... waon (concurrent output)
             L1 - L255           ... length
             .                   ... 1.5x length
             L~1 - L~255         ... length (L4 == L~4, L~1 == L16)
             [..]1 - [..]99      ... loop
             $a - $z             ... variables
             $a[..]  - $z[..]    ... set variables
             +                   ... next track
    @example
             String midi_str, midi_data;
             midi_str = "cde";
             midi_data = midi_str.toMIDI();
             midi_data.saveFile("test.mid");
             midi_str.clear();
             midi_data.clear();
    @return MIDI data (binary String object)
 */
String *strmidi(const char *tracks_text) {
    unsigned char *cpy_tracks_text, **tracks;
    int max_track;
    int length, i;
    String *MIDI;

    if (tracks_text == NULL || tracks_text[0] == '\0') {
        MIDI = new String;
        MIDI->useAsBinary(0);
        return MIDI;
    }

    // Create array of pointers, to call strtmidi
    length = strlen(tracks_text);
    cpy_tracks_text = new unsigned char [length+1];
    memcpy(cpy_tracks_text, tracks_text, length);
    cpy_tracks_text[length] = '\0';
    max_track = 1;
    tracks = new unsigned char * [sizeof(unsigned char *) * max_track];
    tracks[0] = &cpy_tracks_text[0];
    i = 0;
    while (i < length) {
        if (cpy_tracks_text[i] == '\n') {
            unsigned char **new_tracks;
            int j;

            cpy_tracks_text[i] = '\0';

            new_tracks = new unsigned char * [sizeof(unsigned char *) * (max_track + 1)];
            for (j = 0; j < max_track; j++) {
                new_tracks[j] = tracks[j];
            }
            delete [] tracks;
            tracks = new_tracks;
            max_track++;

            tracks[max_track - 1] = &cpy_tracks_text[i + 1];
        } else {
            cpy_tracks_text[i] = (unsigned char)tracks_text[i];
        }
        i++;
    }
    cpy_tracks_text[i] = '\0';

    MIDI = strtmidi((const char **)tracks, max_track);
    delete [] tracks;
    delete [] cpy_tracks_text;
    return MIDI;
}


/*! create MIDI data from texts of track images.
    @param tracks     array of text that specifying MIDI truck
    @param max_track  max number of tracks
    @return MIDI data (binary String object)
 */
String *strtmidi(const char *tracks[], const int max_track) {
    int i, ii, xtrack;
    int textlineloop, textpos_max, current_line, current_line_start_pos, textpos, trackslen;
    int notes_size_max; // buffer size of midi_session.notes data
    int line_size_max;
    unsigned char *line, *before_line;
    unsigned char *line0, *line1;
    TMIDISession midi_session;
    String *MIDI;

    // init
    memset(midi_session.def_Patch, 0, sizeof(int) * 16);
    memcpy(midi_session.head, head_master, HEAD_LENGTH);
    memcpy(midi_session.trkhead, trkhead_master, 13);

    notes_size_max = 1024;
    line_size_max = 1024;

    midi_session.notes = new unsigned char[notes_size_max+2];
    line0 = new unsigned char[line_size_max+2];
    line1 = new unsigned char[line_size_max+2];

    MIDI = new String;
    MIDI->useAsBinary(0);

    xtrack=0;
    midi_session.notepos = 0;
    line = line0;

    textpos_max = 0;
    current_line_start_pos = 0;
    for (current_line = 0; current_line<max_track; current_line++) {
        if (notes_size_max <= midi_session.notepos+29) {
            unsigned char *buf;
            int ps;
            ps = notes_size_max;
            notes_size_max += 1024;
            buf = new unsigned char[notes_size_max + 2];
            memcpy(buf, midi_session.notes, ps);
            delete [] midi_session.notes;
            midi_session.notes = buf;
        }
        if (tracks[current_line][0] == '\0' || tracks[current_line][0] == '/') {
            xtrack++;
            continue;
        }
        midi_session.def_O = 4;
        midi_session.def_L = 4;
        midi_session.def_V = 100;
        current_line_start_pos = midi_session.notepos;
        memcpy(midi_session.notes+midi_session.notepos, midi_session.trkhead, 13);
        midi_session.midi_channel = midi_session.notes+midi_session.notepos+12;
        midi_session.patch = midi_session.notes+midi_session.notepos+10;
        midi_session.notepos += 13;
        midi_session.firstinterval = midi_session.intervalpos = midi_session.notepos-2;

        trackslen = strlen(tracks[current_line]);
        before_line = (unsigned char *)tracks[current_line];
        for (textlineloop = 0; textlineloop < 256; textlineloop++) {
            int stk0;
            stk0 = 0;
            textpos_max = strlen((const char *)before_line);
            ii = 0;
            i = 0;
            while (i < textpos_max) {
                if (before_line[i] == '/') {
                    break;
                }
                if (before_line[i] == '$') {
                    char ch;
                    i++;
                    if (before_line[i]=='/')break;
                    ch=before_line[i];
                    if (textpos_max<=i || !(('a'<=ch && ch<='z') || ('A'<=ch && ch<='Z'))) {
                        delete [] line0;
                        delete [] line1;
                        delete [] midi_session.notes;
                        return MIDI;
                    }
                    i++;
                    if (before_line[i] == '[') {
                        stk0++;
                        if (stk0 != 1) {
                            line[ii] = before_line[i-2];
                            line[ii+1] = before_line[i-1];
                            line[ii+2] = before_line[i];
                            ii+=3;
                            i++;
                            if (line_size_max<=ii) {
                                unsigned char *buf0, *buf1;
                                int ps;
                                ps = line_size_max;
                                line_size_max += 1024;
                                buf0 = new unsigned char[line_size_max + 2];
                                buf1 = new unsigned char[line_size_max + 2];
                                memcpy(buf0, line0, ps);
                                memcpy(buf1, line1, ps);
                                delete [] line0;
                                delete [] line1;
                                line0 = buf0;
                                line1 = buf1;
                            }
                            continue;
                        }
                        i++;
                        if (before_line[i] == '/') {
                            break;
                        }
                    } else {
                        int iii, atta;
                        iii=0;
                        atta=0;
                        while (iii < trackslen-3) {
                            if (tracks[current_line][iii] == '$' && tracks[current_line][iii+1] == ch && tracks[current_line][iii+2] == '[') {
                                iii += 3;
                                atta = 1;
                                break;
                            }
                            iii++;
                        }
                        if (atta == 1) {
                            int iii_start, iii_end, l, loop, stk;
                            iii_start = iii;
                            stk=0;
                            while (iii < trackslen-3) {
                                if (tracks[current_line][iii] == '[') {
                                    stk++;
                                }
                                if (tracks[current_line][iii] == ']') {
                                    if (stk == 0) {
                                        break;
                                    } else {
                                        stk--;
                                    }
                                }
                                iii++;
                            }
                            iii_end=iii-1;
                            loop=1;
                            if ('0' <= before_line[i] && before_line[i] <= '9') {
                                loop = before_line[i]-'0';
                                i++;
                                if ('0' <= before_line[i] && before_line[i] <= '9') {
                                    loop=loop*10+before_line[i]-'0';
                                    i++;
                                    if ('0' <= before_line[i] && before_line[i] <= '9') {
                                        delete [] line0;
                                        delete [] line1;
                                        delete [] midi_session.notes;
                                        return MIDI;
                                    }
                                }
                            }
                            l=0;
                            while(l<loop) {
                                iii = iii_start;
                                stk=0;
                                while (iii <= iii_end) {
                                    if (tracks[current_line][iii] == '[') {
                                        stk++;
                                    }
                                    if (tracks[current_line][iii] == ']') {
                                        if (stk == 0) {
                                            break;
                                        } else {
                                            stk--;
                                        }
                                    }
                                    line[ii] = tracks[current_line][iii];
                                    ii++;
                                    if (line_size_max <= ii) {
                                        unsigned char *buf0, *buf1;
                                        int ps;
                                        ps = line_size_max;
                                        line_size_max += 1024;
                                        buf0 = new unsigned char[line_size_max + 2];
                                        buf1 = new unsigned char[line_size_max + 2];
                                        memcpy(buf0, line0, ps);
                                        memcpy(buf1, line1, ps);
                                        delete [] line0;
                                        delete [] line1;
                                        line0 = buf0;
                                        line1 = buf1;
                                    }
                                    iii++;
                                }
                                l++;
                            }
                        }
                    }
                } else if (before_line[i] == '[') {
                    stk0++;
                    if (stk0 != 1) {
                        line[ii] = before_line[i];
                        ii++;
                        if (line_size_max <= ii) {
                            unsigned char *buf0, *buf1;
                            int ps;
                            ps = line_size_max;
                            line_size_max += 1024;
                            buf0 = new unsigned char[line_size_max + 2];
                            buf1 = new unsigned char[line_size_max + 2];
                            memcpy(buf0, line0, ps);
                            memcpy(buf1, line1, ps);
                            delete [] line0;
                            delete [] line1;
                            line0 = buf0;
                            line1 = buf1;
                        }
                        i++;
                        continue;
                    }
                    i++;
                } else if (before_line[i] == ']') {
                    int iii, iii_start, iii_end, l, loop, stk;
                    if (stk0 != 1) {
                        line[ii] = before_line[i];
                        ii++;
                        if (line_size_max <= ii) {
                            unsigned char *buf0, *buf1;
                            int ps;
                            ps = line_size_max;
                            line_size_max += 1024;
                            buf0 = new unsigned char[line_size_max + 2];
                            buf1 = new unsigned char[line_size_max + 2];
                            memcpy(buf0, line0, ps);
                            memcpy(buf1, line1, ps);
                            delete [] line0;
                            delete [] line1;
                            line0 = buf0;
                            line1 = buf1;
                        }
                        i++;
                        stk0--;
                        if ('0' <= before_line[i] && before_line[i] <= '9') {
                            line[ii] = before_line[i];
                            ii++;
                            if (line_size_max <= ii) {
                                unsigned char *buf0, *buf1;
                                int ps;
                                ps = line_size_max;
                                line_size_max += 1024;
                                buf0 = new unsigned char[line_size_max + 2];
                                buf1 = new unsigned char[line_size_max + 2];
                                memcpy(buf0, line0, ps);
                                memcpy(buf1, line1, ps);
                                delete [] line0;
                                delete [] line1;
                                line0 = buf0;
                                line1 = buf1;
                            }
                            i++;
                            if ('0' <= before_line[i] && before_line[i] <= '9') {
                                line[ii] = before_line[i];
                                ii++;
                                if (line_size_max <= ii) {
                                    unsigned char *buf0, *buf1;
                                    int ps;
                                    ps = line_size_max;
                                    line_size_max += 1024;
                                    buf0 = new unsigned char[line_size_max + 2];
                                    buf1 = new unsigned char[line_size_max + 2];
                                    memcpy(buf0, line0, ps);
                                    memcpy(buf1, line1, ps);
                                    delete [] line0;
                                    delete [] line1;
                                    line0 = buf0;
                                    line1 = buf1;
                                }
                                i++;
                                if ('0' <= before_line[i] && before_line[i] <= '9') {
                                    delete [] line0;
                                    delete [] line1;
                                    delete [] midi_session.notes;
                                    return MIDI;
                                }
                            }
                        }
                        continue;
                    }
                    stk0--;
                    i++;
                    iii_end = iii = i-2;
                    stk = 0;
                    while (0 <= iii) {
                        if (before_line[iii] == ']') {
                            stk++;
                        }
                        if (before_line[iii] == '[') {
                            if (stk == 0) {
                                iii++;
                                break;
                            } else {
                                stk--;
                            }
                        }
                        iii--;
                    }
                    iii_start = iii;
                    loop = 1;
                    if ('0' <= before_line[i] && before_line[i] <= '9') {
                        loop = before_line[i]-'0';
                        i++;
                        if ('0' <= before_line[i] && before_line[i] <= '9') {
                            loop=loop*10+before_line[i]-'0';
                            i++;
                            if ('0' <= before_line[i] && before_line[i] <= '9') {
                                delete [] line0;
                                delete [] line1;
                                delete [] midi_session.notes;
                                return MIDI;
                            }
                        }
                    }
                    l = 1;
                    while (l < loop) {
                        iii = iii_start;
                        stk=0;
                        while(iii <= iii_end) {
                            if (before_line[iii] == '[') {
                                stk++;
                            }
                            if (before_line[iii] == ']') {
                                if (stk == 0) {
                                    break;
                                } else {
                                    stk--;
                                }
                            }
                            line[ii] = before_line[iii];
                            ii++;
                            if (line_size_max <= ii) {
                                unsigned char *buf0, *buf1;
                                int ps;
                                ps = line_size_max;
                                line_size_max += 1024;
                                buf0 = new unsigned char[line_size_max + 2];
                                buf1 = new unsigned char[line_size_max + 2];
                                memcpy(buf0, line0, ps);
                                memcpy(buf1, line1, ps);
                                delete [] line0;
                                delete [] line1;
                                line0 = buf0;
                                line1 = buf1;
                            }
                            iii++;
                        }
                        l++;
                    }
                } else {
                    line[ii] = before_line[i];
                    ii++;
                    if (line_size_max <= ii) {
                        unsigned char *buf0, *buf1;
                        int ps;
                        ps = line_size_max;
                        line_size_max += 1024;
                        buf0 = new unsigned char[line_size_max + 2];
                        buf1 = new unsigned char[line_size_max + 2];
                        memcpy(buf0, line0, ps);
                        memcpy(buf1, line1, ps);
                        delete [] line0;
                        delete [] line1;
                        line0 = buf0;
                        line1 = buf1;
                    }
                    i++;
                }
            }
            textpos_max = ii;
            line[textpos_max] = '\0';
            if (strchr((char *)line, '$') == NULL && strchr((char *)line, '[') == NULL && strchr((char *)line, ']') == NULL) {
                break;
            } else {
                if (textlineloop < 255) {
                    if (line == line0) {
                        line = line1;
                        before_line = line0;
                    } else {
                        line = line0;
                        before_line = line1;
                    }
                } else {
                    delete [] line0;
                    delete [] line1;
                    delete [] midi_session.notes;
                    return MIDI;
                }
            }
        }

        midi_session.waon_kakko = 0;
        textpos = 0;
        while (textpos < textpos_max) {
            if (notes_size_max <= midi_session.notepos+15) {
                unsigned char *buf;
                int ps;
                ps = notes_size_max;
                notes_size_max += 1024;
                buf = new unsigned char[notes_size_max + 2];
                memcpy(buf, midi_session.notes, ps);
                delete [] midi_session.notes;
                midi_session.notes = buf;
            }
            if (isnote(line[textpos]) == 0) {
                delete [] line0;
                delete [] line1;
                delete [] midi_session.notes;
                return MIDI;
            }
            if (textpos+1 < textpos_max) {
                if (isnote(line[textpos+1])) {
                    textpos = _setnote(&midi_session, &line[0], textpos, 1);
                } else {
                    if (textpos+2 < textpos_max) {
                        if (isnote(line[textpos+2])) {
                            textpos = _setnote(&midi_session, &line[0], textpos, 2);
                        } else {
                            if (textpos+3 < textpos_max) {
                                if (isnote(line[textpos+3])) {
                                    textpos = _setnote(&midi_session, &line[0], textpos, 3);
                                } else {
                                    if (textpos+4 < textpos_max) {
                                        if (isnote(line[textpos+4])) {
                                            textpos = _setnote(&midi_session, &line[0], textpos, 4);
                                        } else {
                                            if (textpos+5 < textpos_max) {
                                                if (isnote(line[textpos+5])) {
                                                    textpos = _setnote(&midi_session, &line[0], textpos, 5);
                                                } else {
                                                    if (textpos+6 < textpos_max) {
                                                        if (isnote(line[textpos+6])) {
                                                            textpos = _setnote(&midi_session, &line[0], textpos, 6);
                                                        } else {
                                                            if (textpos+7 < textpos_max) {
                                                                if (isnote(line[textpos+7])) {
                                                                    textpos = _setnote(&midi_session, &line[0], textpos, 7);
                                                                } else {
                                                                    if (textpos+8 < textpos_max) {
                                                                        if (isnote(line[textpos+8])) {
                                                                            textpos = _setnote(&midi_session, &line[0], textpos, 8);
                                                                        } else {
                                                                            delete [] line0;
                                                                            delete [] line1;
                                                                            delete [] midi_session.notes;
                                                                            return MIDI;
                                                                        }
                                                                    } else {
                                                                        textpos = _setnote(&midi_session, &line[0], textpos, 8);
                                                                    }
                                                                }
                                                            } else {
                                                                textpos = _setnote(&midi_session, &line[0], textpos, 7);
                                                            }
                                                        }
                                                    } else {
                                                        textpos = _setnote(&midi_session, &line[0], textpos, 6);
                                                    }
                                                }
                                            } else {
                                                textpos = _setnote(&midi_session, &line[0], textpos, 5);
                                            }
                                        }
                                    } else {
                                        textpos = _setnote(&midi_session, &line[0], textpos, 4);
                                    }
                                }
                            } else {
                                textpos = _setnote(&midi_session, &line[0], textpos, 3);
                            }
                        }
                    } else {
                        textpos = _setnote(&midi_session, &line[0], textpos, 2);
                    }
                }
            } else {
                textpos = _setnote(&midi_session, &line[0], textpos, 1);
            }
        }
        if (midi_session.waon_kakko != 0) {
            delete [] line0;
            delete [] line1;
            delete [] midi_session.notes;
            return MIDI;
        }
        memcpy(midi_session.notes+midi_session.notepos, eot, 3);
        midi_session.notepos += 3;
        midi_session.notes[current_line_start_pos+6] = (unsigned char)(((midi_session.notepos-current_line_start_pos-8)/256) % 256);
        midi_session.notes[current_line_start_pos+7] = (unsigned char)((midi_session.notepos-current_line_start_pos-8) %256);
    }

    midi_session.head[HEAD_TRACK]   = (unsigned char)(((max_track-xtrack+1)/256)%256);
    midi_session.head[HEAD_TRACK+1] = (unsigned char)((max_track-xtrack+1)%256);

    MIDI->setBinary((const char *)midi_session.head, HEAD_LENGTH);
    MIDI->addBinary((const char *)midi_session.notes, midi_session.notepos);

    delete [] line0;
    delete [] line1;
    delete [] midi_session.notes;

    return MIDI;
}


static int _setnote(TMIDISession *midi_session, const unsigned char *track, const int textpos, const int textlen) {
    int L, V, H, N, REV, textpos1, i;
    char lb[7];

    textpos1 = textpos+1;
    L = abs(midi_session->def_L);
    V = midi_session->def_V;
    H = 0; // "."
    if (0 < midi_session->def_L) {
        REV = 0;
    } else {
        REV = 1; // L1==L16 && L4==L4
    }

    N = (midi_session->def_O+1)*12;

    // note number (12*(octave+1) ex. C4=12*(4+1)=60=0x3c)
    switch(track[textpos]) {
        case 'c':
        case 'C':
            N += 0;
            break;
        case 'd':
        case 'D':
            N += 2;
            break;
        case 'e':
        case 'E':
            N += 4;
            break;
        case 'f':
        case 'F':
            N += 5;
            break;
        case 'g':
        case 'G':
            N += 7;
            break;
        case 'a':
        case 'A':
            N += 9;
            break;
        case 'b':
        case 'B':
            N += 11;
            break;
    }

    // "#"
    if (textpos1-textpos < textlen && (track[textpos1] == '#' || track[textpos1] == '+' || track[textpos1] == 's' || track[textpos1] == 'S')) {
        N++;
        textpos1++;
    }

    // flat
    if (textpos1-textpos < textlen && (track[textpos1] == '-' || track[textpos1] == '_')) {
        if (0 < N) {
            N--;
        }
        textpos1++;
    }

    // reverse note length exp.
    if (textpos1-textpos<textlen && track[textpos1]=='~') {
        REV = 1;
        textpos1++;
    }

    if (textpos1-textpos < textlen && '0' <= track[textpos1] && track[textpos1] <= '9') {
        i = 0;
        while(i+textpos1-textpos < textlen && '0' <= track[textpos1+i] && track[textpos1+i] <= '9') {
            lb[i]=track[textpos1 + i];
            i++;
        }
        if (0 < i) {
            if (3 < i) {
                return 0;
            }
            textpos1 += i;
            lb[i] = '\0';
            L = atoi(lb);
            if (L < 0 || 255 < L) {
                return 0;
            }
        }
    }

    if (textpos1-textpos<textlen && track[textpos1]=='.') {
        H = 1;
        textpos1++;
    }

    if (textpos1-textpos < textlen) {
        return 0;
    }

    // note number (12*(octave+1) ex. C4=12*(4+1)=60=0x3c)
    switch (track[textpos]) {
        case 'c':
        case 'C':
        case 'd':
        case 'D':
        case 'e':
        case 'E':
        case 'f':
        case 'F':
        case 'g':
        case 'G':
        case 'a':
        case 'A':
        case 'b':
        case 'B':
            if (L < 1) {
                return 0;
            }
            midi_session->notes[midi_session->notepos+0] = (0xff & N);
            midi_session->notes[midi_session->notepos+1] = V;
            if (midi_session->waon_kakko == 0) {
                if (((REV == 0)?(int)((480/L)*((H == 0)?1.0:1.5)):(int)(30*L*((H == 0)?1.0:1.5)))<=0x7f) { // 1 byte
                    midi_session->notes[midi_session->notepos+2] = ((REV == 0)?(int)((480/L)*((H == 0)?1.0:1.5)):(int)(30*L*((H == 0)?1.0:1.5)));
                    midi_session->notes[midi_session->notepos+3] = midi_session->notes[midi_session->notepos+0];
                    midi_session->notes[midi_session->notepos+4] = 0x00;
                    midi_session->notes[midi_session->notepos+5] = 0x00;
                    midi_session->intervalpos = midi_session->notepos+5;
                    midi_session->notepos += 6;
                } else { // 2 bytes
                    midi_session->notes[midi_session->notepos+2] = 0x80+(((REV == 0)?(int)((480/L)*((H == 0)?1.0:1.5)):(int)(30*L*((H == 0)?1.0:1.5)))/128);
                    midi_session->notes[midi_session->notepos+3] = 0x7f & ((REV == 0)?(int)((480/L)*((H == 0)?1.0:1.5)):(int)(30*L*((H == 0)?1.0:1.5)));
                    midi_session->notes[midi_session->notepos+4] = midi_session->notes[midi_session->notepos+0];
                    midi_session->notes[midi_session->notepos+5] = 0x00;
                    midi_session->notes[midi_session->notepos+6] = 0x00;
                    midi_session->intervalpos = midi_session->notepos+6;
                    midi_session->notepos += 7;
                }
            } else {
                midi_session->notes[midi_session->notepos+2] = 0x00;

                /* for end of the waon */
                midi_session->notes[midi_session->notepos+3] = midi_session->notes[midi_session->notepos+0];
                midi_session->notes[midi_session->notepos+4] = 0x00;
                midi_session->notes[midi_session->notepos+5] = 0x00;

                midi_session->notepos += 3;
            }
            break;
        case 'n':
        case 'N':
        case 'r':
        case 'R':
            if (L < 1 || midi_session->waon_kakko != 0) {
                return 0;
            }
            if (127 < midi_session->notes[midi_session->intervalpos]) { // 2 bytes to 2 bytes
                midi_session->notes[midi_session->intervalpos+0] += ((((REV == 0)?(int)((480/L)*((H == 0)?1.0:1.5)):(int)(30*L*((H == 0)?1.0:1.5)))+midi_session->notes[midi_session->intervalpos+1])/128);
                midi_session->notes[midi_session->intervalpos+1] = 0x7f & (midi_session->notes[midi_session->intervalpos+1]+((REV == 0)?(int)((480/L)*((H == 0)?1.0:1.5)):(int)(30*L*((H == 0)?1.0:1.5))));
                if (midi_session->intervalpos != midi_session->firstinterval) {
                    midi_session->notes[midi_session->intervalpos+2] = 0x00;
                }
            } else if (midi_session->notes[midi_session->intervalpos]+((REV == 0)?(int)((480/L)*((H == 0)?1.0:1.5)):(int)(30*L*((H == 0)?1.0:1.5))) <= 127) { /* 1 byte to 1 byte */
                midi_session->notes[midi_session->intervalpos] += ((REV==0)?(int)((480/L)*((H==0)?1.0:1.5)):(int)(30*L*((H == 0)?1.0:1.5)));
            } else { /* 1 byte to 2 bytes */
                midi_session->notes[midi_session->intervalpos+3] = midi_session->notes[midi_session->intervalpos+2];
                midi_session->notes[midi_session->intervalpos+2] = midi_session->notes[midi_session->intervalpos+1];
                if (midi_session->intervalpos != midi_session->firstinterval) {
                    midi_session->notes[midi_session->intervalpos+2]=0x00;
                }
                midi_session->notes[midi_session->intervalpos+1] = midi_session->notes[midi_session->intervalpos+0];
                midi_session->notes[midi_session->intervalpos+0] = 0x80+((((REV == 0)?(int)((480/L)*((H == 0)?1.0:1.5)):(int)(30*L*((H == 0)?1.0:1.5)))+midi_session->notes[midi_session->intervalpos+1])/128);
                midi_session->notes[midi_session->intervalpos+1] = 0x7f & (midi_session->notes[midi_session->intervalpos+1]+((REV == 0)?(int)((480/L)*((H == 0)?1.0:1.5)):(int)(30*L*((H == 0)?1.0:1.5))));
                midi_session->notepos++;
            }
            break;
        case '(':
            if (textlen != 1 || midi_session->waon_kakko != 0) {
                return 0;
            }
            midi_session->waon_kakko=1;
            break;
        case ')':
            if (L < 1 || midi_session->waon_kakko == 0) {
                return 0;
            }
            midi_session->waon_kakko = 0;
            if (((REV == 0)?(int)((480/L)*((H == 0)?1.0:1.5)):(int)(30*L*((H == 0)?1.0:1.5)))<=0x7f) { // 1 byte
                midi_session->notes[midi_session->notepos-1] = ((REV == 0)?(int)((480/L)*((H == 0)?1.0:1.5)):(int)(30*L*((H == 0)?1.0:1.5)));
                midi_session->notes[midi_session->notepos+1] = 0x00;
                midi_session->notes[midi_session->notepos+2] = 0x00;
                midi_session->intervalpos = midi_session->notepos + 2;
                midi_session->notepos += 3;
            } else { // 2 bytes
                midi_session->notes[midi_session->notepos+1] = midi_session->notes[midi_session->notepos+0];
                midi_session->notes[midi_session->notepos-1] = 0x80+(((REV == 0)?(int)((480/L)*((H == 0)?1.0:1.5)):(int)(30*L*((H == 0)?1.0:1.5)))/128);
                midi_session->notes[midi_session->notepos+0] = 0x7f & ((REV == 0)?(int)((480/L)*((H == 0)?1.0:1.5)):(int)(30*L*((H == 0)?1.0:1.5)));
                midi_session->notes[midi_session->notepos+2] = 0x00;
                midi_session->notes[midi_session->notepos+3] = 0x00;
                midi_session->intervalpos = midi_session->notepos + 3;
                midi_session->notepos += 4;
            }
            break;
        case 'o':
        case 'O':
            if (!(textlen == 2 && '0' <= track[textpos+1] && track[textpos+1] <= '9')) {
                return 0;
            }
            midi_session->def_O = track[textpos+1]-'0';
            break;
        case '>':
            if (textlen!=1) return 0;
            midi_session->def_O++;
            if (midi_session->def_O < 1 || 10 < midi_session->def_O) {
                return 0;
            }
            break;
        case '<':
            if (textlen != 1) {
                return 0;
            }
            midi_session->def_O--;
            if (midi_session->def_O < 1 || 10 < midi_session->def_O) {
                return 0;
            }
            break;
        case 'l':
        case 'L':
            if (L <1) {
                return 0;
            }
            if (REV == 0) {
                midi_session->def_L = L;
            } else {
                midi_session->def_L = -L;
            }
            break;
        case 'v':
        case 'V':
            if (L<=127) {
                midi_session->def_V=L;
            } else {
                return 0;
            }
            break;
        case 'm':
        case 'M':
            if (1 <= L && L <= 16) {
                *midi_session->midi_channel = 0x90+L-1;
                midi_session->midi_channel[-3] = 0xc0+L-1;
                *midi_session->patch = midi_session->def_Patch[(*midi_session->midi_channel)-0x90];
                midi_session->trkhead[12] = *midi_session->midi_channel;
                midi_session->trkhead[9] = midi_session->midi_channel[-3];
            } else {
                return 0;
            }
            break;
        case '@':
            if (1 <= L && L <= 128) {
                *midi_session->patch = L-1;
                midi_session->def_Patch[(*midi_session->midi_channel)-0x90] = *midi_session->patch;
            } else {
                return 0;
            }
            break;
        case 't':
        case 'T':
            if (1 <= L && L <= 250) {
                unsigned long tempo;
                tempo = 60000000/L;
                midi_session->head[HEAD_TEMPO]   = (unsigned char)((tempo/65536)%256);
                midi_session->head[HEAD_TEMPO+1] = (unsigned char)((tempo/128)%256);
                midi_session->head[HEAD_TEMPO+2] = (unsigned char)(tempo%256);
            } else {
                return 0;
            }
            break;
        default:
            break;
    }

    return textpos + textlen;
}


} // namespace apolloron
