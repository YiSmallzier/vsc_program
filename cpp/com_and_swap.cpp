
inline int compare_and_swap(volatile int* addr, int old_value, int new_value) {
    int result;
    asm volatile (
        "lock cmpxchg %2,%1\n\t"
        : "=a"(result), "=m"(*addr)
        : "q"(new_value), "0"(old_value), "m"(*addr)
        : "memory");
    return result;
}

const int n = num_threads;
int bolt;

void P(int i) {
    int spin_lock_wait_time = 0; // 自旋锁等待时间
    while (true) {
        // 获取自旋锁
        while (compare_and_swap(&bolt, 0, 1) == 1) {
            // 如果自旋锁被占用，则快速重试
            if (++spin_lock_wait_time >= n) {
                spin_lock_wait_time = 0; // 重新开始计时
                break;
            }
        }
        if (bolt == 0) {
            // 进入临界区
            /* code */
            // 释放自旋锁
            bolt = 0;
            return;
        }
        // 释放自旋锁
        bolt = 0;
        spin_lock_wait_time = 0; // 重新开始计时
    }
}


