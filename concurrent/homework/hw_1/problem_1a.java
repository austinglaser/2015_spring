public class problem_1a implements Runnable {

    // Total number of threads to be created
    public static final int N_THREADS = 128;

    // Each thread has its own id
    public int thread_id;

    // Constructor
    public problem_1a(int new_thread_id) {
        // Record the thread's given id
        thread_id = new_thread_id;
    }

    // run() function.
    public void run() {
        // Print out thread id
        System.out.println(thread_id);
    }

    public static void main(String args[]) {
        int i = 0;

        // Array of threads
        Thread[] t = new Thread[N_THREADS];

        // Create threads
        for (i = 0; i < N_THREADS; i++) {
            t[i] = new Thread(new problem_1a(i));
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

        // We're finished!
        System.out.println("Done!");
    }

}
