#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../audio/audio.h"

/* Plays a sound file using fork() + exec() so the game doesn't block.
   The child process runs afplay (macOS) and exits on its own. */
void audio_play(const char *filepath)
{
    pid_t pid;

    if (filepath == (void *)0)
        return;

    pid = fork();
    if (pid == 0)
    {
        /* Child process: redirect stderr to /dev/null to suppress errors */
        close(STDERR_FILENO);
        execlp("afplay", "afplay", filepath, (char *)0);
        /* If exec fails, exit the child silently */
        _exit(1);
    }
    /* Parent returns immediately - child plays sound in background */
    /* Reap any finished child processes to avoid zombies */
    if (pid > 0)
        waitpid(-1, (void *)0, WNOHANG);
}

void audio_beep(void)
{
    printf("\a");
    fflush(stdout);
}
