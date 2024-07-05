#ifndef __TATIANA_CONFIG_H__
#define __TATIANA_CONFIG_H__
#define HAVE_BACKTRACE 1
#define PROXY_MAIN_THREAD_ID -1
#define PROXY_UNKN_THREAD_ID -999
#define CFG_DISABLE_MULTIPLEXING_AUTO       1
#define CFG_DISABLE_MULTIPLEXING_ALWAYS     2
#define DEFAULT_PORT 7777
#define DEFAULT_TCP_KEEPALIVE 300
#define DEFAULT_TCP_BACKLOG 511
#define DEFAULT_NUM_THREADS 8
#define BINDADDR_MAX 16
#define MAX_THREADS 512
#define MAX_POOL_SIZE 50
#define CONFIG_MAX_LINE 1024
#define DEFAULT_PID_FILE                    "/var/run/redis-cluster-proxy.pid"
#define DEFAULT_PORT                        7777
#define DEFAULT_UNIXSOCKETPERM              0
#define DEFAULT_MAX_CLIENTS                 10000
#define DEFAULT_THREADS                     8
#define DEFAULT_TCP_KEEPALIVE               300
#define DEFAULT_TCP_BACKLOG                 511
#define DEFAULT_CONNECTIONS_POOL_SIZE       10
#define DEFAULT_CONNECTIONS_POOL_MINSIZE    10
#define DEFAULT_CONNECTIONS_POOL_INTERVAL   50
#define DEFAULT_CONNECTIONS_POOL_SPAWNRATE  2
#define MAX_ENTRY_POINTS 255
#define MAX_ENTRY_POINTS_WARN_MSG "You cannot use more than %d entry points, "\
                                  "skipping entry point '%s'"
#define THREAD_MSG_STOP                     1
typedef struct redisClusterEntryPoint {
    char *host;
    int port;
    char *socket;
    char *address;
} redisClusterEntryPoint;
typedef struct tatianaConfig{
    int port;
    char* cluster_address;
    char* entry_node_host;
    int *entry_node_socket;
    int entry_points_count;
    int tcp_keepalive;
    int max_clients;
    int num_threads;
    int tcp_backlog;
    int daemonize;
    int loglevel;
    int use_colors;
    int dump_queries;
    int dump_queues;
    int dump_buffer;
    int bindaddr_count;
    int disable_multiplexing;
    int cross_slot_enabled;
    char *bindaddr[BINDADDR_MAX];
    char* auth;
    char* pid_file;
    char* log_file;
    char* auth_user;
    struct {
        int size;
        int min_size;
        int spawn_every;
        int spawn_rate;
    } connections_pool;
    redisClusterEntryPoint entry_points[MAX_ENTRY_POINTS];
}tatianaConfig;

extern tatianaConfig config;

int parseOptionsFromFile(const char *filename);
int parseAddress(char *address, redisClusterEntryPoint *entry_point);
#endif