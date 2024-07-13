/*
 * Copyright (C) 2019  Giuseppe Fabio Nicotra <artix2 at gmail dot com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "config.h"
#include "sds.h"
#include "zmalloc.h"
#include "logger.h"

#define CONFIG_MAX_LINE 1024
extern redisClusterProxyConfig config;
void initConfig(void) {
    config.entry_points_count = 0;
    config.port = DEFAULT_PORT;
    config.unixsocket = NULL;
    config.unixsocketperm = DEFAULT_UNIXSOCKETPERM;
    config.tcpkeepalive = DEFAULT_TCP_KEEPALIVE;
    config.maxclients = DEFAULT_MAX_CLIENTS;
    config.num_threads = DEFAULT_THREADS;
    config.tcp_backlog = DEFAULT_TCP_BACKLOG;
    config.daemonize = 0;
    config.loglevel = LOGLEVEL_INFO;
    config.use_colors = 0;
    config.dump_queries = 0;
    config.dump_buffer = 0;
    config.dump_queues = 0;
    config.auth = NULL;
    config.auth_user = NULL;
    config.cross_slot_enabled = 0;
    config.bindaddr_count = 0;
    config.pidfile = NULL;
    config.logfile = NULL;
    config.connections_pool.size = DEFAULT_CONNECTIONS_POOL_SIZE;
    config.connections_pool.min_size = DEFAULT_CONNECTIONS_POOL_MINSIZE;
    config.connections_pool.spawn_every = DEFAULT_CONNECTIONS_POOL_INTERVAL;
    config.connections_pool.spawn_rate = DEFAULT_CONNECTIONS_POOL_SPAWNRATE;
}

int parseAddress(char *address, redisClusterEntryPoint *entry_point) {
    entry_point->host = NULL;
    entry_point->socket = NULL;
    entry_point->port = 0;
    entry_point->address = zstrdup(address);
    int size = strlen(address);
    char *p = strchr(address, ':');
    if (!p) entry_point->socket = zstrdup(address);
    else {
        if (p == address) entry_point->host = zstrdup("localhost");
        else {
            *p = '\0';
            entry_point->host = zstrdup(address);
        }
        if (p - address != size) entry_point->port = atoi(++p);
        if (!entry_point->port) return 0;
    }
    return 1;
}


void checkConfig(void) {
    if (config.logfile != NULL) {
        /* If the logfile is an empty string, set it NULL and use STDOUT */
        if (config.logfile[0] == '\0') {
            zfree(config.logfile);
            config.logfile = NULL;
        } else config.use_colors = 0;
    }
    if (config.connections_pool.size > MAX_POOL_SIZE) {
        config.connections_pool.size = MAX_POOL_SIZE;
        proxyLogWarn("Limiting connections-pool-size to max. %d\n",
            MAX_POOL_SIZE);
    }
    if (config.connections_pool.min_size > config.connections_pool.size)
        config.connections_pool.min_size = config.connections_pool.size;
    if (config.connections_pool.spawn_every < 0)
        config.connections_pool.spawn_every = 0;
}
