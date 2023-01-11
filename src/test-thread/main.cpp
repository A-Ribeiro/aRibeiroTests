#include <aRibeiroCore/aRibeiroCore.h>
#include <aRibeiroPlatform/aRibeiroPlatform.h>

using namespace aRibeiro;

#include <stdio.h>
#include <inttypes.h>//PRId64

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

    printf("PlatformSleep::sleepMillis(5): %" PRId64 "\n", time.deltaTimeMicro);

    time.update();
    PlatformSleep::busySleepMicro(300);
    time.update();

    printf("PlatformSleep::busySleepMicro(300): %" PRId64 "\n", time.deltaTimeMicro);

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
    if (PlatformThread::isCurrentThreadInterrupted())
        return;
    PlatformSleep::sleepMillis(3000);
    printf("Thread 1 SharedCounter Increment...\n");
    sharedCounter++;
    if (PlatformThread::isCurrentThreadInterrupted())
        return;
    PlatformSleep::sleepMillis(3000);
    printf("Thread 1 SharedCounter Increment...\n");
    sharedCounter++;
    if (PlatformThread::isCurrentThreadInterrupted())
        return;
    PlatformSleep::sleepMillis(3000);
    printf("Thread 1 End...\n");
}

void Thread2_CheckCounter() {
    int localCounter = 0;

    printf("  Thread 2 Start...\n");

    while (!PlatformThread::isCurrentThreadInterrupted() && localCounter < 3) {
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

    if (PlatformThread::isCurrentThreadInterrupted())
        return;

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

    if (PlatformThread::isCurrentThreadInterrupted())
        return;

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

    if (PlatformThread::isCurrentThreadInterrupted())
        return;
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

void signal_handler(int signal) {
    printf("   ****************** signal_handler **********************\n");

    //PlatformThread::getMainThread()->interrupt();
    ARIBEIRO_ABORT(true, "Abort Test");
}

int main(int argc, char* argv[]) {

    PlatformSignal::Set(signal_handler);

    printf("\nNumber of system threads: %i\n\n", PlatformThread::QueryNumberOfSystemThreads());

    printf("  press any key to do path test...\n"); fgetc(stdin);
    test_PlatformPath(argv[0]);
    printf("  press any key to do time and sleep test...\n"); fgetc(stdin);
    test_PlatformTime_and_PlatformSleep();
    printf("  press any key to do thread and mutex test...\n"); fgetc(stdin);
    if (PlatformThread::isCurrentThreadInterrupted())
        return 0;
    test_PlatformThread_and_PlatformMutex();
    printf("  press any key to do util test...\n"); fgetc(stdin);
    if (PlatformThread::isCurrentThreadInterrupted())
        return 0;
    test_Util();
    if (PlatformThread::isCurrentThreadInterrupted())
        return 0;
    printf("  press any key to exit...\n");
    fgetc(stdin);

    return 0;
}
