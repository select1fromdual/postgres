/*
 * psql - the PostgreSQL interactive terminal
 *
 * Copyright (c) 2000-2022, PostgreSQL Global Development Group
 *
 * This implements a sort of variable repository.  One could also think of it
 * as a cheap version of an associative array.  Each variable has a string
 * name and a string value.  The value can't be NULL, or more precisely
 * that's not distinguishable from the variable being unset.
 *
 * src/bin/psql/variables.h
 */
#pragma once

#include <functional>
#include <unordered_map>
#include "psqlf.h"

/*
 * Variables can be given "assign hook" functions.  The assign hook can
 * prevent invalid values from being assigned, and can update internal C
 * variables to keep them in sync with the variable's current value.
 *
 * An assign hook function is called before any attempted assignment, with the
 * proposed new value of the variable (or with NULL, if an \unset is being
 * attempted).  If it returns false, the assignment doesn't occur --- it
 * should print an error message with pg_log_error() to tell the user why.
 *
 * When an assign hook function is installed with SetVariableHooks(), it is
 * called with the variable's current value (or with NULL, if it wasn't set
 * yet).  But its return value is ignored in this case.  The hook should be
 * set before any possibly-invalid value can be assigned.
 */
using VariableAssignHook = std::function<bool(const char *newval)>;

/*
 * Variables can also be given "substitute hook" functions.  The substitute
 * hook can replace values (including NULL) with other values, allowing
 * normalization of variable contents.  For example, for a boolean variable,
 * we wish to interpret "\unset FOO" as "\set FOO off", and we can do that
 * by installing a substitute hook.  (We can use the same substitute hook
 * for all bool or nearly-bool variables, which is why this responsibility
 * isn't part of the assign hook.)
 *
 * The substitute hook is called before any attempted assignment, and before
 * the assign hook if any, passing the proposed new value of the variable as a
 * malloc'd string (or NULL, if an \unset is being attempted).  It can return
 * the same value, or a different malloc'd string, or modify the string
 * in-place.  It should free the passed-in value if it's not returning it.
 * The substitute hook generally should not complain about erroneous values;
 * that's a job for the assign hook.
 *
 * When a substitute hook is installed with SetVariableHooks(), it is applied
 * to the variable's current value (typically NULL, if it wasn't set yet).
 * That also happens before applying the assign hook.
 */
using VariableSubstituteHook = std::function<char *(char *newval)>;

/*
 * Data structure representing one variable.
 *
 * Note: if value == NULL then the variable is logically unset, but we are
 * keeping the struct around so as not to forget about its hook function(s).
 */
struct _variable {
  char *value;
  VariableSubstituteHook substitute_hook;
  VariableAssignHook assign_hook;
  _variable(char *v, VariableSubstituteHook f1, VariableAssignHook f2)
      : value(v), substitute_hook(f1), assign_hook(f2) {}
};

using variable = std::unordered_map<char *, _variable *>;

class VariableSpace {
 private:
  variable variable_;

 public:
  // variables() = default;
  ~VariableSpace();

  const char *GetVariable(const char *name) { return variable_[const_cast<char *>(name)]->value; }
  void PrintVariables() {
    for (const auto &[key, value] : variable_) {
      if (value->value) printf("%s = '%s'\n", key, value->value);
      if (cancel_pressed) break;
    }
  }

  variable::iterator begin() { return variable_.begin(); }
  variable::iterator end() { return variable_.end(); }

  bool SetVariable(const char *name, const char *value);
  bool SetVariableBool(const char *name);
  bool DeleteVariable(const char *name);

  void SetVariableHooks(const char *name, VariableSubstituteHook shook, VariableAssignHook ahook);
  bool VariableHasHook(const char *name);
};

bool ParseVariableBool(const char *value, const char *name, bool *result);
bool ParseVariableNum(const char *value, const char *name, int *result);
void PsqlVarEnumError(const char *name, const char *value, const char *suggestions);
