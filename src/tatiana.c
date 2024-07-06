#include "ae.h"
#include "anet.h"
#include "sds.h"
#include "hiredis/hiredis.h"
#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int installIOHandler(aeEventLoop *eventLoop, int fd, int mask, aeFileProc *proc, void *clientData, int retried){
  if(aeCreateFileEvent(eventLoop,  fd,  mask,  proc, clientData) != AE_ERR){
    return 1;
  }else{
    if(!retried){
      return installIOHandler(eventLoop,  fd,  mask,  *proc,  clientData,  retried);
    }
  }
}
void writeToRedis(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask){
  
}
int main() {
  char err_msg[1024];
  const char *addr = "127.0.0.1";
  int port = 6379;
  redisContext *ctx = redisConnectNonBlock(addr, port);
  aeEventLoop* loop = aeCreateEventLoop(1024);
  installIOHandler(loop,  ctx->fd,  AE_WRITABLE,  writeToRedis, NULL, 0);
  return 0;
}
