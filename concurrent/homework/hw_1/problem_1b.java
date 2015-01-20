public class problem_1b implements Runnable {

    // Number of threads
    private static final int N_THREADS = 128;

    // Each thread has its own id
    private int thread_id;

    // Shared count, with it's lock
    private static volatile int count = 0;

    // Constructor
    public problem_1b(int new_thread_id) {
        thread_id = new_thread_id;
    }

    // run() function.
    public void run() {
        // Increment the count
        count++;

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
            t[i] = new Thread(new problem_1b(i));
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
