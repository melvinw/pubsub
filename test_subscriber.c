#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "message.h"
#include "subscriber.h"

int main(void) {
  struct SubscriberContext ctx;
  int ret = Subscribe(SERVER_SOCK_FILE, "foo", &ctx);
  if (ret) {
    return -ret;
  }

  struct Message msg;
  while ((ret = GetUpdate(&ctx, &msg)) == -EAGAIN) {
    sleep(1);
  }

  if (ret == 0) {
    printf("got update: %s\n", msg.value);
    free(msg.key);
    free(msg.value);
  }

  return (ret < 0) ? -ret : Unsubscribe(&ctx);
}
