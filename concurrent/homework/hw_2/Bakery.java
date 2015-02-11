// Use AtomicIntegerArray to get volatile arrays
// In the spirit of this assignment, I've only used the get() and set()
// primitives, so these behave identically to how they would if java
// actually provided arrays with volatile elements. It's quite frustrating
// that this isn't a language built in, honestly...
import java.util.concurrent.atomic.AtomicIntegerArray;

class Bakery
{
    private int n_threads;              // Number of threads accessing the bakery

    private AtomicIntegerArray flags;   // A flag for every thread
    private AtomicIntegerArray labels;  // A number for each thread

    public Bakery(int n_threads)
    {
        // Record the number of threads accessing us
        this.n_threads = n_threads;

        // Create and initialize the flag and label arrays
        flags = new AtomicIntegerArray(n_threads);
        labels = new AtomicIntegerArray(n_threads);
        int i;
        for (i = 0; i < n_threads; i++) {
            flags.set(i, 0);
            labels.set(i, 0);
        }
    }

    public void lock(int thread_id)
    {
        // Indicate our interest in the lock
        flags.set(thread_id, 1);
        labels.set(thread_id, max_label() + 1);

        int i;
        boolean mine = false;       // Whether or not the lock is now mine

        // Wait till we have priority
        do {
            mine = true;
            for (i = 0; i < n_threads; i++) if (i != thread_id) {
                // If there's another thread interested, with lower lexigraphical order,
                // the lock isn't ours
                if ((flags.get(i) == 1) &&
                    is_after(labels.get(thread_id), thread_id, labels.get(i), i)) {
                    mine = false;
                }
            }
        } while (!mine);
    }

    public void unlock(int thread_id)
    {
        // Release our interest
        flags.set(thread_id, 0);
    }

    // Check whether the first pair of values comes after the second
    // in lexicographical order
    private boolean is_after(int major1, int minor1, int major2, int minor2)
    {
        return (major1 > major2) || (major1 == major2 && minor1 > minor2);
    }

    // Find the maximum label of interested threads
    private int max_label()
    {
        int max = 0;
        int i;
        for (i = 0; i < n_threads; i++) {
            if ((flags.get(i) == 1) && (labels.get(i) > max)) max = labels.get(i);
        }

        return max;
    }
}
