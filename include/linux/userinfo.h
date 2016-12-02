#ifndef __LINUX_SYS_USERINFO_H
#define __LINUX_SYS_USERINFO_H

#include <linux/unistd.h>
#include <linux/linkage.h>

struct userinfo {
    int procs;
    int sigs;
    long fds;
};

/*
_syscall2(int, userinfo, int, uid, struct userinfo, *);
*/
#endif

