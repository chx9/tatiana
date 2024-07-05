#ifndef __TATIANA_H__
#define __TATIANA_H__
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <hiredis.h>
#include "ae.h"
#include "cluster.h"
#include "adlist.h"
#include "sds.h"
#include <stddef.h>
#include <stdint.h>
#include "rax.h"
#include <sys/types.h>
#include "cluster.h"
#include "anet.h"
#include "config.h"
#define TATIANA_VERSION "0.0.1"
#define UNUSED(V) ((void)V)
typedef struct workThread{
   int thread_id;
   int io[2];
   pthread_t thread;
   redisCluster *cluster;
   aeEventLoop *loop;
   list *client; 
   list *unlinked_clients;
   list *connection_pool;
   int is_spawning_connections;
   u_int64_t next_client_id;
   _Atomic u_int64_t processed_clients;
   sds msg_buffer;   
}workThread;

typedef struct Tanya{
   aeEventLoop *main_loop;
   int fds[BINDADDR_MAX+1];
   int fd_count;
   int tcp_backlog;
   char neterr[ANET_ERR_LEN];
   workThread **threads;
   _Atomic u_int64_t num_clients;
   rax *commands;
   int min_reserved_fds;
   time_t start_time;
   sds config_file;
   size_t system_memory_size;
   pthread_t main_thread;
   _Atomic int exit_asap;
}Tanya;
// int getCurrentThreadID(void);

#endif