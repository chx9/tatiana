#include "ae.h"
#include "anet.h"
#include "config.h"
#include "hiredis/hiredis.h"
#include "logger.h"
#include "adlist.h"
#include "sds.h"
#include "util.h"
#include "zmalloc.h"
#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <errno.h>
#include <fcntl.h>
#include <hiredis/sds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct WorkThread {
  int id;
  int fd;
  list *clients;
} WorkThread;
typedef struct Proxy {
  int fd;
  WorkThread **workThreads;
  aeEventLoop *loop;
  int num_clients;
  char net_err[ANET_ERR_LEN];
} Proxy;
typedef struct Client{
  int fd;
  int port;
  sds ip;
  sds obuf;
  list requests;
}Client;
Proxy *proxy;
extern redisClusterProxyConfig config;
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
static WorkThread* createWorkThread(int i){
  WorkThread *workThread = (WorkThread*)zmalloc(sizeof(WorkThread));
  workThread->id = i;
  return workThread;
}
Client* createClient(int client_fd, char* client_ip, int client_port){
  proxyLogInfo("creating client from %s:%d", client_ip, client_port);
  Client* c = (Client*)zmalloc(sizeof(Client));
  c->fd = client_fd;
  c->ip = sdsnew(client_ip);
  c->port = client_port;

  proxy->num_clients++;
  return c;
}
void acceptTCP(int client_fd, char* client_ip, int client_port){
    Client* c = createClient(client_fd, client_ip, client_port); 
    
}
void accpetTCPHandler(aeEventLoop* loop, int fd, void* private_data, int mask){
    UNUSED(loop);
    UNUSED(mask);
    UNUSED(private_data);
    int client_port, client_fd; 
    char client_ip[NET_IP_STR_LEN];
    int max = MAX_ACCEPT;
    while(max--){
      client_fd = anetTcpAccept(proxy->net_err, fd, client_ip, sizeof(client_ip), &client_port);
      if(client_fd == ANET_ERR){
        if(errno != EWOULDBLOCK){
          proxyLogErr("Error accpeting new client:%s", proxy->net_err);
          return;
        }
      }
      acceptTCP(client_fd, client_ip, client_port);
    }
}

void initProxy() {
  proxy = (Proxy *)zmalloc(sizeof(Proxy));
  if (proxy == NULL) {
    proxyLogErr("cannot alloc memory for proxy");
    exit(1);
  }
  // net
  proxy->loop = aeCreateEventLoop(10 + 3 * config.num_threads);
  proxy->fd = anetTcpServer(proxy->net_err, config.port, NULL, config.tcp_backlog);
  if (strlen(proxy->net_err)){
    proxyLogErr("%s", proxy->net_err);
    exit(1);
  }
  installIOHandler(proxy->loop, proxy->fd, AE_READABLE, accpetTCPHandler, NULL, 0);
  // work thread
  proxy->workThreads = (WorkThread**)zmalloc(sizeof(WorkThread*) * config.num_threads);
  for(int i=0; i<config.num_threads; i++){
    proxy->workThreads[i] = createWorkThread(i);
  }
  
}

void initWorkThread() {
  proxy->workThreads = (WorkThread **)zmalloc(sizeof(WorkThread*) * config.num_threads);
  if (proxy->workThreads == NULL) {
    proxyLogErr("cannot alloc memory for workThreads");
    exit(1);
  }
}
// static int writeToCluster(aeEventLoop *eventLoop, int fd);
// static int installIOHandler(aeEventLoop *eventLoop, int fd, int mask,
//                             aeFileProc *proc, void *clientData, int retried);
// static int readFromCluster(aeEventLoop *eventLoop, int fd);


// static void readFromClusterHandler(struct aeEventLoop *eventLoop, int fd,
//                                    void *clientData, int mask) {
//   readFromCluster(eventLoop, fd);
// }

// static int readFromCluster(aeEventLoop *eventLoop, int fd) { return 0; }

// static void writeToClusterHandler(struct aeEventLoop *eventLoop, int fd,
//                                   void *clientData, int mask) {
//   if (!installIOHandler(eventLoop, fd, mask, readFromClusterHandler, clientData,
//                         0)) {
//     printf("installIOHandler failed\n");
//     return;
//   }
//   while (1) {
//     writeToCluster(eventLoop, fd);
//   }
// }
// static int writeToCluster(aeEventLoop *eventLoop, int fd) {
//   printf("writing to cluster..\n");
//   sds buf = sdsnew("ping\r\n");
//   size_t buflen = sdslen(buf);
//   int nwritten = 0;
// }
int main() {
  initConfig();
  initProxy();
  aeMain(proxy->loop);
  return 0;
}
