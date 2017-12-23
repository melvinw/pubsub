#include "message.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int serialize_msg(char *buf, const struct Message *msg) {
  char *k = msg->key;
  char *v = msg->value;
  sprintf(buf, "%d %s %s", msg->message_type, (k != NULL) ? k : "", (v != NULL) ? v : "");
  return 0;
}

int deserialize_msg(struct Message *msg, const char *buf) {
  if (buf == NULL) {
    return -EINVAL;
  }

  int ret = sscanf(buf, "%d %s %s", (int*)&msg->message_type, msg->key, msg->value);
  if (ret < 3) {
    return -EINVAL;
  }
  return 0;
}

int copy_msg(struct Message *dst, const struct Message *src) {
  if (src == NULL || dst == NULL) {
    return -EINVAL;
  }
  size_t key_len = strlen(src->key);
  size_t val_len = strlen(src->value);
  dst->message_type = src->message_type;
  strncpy(dst->key, src->key, key_len);
  strncpy(dst->value, src->value, val_len);
  return 0;
}
