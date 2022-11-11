/*
 * psql - the PostgreSQL interactive terminal
 *
 * Copyright (c) 2000-2022, PostgreSQL Global Development Group
 *
 * src/bin/psql/prompt.h
 */
#ifndef PROMPT_H
#define PROMPT_H

#include "psqlf.h"
/* enum promptStatus_t is now defined by psqlscan.h */

char	   *get_prompt(promptStatus_t status, ConditionalStack cstack);

#endif							/* PROMPT_H */
