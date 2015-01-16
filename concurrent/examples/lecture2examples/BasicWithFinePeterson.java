public class BasicWithFinePeterson implements Runnable {

    public int me;
    public static final int countToThis = 1000;
    public static final int noOfExperiments = 1000;
    public static volatile int count = 0;

    //The following variables are used for implementing a lock usign
    //Peterson's protocol.  We use to variables for flags instead of
    //an array, as there is now way to make array elements volatile. 
    public static volatile int flag0;
    public static volatile int flag1;
    public static volatile int victim;

    public void lock() {
	if (me==0) {
	    flag0 = 1;
	    victim = 0;
	    while ((flag1==1) && (victim==0)) {}
	} else {
	    flag1 = 1;
	    victim = 1;
	    while ((flag0==1) && (victim==1)) {}
	}
    }

    public void unlock() {
	if (me==0) flag0=0; else flag1=0;
    }
    
    public BasicWithFinePeterson(int newMe) {
        me = newMe;
	flag0 = 0;
	flag1 = 0;
	victim = 0;
    }

    public void run() {
        int i = 0;
        while (i < countToThis) {
	    lock();
            count = count + 1;
	    unlock();
            i = i + 1;
        }
    }

    public static void main(String args[]) {
        int i = 0;   
	int wrong = 0;

	long startTime = System.nanoTime();
        while (i<noOfExperiments) {
            count = 0;

	    Thread t1 = new Thread(new BasicWithFinePeterson(0));
	    Thread t2 = new Thread(new BasicWithFinePeterson(1));

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
