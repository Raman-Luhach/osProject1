#ifndef CUSTOM_AUDIO_H
#define CUSTOM_AUDIO_H

/* Play a sound file in a child process (non-blocking using fork+exec) */
void audio_play(const char *filepath);

/* Play the terminal bell */
void audio_beep(void);

#endif
