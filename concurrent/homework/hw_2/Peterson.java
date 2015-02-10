class Peterson  {

    //The following variables are used for implementing a lock using
    //Peterson's protocol.  We use two variables for flags instead of
    //an array, as there is no way to make array elements volatile. 
    private static volatile int victim;
    private static volatile int flag0;
    private static volatile int flag1;

    public void lock(int me) {
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

    public void unlock(int me) {
        if (me==0) flag0=0; else flag1=0;
    }
}

