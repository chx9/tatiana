#include "config.h"
#include "zmalloc.h"
#include "stdio.h"
#include "sds.h"
#include "logger.h"
#include <string.h>
// int parseAddress(char *address, redisClusterEntryPoint *entry_point) {
//     entry_point->host = NULL;
//     entry_point->socket = NULL;
//     entry_point->port = 0;
//     entry_point->address = zstrdup(address);
//     int size = strlen(address);
//     char *p = strchr(address, ':');
//     if (!p) entry_point->socket = zstrdup(address);
//     else {
//         if (p == address) entry_point->host = zstrdup("localhost");
//         else {
//             *p = '\0';
//             entry_point->host = zstrdup(address);
//         }
//         if (p - address != size) entry_point->port = atoi(++p);
//         if (!entry_point->port) return 0;
//     }
//     return 1;
// }
// int parseOptionsFromFile(const char *filename) {
//     FILE *f;
//     if (filename[0] == '-' || filename[0] == '\0') f = stdin;
//     else {
//         f = fopen(filename, "r");
//         if (f == NULL) {
//             fprintf(stderr, "Failed to open config file: '%s'\n", filename);
//             return 0;
//         }
//     }
//     int buflen = CONFIG_MAX_LINE + 1;
//     char buf[buflen];
//     int argc = 1, i = 0, linenum = 0, success = 1;
//     sds *argv = zmalloc(sizeof(sds));
//     success = (argv != NULL);
//     if (!success) goto cleanup;
//     /* Insert an empty string since parseOptions always starts from index 1 */
//     argv[0] = sdsempty();
//     while (fgets(buf, buflen, f) != NULL) {
//         linenum++;
//         int numtokens = 0;
//         sds *tokens = NULL;
//         sds line = sdstrim(sdsnew(buf),"\r\n\t ");
//         /* Search for comments */
//         char *comment_start = strchr(line, '#');
//         if (comment_start != NULL) *comment_start = '\0';
//         sdsupdatelen(line);
//         if (sdslen(line) == 0) goto next_line;
//         tokens = sdssplitargs(line, &numtokens);
//         if (tokens == NULL || numtokens == 0) {
//             // proxyLogWarn("Failed to parse line %d in config file '%s'",
//             //              linenum, filename);
//             goto next_line;
//         }
//         sdstolower(tokens[0]);
//         /* Ignore single char options (ie. 'p' for '-p')*/
//         if (sdslen(tokens[0]) <= 0) goto next_line;
//         int handled = 0;
//         if (strcmp("include", tokens[0]) == 0) {
//             success = numtokens > 1;
//             if (!success) {
//                 fprintf(stderr, "Error in config file '%s', at line %d:\n"
//                         "Mandatory FILENAME argument for "
//                         "'include' directive\n", filename, linenum);
//                 goto cleanup;
//             }
//             char *configfile = tokens[1];
//             success = parseOptionsFromFile(configfile);
//             if (!success) goto cleanup;
//             handled = 1;
//         } else if (strcmp("cluster", tokens[0]) == 0 ||
//                    strcmp("entry-point", tokens[0]) == 0)
//         {
//             success = numtokens > 1;
//             if (!success) {
//                 fprintf(stderr, "Error in config file '%s', at line %d:\n"
//                         "Mandatory ADDRESS argument for "
//                         "'%s' option\n", filename, linenum, tokens[0]);
//                 goto cleanup;
//             }
//             if (config.entry_points_count >= MAX_ENTRY_POINTS) {
//                 // proxyLogWarn(MAX_ENTRY_POINTS_WARN_MSG,
//                 //     MAX_ENTRY_POINTS, tokens[1]);
//                 goto next_line;
//             }
//             redisClusterEntryPoint *entry_point =
//                 &(config.entry_points[config.entry_points_count++]);
//             if (!parseAddress(tokens[1], entry_point)) {
//                 fprintf(stderr, "Error in config file '%s', at line %d:\n"
//                         "Invalid address for '%s' option\n",
//                         filename, linenum, tokens[0]);
//                 config.entry_points_count--;
//                 goto next_line;
//             }
//             handled = 1;
//         } else if (strcmp("help", tokens[0]) == 0) goto next_line;
//         if (handled) goto next_line;
//         int from = argc;
//         if (numtokens > 1) {
//             int yesno = 0;
//             if (strcasecmp("yes", tokens[1]) == 0) {
//                 argc += 1;
//                 argv = zrealloc(argv, argc * sizeof(sds));
//                 argv[from] = sdscat(sdsnew("--"), tokens[0]);
//                 yesno = 1;
//             } else if (strcasecmp("no", tokens[1]) == 0) yesno = 1;
//             if (yesno) goto next_line;
//         }
//         argc += numtokens;
//         argv = zrealloc(argv, argc * sizeof(sds));
//         for (i = 0; i < numtokens; i++) {
//             sds token = tokens[i];
//             if (i == 0) {
//                 /* Prepend the "--" to the first token, since it will be
//                  * parsed by the standard pardeOptions function */
//                 token = sdscat(sdsnew("--"), token);
//             } else token = sdsdup(token);
//             argv[from + i] = token;
//         }
// next_line:
//         if (line) sdsfree(line);
//         if (tokens) {
//             for (i = 0; i < numtokens; i++) sdsfree(tokens[i]);
//             zfree(tokens);
//         }
//     }
//     if (argc > 1) parseOptions(argc, argv);
// cleanup:
//     if (f != stdin) fclose(f);
//     if (argv) {
//         for (i = 0; i < argc; i++) sdsfree(argv[i]);
//         zfree(argv);
//     }
//     return success;
// }