#ifndef PUBSUB_MESSAGE_H_
#define PUBSUB_MESSAGE_H_

#include <stdint.h>
#include <inttypes.h>

#define MAX_MSG_LEN 256

enum msg_type {publish, subscribe, unsubscribe};

struct Message {
  enum msg_type message_type;
  char *key;
  char *value;
};

int serialize_msg(char *buf, const struct Message *msg);

int deserialize_msg(struct Message *msg, const char *buf);

int copy_msg(struct Message *dst, const struct Message *src);

#endif // PUBSUB_MESSAGE_H_
