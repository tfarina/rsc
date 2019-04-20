#ifndef ED_INSTANCE_H_
#define ED_INSTANCE_H_

#include <unistd.h>

/** \struct instance
 * @brief An instance of Echo daemon.
 */
typedef struct ed_instance_s {
  pid_t pid;          /* process id */
} ed_instance_t;

void ed_instance_init(ed_instance_t *instance);

#endif  /* ED_INSTANCE_H_ */
