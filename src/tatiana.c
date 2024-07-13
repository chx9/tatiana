#include "ae.h"
#include "anet.h"
#include "config.h"
#include "hiredis/hiredis.h"
#include "logger.h"
#include "sds.h"
#include "util.h"
#include "zmalloc.h"
#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct WorkThread {
  int id;
  int fd;
} Workthread;
typedef struct Proxy {
  int fd;
  Workthread *workThreads;
  aeEventLoop *loop;
  char net_err[ANET_ERR_LEN];
} Proxy;
Proxy *proxy;
extern redisClusterProxyConfig config;
void initProxy() {
  proxy = (Proxy *)zmalloc(sizeof(Proxy));
  if (proxy == NULL) {
    proxyLogErr("cannot alloc memory for proxy");
    exit(1);
  }
  // ae
  proxy->loop = aeCreateEventLoop(10 + 3 * config.num_threads);
  proxy->fd =
      anetTcpServer(proxy->net_err, config.port, NULL, config.tcp_backlog);
}
void initWorkThread() {
  proxy->workThreads =
      (Workthread *)zmalloc(sizeof(Workthread) * config.num_threads);
  if (proxy->workThreads == NULL) {
    proxyLogErr("cannot alloc memory for workThreads");
    exit(1);
  }
}
static int writeToCluster(aeEventLoop *eventLoop, int fd);
static int installIOHandler(aeEventLoop *eventLoop, int fd, int mask,
                            aeFileProc *proc, void *clientData, int retried);
static int readFromCluster(aeEventLoop *eventLoop, int fd);

static int installIOHandler(aeEventLoop *eventLoop, int fd, int mask,
                            aeFileProc *proc, void *clientData, int retried) {
  printf("installiohandler\n");
  UNUSED(proc);
  UNUSED(clientData);
  if (aeCreateFileEvent(eventLoop, fd, mask, proc, clientData) != AE_ERR) {
    return 1;
  } else {
    if (!retried) {
      return installIOHandler(eventLoop, fd, mask, *proc, clientData, retried);
    }
  }
}

static void readFromClusterHandler(struct aeEventLoop *eventLoop, int fd,
                                   void *clientData, int mask) {
  readFromCluster(eventLoop, fd);
}

static int readFromCluster(aeEventLoop *eventLoop, int fd) { return 0; }

static void writeToClusterHandler(struct aeEventLoop *eventLoop, int fd,
                                  void *clientData, int mask) {
  if (!installIOHandler(eventLoop, fd, mask, readFromClusterHandler, clientData,
                        0)) {
    printf("installIOHandler failed\n");
    return;
  }
  while (1) {
    writeToCluster(eventLoop, fd);
  }
}
static int writeToCluster(aeEventLoop *eventLoop, int fd) {
  printf("writing to cluster..\n");
  sds buf = sdsnew("ping\r\n");
  size_t buflen = sdslen(buf);
  int nwritten = 0;
}
int main() {
  // char err_msg[1024];
  // const char *addr = "127.0.0.1";
  // int port = 6379;
  // redisContext *ctx = redisConnectNonBlock(addr, port);
  // if(ctx == NULL){
  //   printf("connection failed!\n");
  // }else{
  //   printf("connected to redis\n");
  // }
  // aeEventLoop *loop = aeCreateEventLoop(1024);
  // installIOHandler(loop, ctx->fd, AE_WRITABLE, writeToClusterHandler, ctx,
  // 0); aeMain(loop);
  initProxy();
  initConfig();
  initWorkThread();
  return 0;
}
