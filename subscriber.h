#ifndef PUBSUB_SUBSCRIBER_H_
#define PUBSUB_SUBSCRIBER_H_

#include <string.h>

#include "message.h"

struct SubscriberContext {
  // The key we're listening for (null-terminated).
  char *key; 

  // Socket for notifications from the broker.
  int socket;
};

struct Update {
  size_t len;
  char *value;
};

// Subscribes to notifications about `key` from the broker at `broker_socket`.
// On success returns zero and fills in `ctx`.
// Retrurns -errno if there was an error.
int Subscribe(char *broker_socket, char *key, struct SubscriberContext *ctx);

// Unsubscribe from notifications from the broker/key described by `ctx`. Closes
// `ctx`.socket 
void UnSubscribe(struct SubscriberContext *ctx);

// On success returns zero and stores the update received in `*update`. 
// Returns -EAGAIN if there were no updates and -errno if there was an error.  
int GetUdpate(struct SubscriberContext *ctx, struct Update *update);

#endif // PUBSUB_SUBSCRIBER_H_
