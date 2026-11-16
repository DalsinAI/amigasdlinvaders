/* The Amiga port's start: the stack, the version string, and the drawer
 * the game reads its data/ from.
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like SDLInvaders. */
#include <stdlib.h>
#include <string.h>
#include <exec/types.h>
#include <dos/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include "amiga_start.h"

/* A Shell gives 4 KB of stack; SDL wants more. libnix's startup sets this
 * much up. */
unsigned long __stack = 128 * 1024;
static const char version[] __attribute__((used)) =
    "$VER: SDLInvaders 0.7.6 (16.11.2026)";

static BPTR old_dir;
static int dir_changed;

static void amiga_restore_dir(void)
{
    if (dir_changed)
    {
        BPTR mine = CurrentDir(old_dir);
        UnLock(mine);
        dir_changed = 0;
    }
}

/* Upstream reads data/... from the current directory. A Workbench start
 * is already in the drawer; a Shell start may be anywhere, so the game
 * moves to its own drawer (PROGDIR:) and back at exit. The high scores
 * are kept in User/, which a GFOpenUp update leaves alone; the first
 * start copies upstream's table there. */
void amiga_setup(void)
{
    BPTR prog = GetProgramDir();
    if (prog)
    {
        BPTR dup = DupLock(prog);
        if (dup)
        {
            old_dir = CurrentDir(dup);
            dir_changed = 1;
            atexit(amiga_restore_dir);
        }
    }
    {
        BPTR l = Lock((CONST_STRPTR)"User", ACCESS_READ);
        if (l == 0)
            l = CreateDir((CONST_STRPTR)"User");
        if (l)
            UnLock(l);
    }
    {
        BPTR f = Lock((CONST_STRPTR)AMIGA_SCORES, ACCESS_READ);
        if (f)
            UnLock(f);
        else
        {
            BPTR in = Open((CONST_STRPTR)"data/highscores", MODE_OLDFILE);
            if (in)
            {
                BPTR out = Open((CONST_STRPTR)AMIGA_SCORES, MODE_NEWFILE);
                if (out)
                {
                    char buf[512];
                    LONG n;
                    while ((n = Read(in, buf, sizeof buf)) > 0)
                        Write(out, buf, n);
                    Close(out);
                }
                Close(in);
            }
        }
    }
}

/* LAB TRACE ONLY: not for the PR */
#include <stdio.h>
void si_trace(const char *s)
{
    BPTR f = Open((CONST_STRPTR)"PROGDIR:trace.txt", MODE_READWRITE);
    if (f) { Seek(f, 0, OFFSET_END); Write(f, (APTR)s, strlen(s)); Write(f, (APTR)"\n", 1); Close(f); }
}
