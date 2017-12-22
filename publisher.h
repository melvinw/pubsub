#ifndef PUBSUB_PUBLISHER_H_
#define PUBSUB_PUBLISHER_H_

// Publishes an update to (`key`, `value) to the broker at `broker_socket`.
// Key and value must both be null-terminated.
// Retrurns zero on success, -errno otherwise.
int Publish(char *broker_socket, char *key, char *value);

#endif // PUBSUB_PUBLISHER_H_
