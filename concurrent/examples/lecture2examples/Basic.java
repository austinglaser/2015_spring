
public class Basic implements Runnable {

    //me is used as a thread id
    public int me;
    //each thread counts to 1000
    public static final int countToThis = 1000;
    //we run 1000 experiments
    public static final int noOfExperiments = 1000;
    //count is a shared variable
    public static volatile int count = 0;

    public Basic(int newMe) {
        me = newMe;
    }

    public void run() {
        int i = 0;
        while (i < countToThis) {
            count = count + 1;
            i = i + 1;
        }
    }

    public static void main(String args[]) {
        int i = 0;   
	int wrong = 0;

	//timing the execution
	long startTime = System.nanoTime();

        while (i<noOfExperiments) {
            count = 0;
            Thread t1 = new Thread(new Basic(0));
            Thread t2 = new Thread(new Basic(1));

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
