class TreeLock {
    private TreeLock sub_lock;
    private Peterson[] locks;
    private boolean is_root;
    private int n_threads_this_level;

    public TreeLock(int n_threads) {
        // Check if n_threads is a power of 2, and greater than 1
        if (!((n_threads > 1) && ((n_threads & (n_threads - 1)) == 0))) {
            throw new IllegalArgumentException();
        }

        // Record how many threads we support
        n_threads_this_level = n_threads;

        int i;
        for (i = 0; i < n_threads / 2; i++) {
            locks[i] = new Peterson();
        }

        // Base case
        if (n_threads == 2) {
            is_root = true;
            sub_lock = null;
        }

        // Recursive constructor; create a sub
        // tree-lock supporting half as many threads
        else {
            is_root = false;
            sub_lock = new TreeLock(n_threads/2);
        }
    }

    public void lock(int thread_id) {
        // Check that the argument's valid
        if (thread_id >= n_threads_this_level) {
            throw new IllegalArgumentException();
        }

        // Take the appropriate lock on this level
        locks[thread_id / 2].lock(thread_id % 2);

        // Lock the sublock to descend to the next level
        if (!is_root) sub_lock.lock(thread_id / 2);
    }

    public void unlock(int thread_id) {
        // Check that the argument's valid
        if (thread_id >= n_threads_this_level) {
            throw new IllegalArgumentException();
        }

        // Release sub-locks
        if (!is_root) sub_lock.unlock(thread_id/2);

        // Release this level's lock
        locks[thread_id/2].unlock(thread_id % 2);
    }
}
