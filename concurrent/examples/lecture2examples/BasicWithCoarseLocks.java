import java.util.concurrent.locks.ReentrantLock;

public class BasicWithCoarseLocks implements Runnable {

    public int me;
    public static final int countToThis = 1000;
    public static final int noOfExperiments = 1000;
    public static volatile int count = 0;

    public static final ReentrantLock lock = new ReentrantLock();

    public BasicWithCoarseLocks(int newMe) {
        me = newMe;
    }

    public void run() {
        int i = 0;
	lock.lock();
        while (i < countToThis) {
            count = count + 1;
            i = i + 1;
        }
	lock.unlock();
    }

    public static void main(String args[]) {
        int i = 0;   
	int wrong = 0;

	long startTime = System.nanoTime();
        while (i<noOfExperiments) {
            count = 0;

	    Thread t1 = new Thread(new BasicWithCoarseLocks(0));
	    Thread t2 = new Thread(new BasicWithCoarseLocks(1));

            t1.start();
            t2.start();
	    try {
                t1.join();t2.join();
            }
            catch (InterruptedException e) {}
        
            if (count != 2*countToThis) { 
                System.out.println("Wrong : " + count);
                wrong++;
            } 

	    i = i + 1;
        }
	long endTime = System.nanoTime();
	System.out.println("That took " + (endTime - startTime)/1000000 + " milliseconds"); 
        System.out.println("Mistakes:  " + wrong + "/" + noOfExperiments);
    }

}
