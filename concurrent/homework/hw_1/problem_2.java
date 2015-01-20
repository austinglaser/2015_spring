import java.util.concurrent.locks.ReentrantLock;

public class problem_2 implements Runnable {

    // Number of threads
    private static final int N_THREADS = 128;

    // Each thread has its own id
    private int thread_id;

    // Shared count, with it's lock
    private static volatile int count = 0;
    public static final ReentrantLock count_lock = new ReentrantLock();

    // Constructor
    public problem_2(int new_thread_id) {
        thread_id = new_thread_id;
    }

    // run() function.
    public void run() {
        // Any access to the shared count variable is inside this lock zone. This means that only
        // one thread will be changing the variable at a given time; ensuring that we don't get
        // duplicated increment instructions (resulting in a lower final count than we would
        // expect)
        count_lock.lock();
        // Increment the count
        count++;
        count_lock.unlock();

        // Print out thread id
        System.out.println(thread_id);
    }

    // Class entry point
    public static void main(String args[]) {
        int i = 0;

        // Array of threads
        Thread[] t = new Thread[N_THREADS];

        // Create threads
        for (i = 0; i < N_THREADS; i++) {
            t[i] = new Thread(new problem_2(i));
        }

        // Start threads
        for (i = 0; i < N_THREADS; i++) {
            t[i].start();
        }

        // Wait for threads
        for (i = 0; i < N_THREADS; i++) {
            try {
                t[i].join();
            }
            catch (InterruptedException e) {}
        }

        // Print final count
        System.out.println("Final Count: " + count);
    }
}
