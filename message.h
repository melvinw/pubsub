#ifndef PUBSUB_MESSAGE_H_
#define PUBSUB_MESSAGE_H_

#include <stdint.h>

struct Message {
  uint8_t mesage_type;
  uint32_t key_len;
  char *key;
  uint32_t value_len;
  char *value;
};

#endif // PUBSUB_MESSAGE_H_
