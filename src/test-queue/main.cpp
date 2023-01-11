#include <aRibeiroCore/aRibeiroCore.h>
#include <aRibeiroPlatform/aRibeiroPlatform.h>
using namespace aRibeiro;

ObjectQueue<int> queue;

void thread(int n) {
    bool isSignaled;
    printf("start thread %i\n", n);
    while ( !PlatformThread::getCurrentThread()->isCurrentThreadInterrupted() ) {
        int v = queue.dequeue(&isSignaled);
        if (isSignaled)
            break;
        printf("[thread %i]: %i\n", n, v);
        PlatformSleep::sleepMillis( Random::getRange(300,500) );
    }
    printf("end thread %i\n", n);
}

void signal_handler(int signal) {
    printf("   ****************** signal_handler **********************\n");

    //PlatformThread::getMainThread()->interrupt();
    ARIBEIRO_ABORT(true, "Abort Test");
}

int main(int argc, char* argv[]){
    PlatformSignal::Set(signal_handler);

    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));
    // initialize self referencing of the main thread.
    PlatformThread::getMainThread();

    aRibeiro::PlatformThread *thread_[] = {
        new aRibeiro::PlatformThread(&thread, 1),
        new aRibeiro::PlatformThread(&thread, 2),
        new aRibeiro::PlatformThread(&thread, 3),
        new aRibeiro::PlatformThread(&thread, 4),
        new aRibeiro::PlatformThread(&thread, 5)
    };

    for(int i=0;i<5;i++)
        thread_[i]->start();

    printf("--- Type any number to the Thread (-1 to exit) ---\n");

    char data[1024];
    while (true) {

        char* readed_size = fgets(data, 1024, stdin);
        int i = atoi(data);

        if (i == -1)
            break;

        for(int j=0;j<5;j++)
            queue.enqueue( i + j );
    }

    // end all threads
    for(int i=0;i<5;i++)
        thread_[i]->interrupt();
    for(int i=0;i<5;i++)
        thread_[i]->wait();
    for(int i=0;i<5;i++)
        aRibeiro::setNullAndDelete(thread_[i]);

    return 0;
}
