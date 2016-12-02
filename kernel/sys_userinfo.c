#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/user.h>
#include <asm-generic/atomic-long.h>
#include <asm/atomic.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/linkage.h>
#include <linux/kallsyms.h>
#include <trace/syscall.h>
#include <linux/userinfo.h>


/**
 * Returns
 *
 */

/* https://lwn.net/Articles/604287/ */
/* https://0xax.gitbooks.io/linux-insides/content/SysCall/syscall-1.html */
__must_check
SYSCALL_DEFINE2(userinfo, int, uid, struct userinfo __user *,info) {

    int nrOfProcesses, nrOfPending;
    long nrOfFDs;
    kuid_t kuid;
    struct user_struct* current_user;
    struct user_namespace *ns;


    /* Verify we can write results back to userspace */
    if (!access_ok(VERIFY_WRITE, info, sizeof(struct userinfo)))
        return -EFAULT;

    if (!access_ok(VERIFY_READ, info, sizeof(struct userinfo)))
        return -EFAULT;

    /* Check if parameter struct is of correct size */
    if (sizeof(*info) != sizeof(struct userinfo))
        return -EINVAL;


    /* Get namespace from which to get real uid */
    ns = current_user_ns();
    if (ns == NULL)
        return -EFAULT;
    kuid = make_kuid(ns, uid);

    /* Validate uid */
    if (!uid_valid(kuid))
        return -EINVAL;

    /* Get struct with user info */
    current_user = find_user(kuid);
    if ( current_user == NULL)
        return -EFAULT;

    /* Read data of interest */
    nrOfProcesses = atomic_read(&(current_user->processes));
    nrOfPending = atomic_read(&(current_user->sigpending));
    nrOfFDs = atomic_long_read(&(current_user->epoll_watches));


    /* Copy back to user space */
    if ( copy_to_user(&info->procs, &nrOfProcesses, sizeof(nrOfProcesses)))
        return -EFAULT;
    if ( copy_to_user(&info->sigs, &nrOfPending, sizeof(nrOfPending)))
        return -EFAULT;
    if ( copy_to_user(&info->fds, &nrOfFDs, sizeof(nrOfFDs)))
        return -EFAULT;

    /* Success */
    return 0;
}
