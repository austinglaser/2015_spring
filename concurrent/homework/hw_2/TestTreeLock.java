import java.util.concurrent.locks.Lock;

public class TestTreeLock implements Runnable {

    public int id;
    public static final int countToThis = 1000;
    public static final int noOfExperiments = 1000;
    public static final int noOfThreads = 8;
    public static volatile int count = 0;
    public static TreeLock lock = new TreeLock(noOfThreads);

    public TestTreeLock(int newId) {
        id = newId;
    }

    public void run() {
        int i = 0;
        while (i < countToThis) {
            lock.lock(id);
            count = count + 1;
            lock.unlock(id);
            i = i + 1;
        }
    }

    public static void main(String args[]) {
        int experiment_number = 0;   
        int wrong = 0;
        int i;

        long startTime = System.nanoTime();
        while (experiment_number<noOfExperiments) {
            count = 0;

            // Create and start threads
            Thread threads[] = new Thread[noOfThreads];
            for (i = 0; i < noOfThreads; i++) threads[i] = new Thread(new TestTreeLock(i));
            for (i = 0; i < noOfThreads; i++) threads[i].start();

            // Wait for them all to finish
            try {
                for (i = 0; i < noOfThreads; i++) threads[i].join();
            }
            catch (InterruptedException e) {}

            // Check result
            if (count != noOfThreads*countToThis) { 
                System.out.println("Wrong : " + count);
                wrong++;
            } 

            experiment_number = experiment_number + 1;
        }
        long endTime = System.nanoTime();
        System.out.println("That took " + (endTime - startTime)/1000000 + " milliseconds"); 
        System.out.println("Mistakes:  " + wrong + "/" + noOfExperiments);
    }

}

