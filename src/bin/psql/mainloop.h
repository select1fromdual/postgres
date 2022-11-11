
#ifndef MAINLOOP_H
#define MAINLOOP_H

#include "psqlf.h"
extern const PsqlScanCallbacks psqlscan_callbacks;

extern int MainLoop(FILE *source);

#endif /* MAINLOOP_H */
