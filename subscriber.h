#ifndef PUBSUB_SUBSCRIBER_H_
#define PUBSUB_SUBSCRIBER_H_

#include <stdint.h>

#include "message.h"

struct SubscriberContext {
  // The key we're listening for (null-terminated).
  char *key; 

  // Socket for notifications from the broker.
  int socket;
};

// Subscribes to notifications about `key` from the broker at `broker_socket`.
// 'broker_socket` and 'key' must both be null-terminated.
// On success returns zero and fills in `ctx`.
// Retrurns -errno if there was an error.
int Subscribe(char *broker_socket, char *key, struct SubscriberContext *ctx);

// Unsubscribe from notifications from the broker/key described by `ctx` and
// closes `ctx`.socket 
int Unsubscribe(struct SubscriberContext *ctx);

// On success returns zero and stores the received update in `*msg`. 
// Returns -EAGAIN if there were no updates or -errno if there was an error.  
int GetUpdate(const struct SubscriberContext *ctx, struct Message *msg);

#endif // PUBSUB_SUBSCRIBER_H_
