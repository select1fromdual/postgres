#pragma once

#define USE_READLINE 1

#include <readline/readline.h>
#include <readline/history.h>

#include "psqlf.h"

extern char *gets_interactive(const char *prompt, PQExpBuffer query_buf);
extern char *gets_fromFile(FILE *source);

extern void initializeInput(int flags);

extern bool printHistory(const char *fname, unsigned short int pager);

extern void pg_append_history(const char *s, PQExpBuffer history_buf);
extern void pg_send_history(PQExpBuffer history_buf);
