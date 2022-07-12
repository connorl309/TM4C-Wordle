// Music.h
// playing your favorite Song.
//
// For use with the TM4C123
// EE319K lab6 
// 1/11/22

// Description: 
// This file contains the interrupt delays for notes, the samples
// for the sine wave used to make sound, and the data structure
// containing notes for your favorite.
//

#ifndef MUSIC_H
#define MUSIC_H
// Initialize music player
void Music_Init(void);

// Play your favorite song, while button pushed or until end
void Music_PlaySong(void);

// Stop song
void Music_StopSong(void);

#endif

