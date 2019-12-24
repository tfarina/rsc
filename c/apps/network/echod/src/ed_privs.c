#include "ed_privs.h"

#include <errno.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>

#include "ed_log.h"
#include "ed_rcode.h"

int ed_change_user(char const *username) {
  struct passwd *pw;
  gid_t gid;
  uid_t uid;
  int rc;

  pw = getpwnam(username);
  if (pw == NULL) {
    ed_log_error("cannot find user '%s' to switch to", username);
    return ED_ERROR;
  }

  gid = pw->pw_gid;
  uid = pw->pw_uid;

  rc = setresgid(gid, gid, gid);
  if (rc < 0) {
    ed_log_error("unable to set group id of %s: %s", username, strerror(errno));
    return ED_ERROR;
  }

  rc = setresuid(uid, uid, uid);
  if (rc < 0) {
    ed_log_error("unable to set user id of %s: %s", username, strerror(errno));
    return ED_ERROR;
  }

  return ED_OK;
}
