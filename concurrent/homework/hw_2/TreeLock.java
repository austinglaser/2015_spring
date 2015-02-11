// I originally tried to implement this using the Peterson lock as a subclass. After failing
// to get that to work, I implemented this version -- when this was complete and working, I
// realized the culprit was Peterson's use of static variables as the flags and victim.
// In some ways, that would ultimatelly be a more elegant solution, but I believe either 
// would work.

class TreeLock
{
    private int n_threads;      // Number of threads who will access the lock
    private boolean is_root;    // Whether this lock is the root (last lock before entering CS)
    private TreeLock sub_lock;  // Next lock down (contains effectively half as many locks as current level)

    // Use AtomicIntegerArray to get volatile arrays
    // In the spirit of this assignment, I've only used the get() and set()
    // primitives, so these behave identically to how they would if java
    // actually provided arrays with volatile elements. It's quite frustrating
    // that this isn't a language built in, honestly...
    private java.util.concurrent.atomic.AtomicIntegerArray flags;
    private java.util.concurrent.atomic.AtomicIntegerArray victims;

    public TreeLock(int n_threads)
    {
        // Check for valid argument
        if (n_threads < 2 || !is_power_of_two(n_threads)) {
            throw new IllegalArgumentException();
        }

        // Record number of threads for the lock
        this.n_threads = n_threads;

        // Create flags and victims for this level. We need a flag for every thread,
        // and a victim indicator for each pair of threads
        flags = new java.util.concurrent.atomic.AtomicIntegerArray(n_threads);
        victims = new java.util.concurrent.atomic.AtomicIntegerArray(n_threads / 2);

        // Initialize arrays
        int i;
        for (i = 0; i < n_threads; i++) flags.set(i, 0);
        for (i = 0; i < n_threads/2; i++) victims.set(i, 0);

        // Base case; this is the root lock. It has no lower levels,
        // and will allow direct access to the critical section to
        // the thread who takes it
        if (n_threads == 2) {
            is_root = true;
            sub_lock = null;
        }

        // Recursive case; locking this lock simply provides
        // access to the next lock down
        else {
            is_root = false;
            sub_lock = new TreeLock(n_threads / 2);
        }
    }

    public void lock(int thread_id)
    {
        if (thread_id >= n_threads || thread_id < 0) {
            throw new IllegalArgumentException();
        }

        // Determine our opposition this level. Each thread only
        // competes with it's neighbor on each level, implementing exactly
        // the Peterson algorithm. We store the flags in a single array
        // for convenience
        int opponent = (thread_id % 2 == 0) ? thread_id + 1 : thread_id - 1;

        // Figure out which victim element to use. We will use
        // the thread's id as the victim value to simplify the code
        int victim = thread_id / 2;

        // Say we're locking
        flags.set(thread_id, 1);
        victims.set(victim, thread_id);

        // Wait for our opponent to be unlocked or to become the victim
        while (flags.get(opponent) == 1 && victims.get(victim) == thread_id);

        // Now that we have this level's lock, try to take the next one
        // if there is one
        if (!is_root) sub_lock.lock(thread_id / 2);
    }

    public void unlock(int thread_id)
    {
        if (thread_id >= n_threads || thread_id < 0) {
            throw new IllegalArgumentException();
        }

        // It's important that we unlock the lower level first,
        // or mutual exclusion will be violated.
        // Releasing this level's lock will allow a competing thread
        // to move on to the next level and try to lock there.
        // It will designate itself the victim, releasing the thread
        // which is competing against this one (if there is any). 
        // Then, we will proceed to unlock that level, releasing
        // the thread which just joined (because it sees the flag clear)
        // and allowing both into the critical section

        // Unlock the lower level
        if (!is_root) sub_lock.unlock(thread_id / 2);

        // Unlock this level
        flags.set(thread_id, 0);
    }

    // Bit-twiddling check for whether n is a power of two
    private static boolean is_power_of_two(int n)
    {
        return (n > 0) && ((n & (n - 1)) == 0);
    }
}
