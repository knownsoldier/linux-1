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

/* https://lwn.net/Articles/604287/ */
/* https://0xax.gitbooks.io/linux-insides/content/SysCall/syscall-1.html */
//SYSCALL_DEFINE3(read, unsigned int, fd, char __user *, buf, size_t, count) {
//SYSCALL_DEFINE2(mkdir, const char __user *, pathname, int, mode) {
SYSCALL_DEFINE4(userinfo, int, uid, int *,procs, int *,sigs, int *,fds) {
//asmlinkage long sys_userinfo(uid_t uid, const char __user * procs, const char __user *sigs, const char __user *fds) { 
/* extern struct user_struct *find_user(kuid_t);
 * struct user_struct {
 *	atomic_t __count;	 reference count
 *	atomic_t processes;	 How many processes does this user have?
 *	atomic_t sigpending;	 How many pending signals does this user have?
 */

    int nrOfProcesses, nrOfPending;
    long nrOfFDs;
    kuid_t kuid;
    struct user_struct* current_user;

    struct user_namespace *ns = current_user_ns();
    
    if (ns == NULL)
        return -EFAULT;

    kuid = make_kuid(ns, uid);
    
    if (!uid_valid(kuid))
        return -EINVAL;

    current_user = find_user(kuid);
    if ( current_user == NULL) 
        return -EFAULT;

    nrOfProcesses = atomic_read(&(current_user->processes));
    nrOfPending = atomic_read(&(current_user->sigpending));
    nrOfFDs = atomic_long_read(&(current_user->epoll_watches));

    
    if ( copy_to_user(procs, &nrOfProcesses, sizeof(nrOfProcesses)))
        return -EFAULT;
    if ( copy_to_user(sigs, &nrOfPending, sizeof(nrOfPending)))
        return -EFAULT;
    if ( copy_to_user(fds, &nrOfFDs, sizeof(nrOfFDs)))
        return -EFAULT;


			/* return ERR_PTR(-EINVAL); */

    return 0;
}
