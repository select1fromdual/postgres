/*
 * psql - the PostgreSQL interactive terminal
 *
 * Copyright (c) 2000-2022, PostgreSQL Global Development Group
 *
 * src/bin/psql/tab-complete.h
 */
#pragma once

#include "psqlf.h"
extern PQExpBuffer tab_completion_query_buf;

extern void initialize_readline(void);
