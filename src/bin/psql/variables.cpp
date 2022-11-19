/*
 * psql - the PostgreSQL interactive terminal
 *
 * Copyright (c) 2000-2022, PostgreSQL Global Development Group
 *
 * src/bin/psql/variables.c
 */

#include "variables.h"

VariableSpace::~VariableSpace() {
  for (auto &[key, val] : variable_) {
    if (val->value) free(val->value);
    free(key);
  }
  variable_.clear();
}

/*
 * Check whether a variable's name is allowed.
 *
 * We allow any non-ASCII character, as well as ASCII letters, digits, and
 * underscore.  Keep this in sync with the definition of variable_char in
 * psqlscan.l and psqlscanslash.l.
 */
static bool valid_variable_name(const char *name) {
  const unsigned char *ptr = (const unsigned char *)name;

  /* Mustn't be zero-length */
  if (*ptr == '\0') return false;

  while (*ptr) {
    if (IS_HIGHBIT_SET(*ptr) || strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                       "abcdefghijklmnopqrstuvwxyz"
                                       "_0123456789",
                                       *ptr) != nullptr)
      ptr++;
    else
      return false;
  }

  return true;
}

/*
 * Try to interpret "value" as a boolean value, and if successful,
 * store it in *result.  Otherwise don't clobber *result.
 *
 * Valid values are: true, false, yes, no, on, off, 1, 0; as well as unique
 * prefixes thereof.
 *
 * "name" is the name of the variable we're assigning to, to use in error
 * report if any.  Pass name == nullptr to suppress the error report.
 *
 * Return true when "value" is syntactically valid, false otherwise.
 */
bool ParseVariableBool(const char *value, const char *name, bool *result) {
  size_t len;
  bool valid = true;

  /* Treat "unset" as an empty string, which will lead to error below */
  if (value == nullptr) value = "";

  len = strlen(value);

  if (len > 0 && strncasecmp(value, "true", len) == 0)
    *result = true;
  else if (len > 0 && strncasecmp(value, "false", len) == 0)
    *result = false;
  else if (len > 0 && strncasecmp(value, "yes", len) == 0)
    *result = true;
  else if (len > 0 && strncasecmp(value, "no", len) == 0)
    *result = false;
  /* 'o' is not unique enough */
  else if (strncasecmp(value, "on", (len > 2 ? len : 2)) == 0)
    *result = true;
  else if (strncasecmp(value, "off", (len > 2 ? len : 2)) == 0)
    *result = false;
  else if (strcasecmp(value, "1") == 0)
    *result = true;
  else if (strcasecmp(value, "0") == 0)
    *result = false;
  else {
    /* string is not recognized; don't clobber *result */
    if (name) pg_log_error("unrecognized value \"%s\" for \"%s\": Boolean expected", value, name);
    valid = false;
  }
  return valid;
}

/*
 * Try to interpret "value" as an integer value, and if successful,
 * store it in *result.  Otherwise don't clobber *result.
 *
 * "name" is the name of the variable we're assigning to, to use in error
 * report if any.  Pass name == nullptr to suppress the error report.
 *
 * Return true when "value" is syntactically valid, false otherwise.
 */
bool ParseVariableNum(const char *value, const char *name, int *result) {
  char *end;
  long numval;

  /* Treat "unset" as an empty string, which will lead to error below */
  if (value == nullptr) value = "";

  errno = 0;
  numval = strtol(value, &end, 0);
  if (errno == 0 && *end == '\0' && end != value && numval == (int)numval) {
    *result = (int)numval;
    return true;
  } else {
    /* string is not recognized; don't clobber *result */
    if (name) pg_log_error("invalid value \"%s\" for \"%s\": integer expected", value, name);
    return false;
  }
}

/*
 * Set the variable named "name" to value "value",
 * or delete it if "value" is nullptr.
 *
 * Returns true if successful, false if not; in the latter case a suitable
 * error message has been printed, except for the unexpected case of
 * space or name being nullptr.
 */
bool VariableSpace::SetVariable(const char *name, const char *value) {
  if (!name) return false;

  if (!valid_variable_name(name)) {
    /* Deletion of non-existent variable is not an error */
    if (!value) return true;
    pg_log_error("invalid variable name: \"%s\"", name);
    return false;
  }

  if (auto kval = variable_.find(const_cast<char *>(name)); kval != variable_.end()) {
    /*
     * Found entry, so update, unless assign hook returns false.
     *
     * We must duplicate the passed value to start with.  This
     * simplifies the API for substitute hooks.  Moreover, some assign
     * hooks assume that the passed value has the same lifespan as the
     * variable.  Having to free the string again on failure is a
     * small price to pay for keeping these APIs simple.
     */
    auto val = kval->second;
    char *new_value = value ? strdup(value) : nullptr;
    bool confirmed;

    if (val->substitute_hook) new_value = val->substitute_hook(new_value);

    if (val->assign_hook)
      confirmed = val->assign_hook(new_value);
    else
      confirmed = true;

    if (confirmed) {
      free(val->value);
      val->value = new_value;

      /*
       * If we deleted the value, and there are no hooks to
       * remember, we can discard the variable altogether.
       */
      if (new_value == nullptr && val->substitute_hook == nullptr && val->assign_hook == nullptr) {
        free(val);
        variable_.erase(kval);
      }
    } else
      free(new_value); /* current->value is left unchanged */

    return confirmed;
  }

  /* not present, make new entry ... unless we were asked to delete */
  if (value) variable_[strdup(name)] = new _variable(strdup(value), nullptr, nullptr);
  return true;
}

/*
 * Attach substitute and/or assign hook functions to the named variable.
 * If you need only one hook, pass nullptr for the other.
 *
 * If the variable doesn't already exist, create it with value nullptr, just so
 * we have a place to store the hook function(s).  (The substitute hook might
 * immediately change the nullptr to something else; if not, this state is
 * externally the same as the variable not being defined.)
 *
 * The substitute hook, if given, is immediately called on the variable's
 * value.  Then the assign hook, if given, is called on the variable's value.
 * This is meant to let it update any derived psql state.  If the assign hook
 * doesn't like the current value, it will print a message to that effect,
 * but we'll ignore it.  Generally we do not expect any such failure here,
 * because this should get called before any user-supplied value is assigned.
 */
void VariableSpace::SetVariableHooks(const char *name, VariableSubstituteHook shook, VariableAssignHook ahook) {
  if (!name) return;

  if (!valid_variable_name(name)) return;

  if (auto kval = variable_.find(const_cast<char *>(name)); kval != variable_.end()) {
    /* found entry, so update */
    auto val = kval->second;
    val->substitute_hook = shook;
    val->assign_hook = ahook;
    if (shook) val->value = shook(val->value);
    if (ahook) ahook(val->value);
    return;
  }

  auto key = strdup(name);
  variable_[key] = new _variable(nullptr, shook, ahook);

  if (shook) variable_[key]->value = shook(variable_[key]->value);
  if (ahook) ahook(variable_[key]->value);
}

/*
 * Return true iff the named variable has substitute and/or assign hook
 * functions.
 */
bool VariableSpace::VariableHasHook(const char *name) {
  Assert(name);

  if (auto kval = variable_.find(const_cast<char *>(name)); kval != variable_.end()) {
    return (kval->second->substitute_hook != nullptr || kval->second->assign_hook != nullptr);
  }

  return false;
}

/*
 * Convenience function to set a variable's value to "on".
 */
bool VariableSpace::SetVariableBool(const char *name) { return SetVariable(name, "on"); }

/*
 * Attempt to delete variable.
 *
 * If unsuccessful, print a message and return "false".
 * Deleting a nonexistent variable is not an error.
 */
bool VariableSpace::DeleteVariable(const char *name) { return SetVariable(name, nullptr); }

/*
 * Emit error with suggestions for variables or commands
 * accepting enum-style arguments.
 * This function just exists to standardize the wording.
 * suggestions should follow the format "fee, fi, fo, fum".
 */
void PsqlVarEnumError(const char *name, const char *value, const char *suggestions) {
  pg_log_error(
      "unrecognized value \"%s\" for \"%s\"\n"
      "Available values are: %s.",
      value, name, suggestions);
}
