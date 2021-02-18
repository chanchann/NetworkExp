## epoll

三个步骤

1. epoll_create

2. epoll_ctl 

3. epoll_wait

```cpp
int epoll_create(int size);
int epoll_create1(int flags);

int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
       

int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
```

[epoll实验](../src/senddata/)

## 对比LT和ET

## epoll各种事件检测

## epoll源码分析

[epoll code](https://github.com/torvalds/linux/blob/master/fs/eventpoll.c)

### 基础数据结构

1. eventpoll

2. epitem

3. eppoll_entry

### eventpoll

在调用 epoll_create 之后内核侧创建的一个句柄，表示了一个 epoll 实例, 后续如果我们再调用 epoll_ctl 和 epoll_wait 等，都是对这个 eventpoll 数据进行操作，这部分数据会被保存在 epoll_create 创建的匿名文件 file 的 private_data 字段中。

```cpp
struct eventpoll {
    spinlock_t lock;
    struct mutex mtx;
    //这个队列里存放的是执行epoll_wait从而等待的进程队列
    wait_queue_head_t wq;
    //这个队列里存放的是该eventloop作为poll对象的一个实例，加入到等待的队列
    //这是因为eventpoll本身也是一个file, 所以也会有poll操作
    wait_queue_head_t poll_wait;
    //这里存放的是事件就绪的fd列表，链表的每个元素是下面的epitem
    struct list_head rdllist;
    //这是用来快速查找fd的红黑树
    struct rb_root_cached rbr;
    struct epitem *ovflist;
    struct wakeup_source *ws;
    struct user_struct *user;
    struct file *file;
    int visited;
    struct list_head visited_list_link;
#ifdef CONFIG_NET_RX_BUSY_POLL
    unsigned int napi_id;
#endif
};
```

### epitem

每当我们调用 epoll_ctl 增加一个 fd 时，内核就会为我们创建出一个 epitem 实例，并且把这个实例作为红黑树的一个子节点，增加到 eventpoll 结构体中的红黑树中，对应的字段是 rbr。这之后，查找每一个 fd 上是否有事件发生都是通过红黑树上的 epitem 来操作。

```cpp 

struct epitem {
    union {
        /* RB tree node links this structure to the eventpoll RB tree */
        struct rb_node rbn;
        /* Used to free the struct epitem */
        struct rcu_head rcu;
    };
    //将这个epitem连接到eventpoll 里面的rdllist的list指针
    struct list_head rdllink;
    struct epitem *next;
    //epoll监听的fd
    struct epoll_filefd ffd;
    //一个文件可以被多个epoll实例所监听，这里就记录了当前文件被监听的次数
    int nwait;
    struct list_head pwqlist;
    //当前epollitem所属的eventpoll
    struct eventpoll *ep;
    struct list_head fllink;
    struct wakeup_source __rcu *ws;
    struct epoll_event event;
};
```

## eppoll_entry

每次当一个 fd 关联到一个 epoll 实例，就会有一个 eppoll_entry 产生。

```cpp
struct eppoll_entry {
    struct list_head llink;
    struct epitem *base;
    wait_queue_entry_t wait;
    wait_queue_head_t *whead;
};
```

## epoll_create

```cpp 
// 对传入的 flags 参数做简单的验证
BUILD_BUG_ON(EPOLL_CLOEXEC != O_CLOEXEC);
if (flags & ~EPOLL_CLOEXEC)
    return -EINVAL;
// 内核申请分配 eventpoll 需要的内存空间
error = ep_alloc(&ep);
if (error < 0)
    return error;
// 分配文件描述字
fd = get_unused_fd_flags(O_RDWR | (flags & O_CLOEXEC));
if (fd < 0) {
    error = fd;
    goto out_free_ep;
}
// 分配匿名文件
// 在调用 anon_inode_get_file 的时候，epoll_create 将 eventpoll 作为匿名文件 file 的 private_data 保存了起来
// 在之后通过 epoll 实例的文件描述字来查找时，就可以快速地定位到 eventpoll 对象了
file = anon_inode_getfile("[eventpoll]", &eventpoll_fops, ep,
             O_RDWR | (flags & O_CLOEXEC));
if (IS_ERR(file)) {
    error = PTR_ERR(file);
    goto out_free_fd;
}
// eventpoll 的实例会保存一份匿名文件的引用
ep->file = file;
// 调用 fd_install 函数将匿名文件和文件描述字完成了绑定。
fd_install(fd, file);
// 文件描述字作为 epoll 的文件句柄，被返回。
return fd;
```

## epoll_ctl

```cpp 
// epoll_ctl 函数通过 epoll 实例句柄来获得对应的匿名文件
//获得epoll实例对应的匿名文件, UNIX一切皆文件
f = fdget(epfd);
if (!f.file)
    goto error_return;
//获得真正的文件（添加的套接字对应的文件），如监听套接字、读写套接字
tf = fdget(fd);
if (!tf.file)
    goto error_fput;

// 行了一系列的数据验证，以保证用户传入的参数是合法的
// 比如 epfd 真的是一个 epoll 实例句柄，而不是一个普通文件描述符。
//如果不支持poll，那么该文件描述字是无效的
error = -EPERM;
if (!tf.file->f_op->poll)
    goto error_tgt_fput;
if xxx
if xxx
...

// 如果获得了一个真正的 epoll 实例句柄，就可以通过 private_data 获取之前创建的 eventpoll 实例
ep = f.file->private_data;

// 红黑树查找 -> 高效原因
// 接下来 epoll_ctl 通过目标文件和对应描述字，在红黑树中查找是否存在该套接字
// eventpoll 通过红黑树跟踪了当前监听的所有文件描述字，而这棵树的根就保存在 eventpoll 数据结构中。
struct rb_root_cached rbr;

// 对于每个被监听的文件描述字，都有一个对应的 epitem 与之对应，epitem 作为红黑树中的节点就保存在红黑树中。
epi = ep_find(ep, tf.file, fd);

// epitem 必须提供比较能力，以便可以按大小顺序构建出一棵有序的二叉树。其排序能力是依靠 epoll_filefd 结构体来完成的，
// epoll_filefd 可以简单理解为需要监听的文件描述字，它对应到二叉树上的节点。
struct epoll_filefd {
  struct file *file; 
  int fd; 
} __packed;
// 按照文件的地址大小排序。如果两个相同，就按照文件文件描述字来排序。
static inline int ep_cmp_ffd(struct epoll_filefd *p1,
                            struct epoll_filefd *p2)
{
  return (p1->file > p2->file ? +1:
       (p1->file < p2->file ? -1 : p1->fd - p2->fd));
}
// 在进行完红黑树查找之后，如果发现是一个 ADD 操作，并且在树中没有找到对应的二叉树节点，就会调用 ep_insert 进行二叉树节点的增加。
case EPOLL_CTL_ADD:
    if (!epi) {
        epds.events |= POLLERR | POLLHUP;
        error = ep_insert(ep, &epds, tf.file, fd, full_check);
    } else
        error = -EEXIST;
    if (full_check)
        clear_tfile_check_list();
    break;
```

```cpp 
// 跳转进入ep_insert()

// ep_insert 首先判断当前监控的文件值是否超过了 /proc/sys/fs/epoll/max_user_watches 的预设最大值，如果超过了则直接返回错误。
user_watches = atomic_long_read(&ep->user->epoll_watches);
if (unlikely(user_watches >= max_user_watches))
    return -ENOSPC;

// 接下来是分配资源和初始化动作。
if (!(epi = kmem_cache_alloc(epi_cache, GFP_KERNEL)))
        return -ENOMEM;
    INIT_LIST_HEAD(&epi->rdllink);
    INIT_LIST_HEAD(&epi->fllink);
    INIT_LIST_HEAD(&epi->pwqlist);
    epi->ep = ep;
    ep_set_ffd(&epi->ffd, tfile, fd);
    epi->event = *event;
    epi->nwait = 0;
    epi->next = EP_UNACTIVE_PTR;

// ep_insert 会为加入的每个文件描述字设置回调函数。这个回调函数是通过函数 ep_ptable_queue_proc 来进行设置的
epq.epi = epi;
init_poll_funcptr(&epq.pt, ep_ptable_queue_proc);

// 跳转进入ep_ptable_queue_proc
// 对应的文件描述字上如果有事件发生，就会调用这个函数，比如套接字缓冲区有数据了，就会回调这个函数
static void ep_ptable_queue_proc(struct file *file, wait_queue_head_t *whead,poll_table *pt)
{
    struct epitem *epi = ep_item_from_epqueue(pt);
    struct eppoll_entry *pwq;

    if (epi>nwait >= 0 && (pwq = kmem_cache_alloc(pwq_cache, GFP_KERNEL))) {
        init_waitqueue_func_entry(&pwq->wait, ep_poll_callback);
        pwq->whead = whead;
        pwq->base = epi;
        if (epi->event.events & EPOLLEXCLUSIVE)
            add_wait_queue_exclusive(whead, &pwq->wait);
        else
            add_wait_queue(whead, &pwq->wait);
        list_add_tail(&pwq->llink, &epi->pwqlist);
        epi->nwait++;
    } else {
        /* We have to signal that an error occurred */
        epi->nwait = -1;
    }
}

// 跳转到ep_poll_callback
// 将内核事件真正地和 epoll 对象联系了起来。

/*
 * This is the callback that is passed to the wait queue wakeup
 * mechanism. It is called by the stored file descriptors when they
 * have events to report.
 */
static int ep_poll_callback(wait_queue_entry_t *wait, unsigned mode, int sync, void *key)
{
    int pwake = 0;
    unsigned long flags;
    // 通过这个文件的 wait_queue_entry_t 对象找到对应的 epitem 对象
    // 因为 eppoll_entry 对象里保存了 wait_queue_entry_t，根据 wait_queue_entry_t 这个对象的地址就可以简单计算出 eppoll_entry 对象的地址，从而可以获得 epitem 对象的地址
    struct epitem *epi = ep_item_from_wait(wait);
    // 一旦获得 epitem 对象，就可以寻迹找到 eventpoll 实例。
    struct eventpoll *ep = epi->ep;
    spin_lock_irqsave(&ep->lock, flags);
    // 对发生的事件进行过滤, ep_insert 向对应监控文件注册的是所有的事件，而实际用户侧订阅的事件未必和内核事件对应
    // 比如，用户向内核订阅了一个套接字的可读事件，在某个时刻套接字的可写事件发生时，并不需要向用户空间传递这个事件。

    if (key && !((unsigned long) key & epi->event.events))
        goto out_unlock;
    // 判断是否需要把该事件传递给用户空间
    if (unlikely(ep->ovflist != EP_UNACTIVE_PTR)) {
    if (epi->next == EP_UNACTIVE_PTR) {
        epi->next = ep->ovflist;
        ep->ovflist = epi;
        if (epi->ws) {
            __pm_stay_awake(ep->ws);
        }
    }
    goto out_unlock;
    }
    // 如果需要，而且该事件对应的 event_item 不在 eventpoll 对应的已完成队列中，就把它放入该队列，以便将该事件传递给用户空间。
    if (!ep_is_linked(&epi->rdllink)) {
        list_add_tail(&epi->rdllink, &ep->rdllist);
        ep_pm_stay_awake_rcu(epi);
    }
    // 当我们调用 epoll_wait 的时候，调用进程被挂起，在内核看来调用进程陷入休眠。如果该 epoll 实例上对应描述字有事件发生，这个休眠进程应该被唤醒，以便及时处理事件.
    // wake_up_locked 函数唤醒当前 eventpoll 上的等待进程。
    if (waitqueue_active(&ep->wq)) {
        if ((epi->event.events & EPOLLEXCLUSIVE) &&
                    !((unsigned long)key & POLLFREE)) {
            switch ((unsigned long)key & EPOLLINOUT_BITS) {
            case POLLIN:
                if (epi->event.events & POLLIN)
                    ewake = 1;
                break;
            case POLLOUT:
                if (epi->event.events & POLLOUT)
                    ewake = 1;
                break;
            case 0:
                ewake = 1;
                break;
            }
        }
        wake_up_locked(&ep->wq);
    } 
```

### epoll_wait

```cpp 
// epoll_wait 函数首先进行一系列的检查，例如传入的 maxevents 应该大于 0。
if (maxevents <= 0 || maxevents > EP_MAX_EVENTS)
    return -EINVAL;

if (!access_ok(VERIFY_WRITE, events, maxevents * sizeof(struct epoll_event)))
    return -EFAULT;

// 和epoll_ctl 一样，通过 epoll 实例找到对应的匿名文件和描述字，并且进行检查和验证。
f = fdget(epfd);
if (!f.file)
    return -EBADF;

error = -EINVAL;
if (!is_file_epoll(f.file))
    goto error_fput;
// 还是通过读取 epoll 实例对应匿名文件的 private_data 得到 eventpoll 实例
ep = f.file->private_data;
// 调用 ep_poll 来完成对应的事件收集并传递到用户空间
error = ep_poll(ep, events, maxevents, timeout);
```

## ep_poll 

ep_poll 就分别对 timeout 不同值的场景进行了处理。如果大于 0 则产生了一个超时时间，如果等于 0 则立即检查是否有事件发生。

```cpp
static int ep_poll(struct eventpoll *ep, struct epoll_event __user *events,int maxevents, long timeout)
{
    int res = 0, eavail, timed_out = 0;
    unsigned long flags;
    u64 slack = 0;
    wait_queue_entry_t wait;
    ktime_t expires, *to = NULL;

    if (timeout > 0) {
        struct timespec64 end_time = ep_set_mstimeout(timeout);
        slack = select_estimate_accuracy(&end_time);
        to = &expires;
        *to = timespec64_to_ktime(end_time);
    } else if (timeout == 0) {
        /*
        * Avoid the unnecessary trip to the wait queue loop, if the
        * caller specified a non blocking operation.
        */
        timed_out = 1;
        spin_lock_irqsave(&ep->lock, flags);
        goto check_events;
    }
    spin_lock_irqsave(&ep->lock, flags);
    // 获得这把锁之后，检查当前是否有事件发生，如果没有，就把当前进程加入到 eventpoll 的等待队列 wq 中，这样做的目的是当事件发生时，ep_poll_callback 函数可以把该等待进程唤醒。

    if (!ep_events_available(ep)) {
        ep_reset_busy_poll_napi_id(ep);
        init_waitqueue_entry(&wait, current);
        __add_wait_queue_exclusive(&ep->wq, &wait);

    // 一个无限循环, 这个循环中通过调用 schedule_hrtimeout_range，将当前进程陷入休眠，CPU 时间被调度器调度给其他进程使用

    //这个循环里，当前进程可能会被唤醒，唤醒的途径包括
    //1.当前进程超时
    //2.当前进行收到一个signal信号
    //3.某个描述字上有事件发生
    //对应的1.2.3都会通过break跳出循环
    //第4个可能是当前进程被CPU重新调度，进入for循环的判断，如果没有满足1.2.3的条件，就又重新进入休眠
    for (;;) {
        set_current_state(TASK_INTERRUPTIBLE);
        if (fatal_signal_pending(current)) {
            res = -EINTR;
            break;
        }
        if (ep_events_available(ep) || timed_out)
            break;
        if (signal_pending(current)) {
            res = -EINTR;
            break;
        }
        spin_unlock_irqrestore(&ep->lock, flags);
        //通过调用schedule_hrtimeout_range，当前进程进入休眠，CPU时间被调度器调度给其他进程使用
        if (!schedule_hrtimeout_range(to, slack, HRTIMER_MODE_ABS))
            timed_out = 1;
        spin_lock_irqsave(&ep->lock, flags);
    }


    //从休眠中结束，将当前进程从wait队列中删除，设置状态为TASK_RUNNING，接下来进入check_events，来判断是否是有事件发生
    __remove_wait_queue(&ep->wq, &wait);
    __set_current_state(TASK_RUNNING);


    //ep_send_events将事件拷贝到用户空间
    if (!res && eavail &&
        !(res = ep_send_events(ep, events, maxevents)) && !timed_out)
        goto fetch_events;
    return res;
}

```

## ep_send_events

[未完待续](https://www.nowcoder.com/discuss/26226)