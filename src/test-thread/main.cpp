#include <aRibeiroCore/aRibeiroCore.h>
#include <aRibeiroPlatform/aRibeiroPlatform.h>

using namespace aRibeiro;

#include <stdio.h>

//
// Test: PlatformTime PlatformSleep
//
void test_PlatformTime_and_PlatformSleep() {

    printf("---------------------------------------\n");
    printf("  test_PlatformTime_and_PlatformSleep\n");
    printf("---------------------------------------\n");

    PlatformTime time;

    time.timeScale = 0.5f;
    printf("time.timeScale = 0.5f\n");

    time.update();
    PlatformSleep::sleepMillis(5);
    time.update();

#if !defined(OS_TARGET_win)

    printf("PlatformSleep::sleepMillis(5): %lld\n", time.deltaTimeMicro);

#else

    printf("PlatformSleep::sleepMillis(5): %I64d\n", time.deltaTimeMicro);

#endif




    time.update();
    PlatformSleep::busySleepMicro(300);
    time.update();

#if !defined(OS_TARGET_win)
    printf("PlatformSleep::busySleepMicro(300): %lld\n", time.deltaTimeMicro);
#else
    printf("PlatformSleep::busySleepMicro(300): %I64d\n", time.deltaTimeMicro);
#endif


    time.update();
    PlatformSleep::sleepMillis(2000);
    time.update();

    printf("time.deltaTime (2 secs): %f\n", time.deltaTime);
    printf("time.unscaledDeltaTime (2 secs): %f\n", time.unscaledDeltaTime);

    printf("\n");
}
//
// Test: PlatformThread PlatformMutex
//
volatile int sharedCounter = 0;

void Thread1_IncrementCounter() {
    sharedCounter = 0;
    printf("Thread 1 Start...\n");
    PlatformSleep::sleepMillis(3000);
    printf("Thread 1 SharedCounter Increment...\n");
    sharedCounter++;
    PlatformSleep::sleepMillis(3000);
    printf("Thread 1 SharedCounter Increment...\n");
    sharedCounter++;
    PlatformSleep::sleepMillis(3000);
    printf("Thread 1 SharedCounter Increment...\n");
    sharedCounter++;
    PlatformSleep::sleepMillis(3000);
    printf("Thread 1 End...\n");
}

void Thread2_CheckCounter() {
    int localCounter = 0;

    printf("  Thread 2 Start...\n");

    while (localCounter < 3) {
        if (localCounter < sharedCounter) {
            printf("  Thread 2 Detected SharedCounter modification...\n");
            printf("  Thread 2 LocalCounter Increment...\n");
            localCounter++;
        }
        //avoid 100% CPU using by this thread
        PlatformSleep::sleepMillis(1);
    }
    printf("  Thread 2 End...\n");
}

void Thread3_Interrupt() {

    printf("Thread3_Interrupt Start...\n");
    printf("Thread3_Interrupt waiting interrupt from another thread...\n");

    while (!PlatformThread::isCurrentThreadInterrupted()) {
        //avoid 100% CPU using by this thread
        PlatformSleep::sleepMillis(3000);
    }

    printf("Thread3_Interrupt Interrupt Detected !!!\n");
    printf("Thread3_Interrupt End...\n");
}

PlatformMutex mutex;

void Thread4_Mutex() {
    printf("Thread4_Mutex Start...\n");

    mutex.lock();

    printf("Thread4_Mutex Critical Section Enter Success !!!\n");

    mutex.unlock();

    printf("Thread4_Mutex End...\n");
}

void test_PlatformThread_and_PlatformMutex() {

    printf("---------------------------------------\n");
    printf("  test_PlatformThread\n");
    printf("---------------------------------------\n");

    {
        PlatformThread t1(&Thread1_IncrementCounter);
        PlatformThread t2(&Thread2_CheckCounter);

        t1.start();
        t2.start();
    }
    printf("\n");

    printf("  InterruptTest\n");
    printf("----------------\n");
    {
        PlatformThread t3(&Thread3_Interrupt);
        t3.start();


        PlatformSleep::sleepMillis(4000);
        printf("  MainThread: Interrupt and Waiting thread To Finish!!!\n");
        t3.interrupt();
        t3.wait();
        printf("  MainThread: Thread Finish Detected!!!\n");

    }


    printf("\n");

    printf("  MutexTest\n");
    printf("----------------\n");
    {
        PlatformThread t3(&Thread4_Mutex);

        printf("  MainThread: Mutex lock before thread start\n");
        mutex.lock();

        t3.start();

        PlatformSleep::sleepSec(3);

        printf("  MainThread: Mutex unlock\n");
        mutex.unlock();

    }

    printf("\n");
}

//
// Test: PlatformPath
//
void test_PlatformPath(char* argv0) {
    printf("---------------------------------------\n");
    printf("  test_PlatformPath\n");
    printf("---------------------------------------\n");

    std::string saveGamePath = PlatformPath::getSaveGamePath("CompanyName", "GameName");
    printf("  SaveGamePath: %s\n", saveGamePath.c_str());

    std::string exePath = PlatformPath::getExecutablePath(argv0);
    std::string workPath = PlatformPath::getWorkingPath();

    printf("  exePath: %s\n", exePath.c_str());
    printf("  workPath: %s\n", workPath.c_str());

    printf(" changeWorkPath!!!\n");
    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv0));

    workPath = PlatformPath::getWorkingPath();
    printf("  new workPath: %s\n", workPath.c_str());

    printf("\n");
}

//
// Test: Util test
//
void test_Util() {
    printf("---------------------------------------\n");
    printf("  test_Util\n");
    printf("---------------------------------------\n");

    printf("\n");

    printf(" getFirstMacAddress\n");
    getFirstMacAddress();

    printf("\n");
}


int main(int argc, char* argv[]) {


    printf("  press any key to do path test...\n"); fgetc(stdin);
    test_PlatformPath(argv[0]);
    printf("  press any key to do time and sleep test...\n"); fgetc(stdin);
    test_PlatformTime_and_PlatformSleep();
    printf("  press any key to do thread and mutex test...\n"); fgetc(stdin);
    test_PlatformThread_and_PlatformMutex();
    printf("  press any key to do util test...\n"); fgetc(stdin);
    test_Util();


    printf("  press any key to exit...\n");
    fgetc(stdin);

    return 0;
}
