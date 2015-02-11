// Use AtomicIntegerArray to get volatile arrays
// In the spirit of this assignment, I've only used the get() and set()
// primitives, so these behave identically to how they would if java
// actually provided arrays with volatile elements. It's quite frustrating
// that this isn't a language built in, honestly...
import java.util.concurrent.atomic.AtomicIntegerArray;

class FilterMExc {
    // Number of threads accessing this lock
    private int n_threads;  

    // Number of levels the lock has. Since each level blocks at most one 
    // thread, M (the maximum number of threads in the critical section at
    // any one time) is equal to n_threads - n_levels. We need not therefore
    // store M (n_in_zone) explicitly
    private int n_levels;   

    private AtomicIntegerArray levels;  // Each thread has a level value
    private AtomicIntegerArray victims; // Each level has a victim

    public FilterMExc(int n_threads, int n_in_zone)
    {
        if (n_threads <= n_in_zone) {
            throw new IllegalArgumentException();
        }

        // Record number of threads, and calculate how many
        // levels we need for proper M-exclusion
        this.n_threads = n_threads;
        n_levels = n_threads - n_in_zone;

        // Create and initialize level and victim arrays
        levels  = new AtomicIntegerArray(n_threads);
        victims = new AtomicIntegerArray(n_levels);
        int i;
        for (i = 0; i < n_threads; i++) levels.set(i, 0);
        for (i = 0; i < n_levels; i++) victims.set(i, 0);
    }

    public void lock(int thread_id)
    {
        if (thread_id >= n_threads || thread_id < 0) {
            throw new IllegalArgumentException();
        }

        // Walk up all the levels
        int current_level;
        for (current_level = 1; current_level < n_levels; current_level++) {
            // Indicate our interest, make ourselves the victim
            levels.set(thread_id, current_level);
            victims.set(current_level, thread_id);

            int i;
            boolean highest;
            boolean victim;

            // Wait as long as there are threads at a higher level than us and
            // we are the victim
            do {
                highest = true;
                for (i = 0; i < n_threads; i++) if (i != thread_id) {
                    if (levels.get(i) >= current_level) highest = false;
                }
                victim = (victims.get(current_level) == thread_id);
            } while (!highest && victim);
        }
    }

    public void unlock(int thread_id)
    {
        if (thread_id >= n_threads || thread_id < 0) {
            throw new IllegalArgumentException();
        }
        
        // Indicate we're no longer interested in the lock
        levels.set(thread_id, 0);
    }
}
