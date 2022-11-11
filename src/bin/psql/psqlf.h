#pragma once

extern "C" {

#include "libpq-fe.h"
#include "portability/instr_time.h"
#include "postgres_fe.h"
#include "common/logging.h"
#include "common/string.h"
#include "fe_utils/print.h"
#include "fe_utils/conditional.h"
#include "fe_utils/print.h"
#include "fe_utils/psqlscan.h"
#include "fe_utils/cancel.h"
#include "fe_utils/string_utils.h"
#include "fe_utils/mbprint.h"
#include "catalog/pg_am.h"
#include "catalog/pg_attribute_d.h"
#include "catalog/pg_cast_d.h"
#include "catalog/pg_default_acl_d.h"
#include "common/username.h"
#include "mb/pg_wchar.h"
#include "catalog/pg_am_d.h"
#include "catalog/pg_class_d.h"
#include "common/keywords.h"

struct PQExpBufferData;
struct pg_result;
struct pg_conn;
struct pg_cancel;
struct printQueryOpt;
struct PsqlScanStateData;
struct ConditionalStackData;
}

using PQExpBuffer = PQExpBufferData *;
using PGresult = pg_result;
using PGconn = pg_conn;
using Oid = unsigned int;
using PsqlScanState = PsqlScanStateData *;
using ConditionalStack = ConditionalStackData *;

#define IS_DIR_SEP(ch) ((ch) == '/')

#ifndef is_absolute_path
#define is_absolute_path(filename) (IS_DIR_SEP((filename)[0]))
#endif

static inline bool is_unixsock_path(const char *path) { return is_absolute_path(path) || path[0] == '@'; }