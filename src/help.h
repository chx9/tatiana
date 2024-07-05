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

#ifndef __REDIS_CLUSTER_PROXY_HELP_H__
#define __REDIS_CLUSTER_PROXY_HELP_H__

extern const char *proxyCommandHelp[];
extern const char *proxyCommandSubcommandClientHelp[];
extern const char *proxyCommandSubcommandClusterHelp[];
extern const char *proxyCommandSubcommandDebugtHelp[];
extern const char *mainHelpString;

void printHelp(void);

#endif /* __REDIS_CLUSTER_PROXY_HELP_H__ */
