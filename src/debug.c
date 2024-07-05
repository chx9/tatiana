// #include "config.h"
// #include "logger.h"
// #include "sds.h"
// #include "tatiana.h"
// #include "redis_config.h"

// #include <arpa/inet.h>
// #include <signal.h>
// #include <dlfcn.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <inttypes.h>
// #include <sys/stat.h>
// #include <sys/utsname.h>
// #include <errno.h>
// #include <stdio.h>
// #include <string.h>
// #include <pthread.h>
// #ifdef HAVE_BACKTRACE
// #include <execinfo.h>
// #ifndef __OpenBSD__
// #include <ucontext.h>
// #else
// typedef ucontext_t sigcontext_t;
// #endif
// #include <fcntl.h>
// #include <unistd.h>
// #endif /* HAVE_BACKTRACE */

// #ifdef __CYGWIN__
// #ifndef SA_ONSTACK
// #define SA_ONSTACK 0x08000000
// #endif
// #endif
// extern Tanya tatiana;
// extern int ae_api_kqueue;
// int sendStopMessageToThread(workThread *thread);
// const char *assert_failed = NULL;
// const char *assert_file = NULL;
// int assert_line = -1;
// int bug_report_start = 0;
// #ifdef HAVE_BACKTRACE
// static void *getMcontextEip(ucontext_t *uc) {
// #if defined(__APPLE__) && !defined(MAC_OS_X_VERSION_10_6)
//     /* OSX < 10.6 */
//     #if defined(__x86_64__)
//     return (void*) uc->uc_mcontext->__ss.__rip;
//     #elif defined(__i386__)
//     return (void*) uc->uc_mcontext->__ss.__eip;
//     #else
//     return (void*) uc->uc_mcontext->__ss.__srr0;
//     #endif
// #elif defined(__APPLE__) && defined(MAC_OS_X_VERSION_10_6)
//     /* OSX >= 10.6 */
//     #if defined(_STRUCT_X86_THREAD_STATE64) && !defined(__i386__)
//     return (void*) uc->uc_mcontext->__ss.__rip;
//     #else
//     return (void*) uc->uc_mcontext->__ss.__eip;
//     #endif
// #elif defined(__linux__)
//     /* Linux */
//     #if defined(__i386__) || defined(__ILP32__)
//     return (void*) uc->uc_mcontext.gregs[14]; /* Linux 32 */
//     #elif defined(__X86_64__) || defined(__x86_64__)
//     return (void*) uc->uc_mcontext.gregs[16]; /* Linux 64 */
//     #elif defined(__ia64__) /* Linux IA64 */
//     return (void*) uc->uc_mcontext.sc_ip;
//     #elif defined(__arm__) /* Linux ARM */
//     return (void*) uc->uc_mcontext.arm_pc;
//     #elif defined(__aarch64__) /* Linux AArch64 */
//     return (void*) uc->uc_mcontext.pc;
//     #endif
// #elif defined(__FreeBSD__)
//     /* FreeBSD */
//     #if defined(__i386__)
//     return (void*) uc->uc_mcontext.mc_eip;
//     #elif defined(__x86_64__)
//     return (void*) uc->uc_mcontext.mc_rip;
//     #endif
// #elif defined(__OpenBSD__)
//     /* OpenBSD */
//     #if defined(__i386__)
//     return (void*) uc->sc_eip;
//     #elif defined(__x86_64__)
//     return (void*) uc->sc_rip;
//     #endif
// #elif defined(__DragonFly__)
//     return (void*) uc->uc_mcontext.mc_rip;
// #else
//     return NULL;
// #endif
// }

// /* Return a file descriptor to write directly to the Redis log with the
//  * write(2) syscall, that can be used in critical sections of the code
//  * where the rest of Redis can't be trusted (for example during the memory
//  * test) or when an API call requires a raw fd.
//  *
//  * Close it with closeDirectLogFiledes(). */
// int openDirectLogFiledes(void) {
//     int log_to_stdout = (
//         config.log_file == NULL ||
//         config.log_file[0] == '\0'
//     );
//     int fd = log_to_stdout ?
//         STDOUT_FILENO :
//         open(config.log_file, O_APPEND|O_CREAT|O_WRONLY, 0644);
//     return fd;
// }
// /* Used to close what closeDirectLogFiledes() returns. */
// void closeDirectLogFiledes(int fd) {
//     int log_to_stdout = (
//         config.log_file == NULL ||
//         config.log_file[0] == '\0'
//     );
//     if (!log_to_stdout) close(fd);
// }

// /* Logs the stack trace using the backtrace() call. This function is designed
//  * to be called from signal handlers safely. */
// void logStackTrace(ucontext_t *uc) {
//     void *trace[101];
//     int trace_size = 0, fd = openDirectLogFiledes();

//     if (fd == -1) {
//         proxyLogRaw("Cannot write to log!\n");
//         return; /* If we can't log there is anything to do. */
//     }

//     /* Generate the stack trace */
//     trace_size = backtrace(trace+1, 100);

//     if (getMcontextEip(uc) != NULL) {
//         char *msg1 = "EIP:\n";
//         char *msg2 = "\nBacktrace:\n";
//         if (write(fd,msg1,strlen(msg1)) == -1) {/* Avoid warning. */};
//         trace[0] = getMcontextEip(uc);
//         backtrace_symbols_fd(trace, 1, fd);
//         if (write(fd,msg2,strlen(msg2)) == -1) {/* Avoid warning. */};
//     }

//     /* Write symbols to log file */
//     backtrace_symbols_fd(trace+1, trace_size, fd);

//     /* Cleanup */
//     closeDirectLogFiledes(fd);
// }
// void proxyLogHexDump(char *descr, void *value, size_t len) {
//     char buf[65], *b;
//     unsigned char *v = value;
//     char charset[] = "0123456789abcdef";

//     proxyLogRaw("%s (hexdump of %zu bytes):\n", descr, len);
//     b = buf;
//     while(len) {
//         b[0] = charset[(*v)>>4];
//         b[1] = charset[(*v)&0xf];
//         b[2] = '\0';
//         b += 2;
//         len--;
//         v++;
//         if (b-buf == 64 || len == 0) {
//             proxyLogRaw(buf);
//             b = buf;
//         }
//     }
//     proxyLogRaw("\n");
// }
// void bugReportStart(void) {
//     if (bug_report_start == 0) {
//         proxyLogRaw(
//             "\n\n=== PROXY BUG REPORT START: Cut & paste "
//             "starting from here ===\n"
//         );
//         bug_report_start = 1;
//     }
// }
// static void killThreads(int exclude) {
//     int err, j;

//     for (j = 0; j < config.num_threads; j++) {
//         if (j == exclude) continue;
//         workThread *thread = tatiana.threads[j];
//         if (pthread_cancel(thread->thread) == 0) {
//             /* Unfortunately, kqueue's kevent blocks even if pthread_cancel
//              * gets called, so we need to also send a stop message. */
//             if (ae_api_kqueue) sendStopMessageToThread(thread);
//             if ((err = pthread_join(thread->thread, NULL)) != 0) {
//                 proxyLogErr("Thread %d can be joined: %s",
//                         j, strerror(err));
//             } else {
//                 proxyLogErr("Thread %d terminated", j);
//             }
//         }
//     }
// }
// void sigsegvHandler(int sig, siginfo_t *info, void *secret) {
//   ucontext_t *uc = (ucontext_t *)secret;
//   void *eip = getMcontextEip(uc);
//   struct sigaction act;
//   UNUSED(info);
//   int cur_thread_id = getCurrentThreadID();
//   killThreads(cur_thread_id);

//   bugReportStart();
//   if (eip != NULL) {
//     proxyLogErr("Crashed running the instruction at: %p", eip);
//   }
//   if (sig == SIGSEGV || sig == SIGBUS) {
//     proxyLogErr("Accessing address: %p", (void *)info->si_addr);
//   }
//   if (cur_thread_id == PROXY_MAIN_THREAD_ID)
//     proxyLogErr("Handling crash on main thread");
//   else if (cur_thread_id == PROXY_UNKN_THREAD_ID)
//     proxyLogErr("Handling crash on thread: unknown");
//   else
//     proxyLogErr("Handling crash on thread: %d", cur_thread_id);
//   if (assert_failed != NULL) {
//     proxyLogErr("Failed assertion: %s (%s:%d)", assert_failed, assert_file,
//                 assert_line);
//   }

//   /* Log the stack trace */
//   proxyLogRaw("\n\n------ STACK TRACE ------\n");
//   logStackTrace(uc);

//   /* Log INFO and CLIENT LIST */
//   proxyLogRaw("\n\n------ INFO OUTPUT ------\n");
//   sds infostr = genInfoString(NULL, NULL);
//   proxyLogRaw(infostr);
//   sdsfree(infostr);

//   /* Log structure sizes */

//   proxyLogRaw("\n\n---- SIZEOF STRUCTS ----\n");
//   sds structsizestr = genStructSizeString();
//   proxyLogRaw("%s\n", structsizestr);
//   sdsfree(structsizestr);

//   /* Log dump of processor registers */
//   logRegisters(uc);

// #if defined(HAVE_PROC_MAPS)
//   /* Skip memory test if not running inside the main thread, since it would
//    * crash on a memory segment delcared to be writable, but actually not
//    * writable by the thread. */
//   if (pthread_self() != tatiana.main_thread)
//     goto dump_code;
//   /* Test memory */
//   proxyLogRaw("\n------ FAST MEMORY TEST ------\n");
//   if (memtest_test_linux_anonymous_maps()) {
//     proxyLogRaw("!!! MEMORY ERROR DETECTED! Check your memory ASAP !!!\n");
//   } else {
//     proxyLogRaw(
//         "Fast memory test PASSED, however your memory can still be broken. "
//         "Please run a memory test for several hours if possible.\n");
//   }
// dump_code:
// #endif
//   if (eip != NULL) {
//     Dl_info info;
//     if (dladdr(eip, &info) != 0) {
//       proxyLogRaw("\n\n------ DUMPING CODE AROUND EIP ------\n"
//                   "Symbol: %s (base: %p)\n"
//                   "Module: %s (base %p)\n"
//                   "$ xxd -r -p /tmp/dump.hex /tmp/dump.bin\n"
//                   "$ objdump --adjust-vma=%p -D -b binary -m i386:x86-64 "
//                   "/tmp/dump.bin\n"
//                   "------\n",
//                   info.dli_sname, info.dli_saddr, info.dli_fname,
//                   info.dli_fbase, info.dli_saddr);
//       size_t len = (long)eip - (long)info.dli_saddr;
//       unsigned long sz = sysconf(_SC_PAGESIZE);
//       if (len < 1 << 13) { /* we don't have functions over 8k (verified) */
//         /* Find the address of the next page, which is our "safety"
//          * limit when dumping. Then try to dump just 128 bytes more
//          * than EIP if there is room, or stop sooner. */
//         unsigned long next = ((unsigned long)eip + sz) & ~(sz - 1);
//         unsigned long end = (unsigned long)eip + 128;
//         if (end > next)
//           end = next;
//         len = end - (unsigned long)info.dli_saddr;
//         proxyLogHexDump("dump of function ", info.dli_saddr, len);
//         dumpX86Calls(info.dli_saddr, len);
//       }
//     }
//   }

//   proxyLogRaw(
//       "\n\n=== PROXY BUG REPORT END. Make sure to include from START to END. "
//       "===\n\n"
//       "       Please report the crash by opening an issue on github:\n\n"
//       "           https://github.com/artix75/redis-cluster-proxy/issues\n\n");

//   if (config.daemonize && config.pidfile)
//     unlink(config.pidfile);

//   /* Make sure we exit with the right signal at the end. So for instance
//    * the core will be dumped if enabled. */
//   sigemptyset(&act.sa_mask);
//   act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
//   act.sa_handler = SIG_DFL;
//   sigaction(sig, &act, NULL);
//   kill(getpid(), sig);
// }
// #endif /* HAVE_BACKTRACE */
