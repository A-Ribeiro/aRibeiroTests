#include <aRibeiroCore/aRibeiroCore.h>
#include <aRibeiroPlatform/aRibeiroPlatform.h>
using namespace aRibeiro;

void signal_handler(int signal) {
    printf("   ****************** signal_handler **********************\n");

    PlatformThread::getMainThread()->interrupt();
}

void thread_cancel_read(int sleep_time) {
    PlatformSleep::sleepMillis(sleep_time);
    printf("   ****************** cancel reading!!! **********************\n");
    PlatformThread::getMainThread()->interrupt();
}


int main(int argc, char* argv[]){
    setGlobalThreadPriority(GlobalThreadPriority_Realtime);
    PlatformSignal::Set( signal_handler );
    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));
    printf("TEST Run Child Process\n"
        "\n"
        "For this is how God loved the world :\n"
        "he gave his only Son, so that everyone\n"
        "who believes in him may not perish\n"
        "but may have eternal life.\n"
        "\n"
        "John 3:16\n\n");


    std::string stdout_child_str_output;



    PlatformThread* thread = new PlatformThread(thread_cancel_read, 2000);
    thread->start();

#if defined(_WIN32)
    
    WindowsPipe child_stdout;
    PlatformProcess process("tst.bat", StringUtil::parseArgv("\"string from app\""), INT32_MAX,
        NULL,&child_stdout,NULL
    );

    std::vector<uint8_t> raw_output;
    aRibeiro::ObjectBuffer buffer;
    while (!child_stdout.isReadSignaled())
    {
        printf("[");
        fflush(stdout);
        if (child_stdout.read(&buffer))
            raw_output.insert(raw_output.end(), &buffer.data[0], &buffer.data[buffer.size]);

        printf("]");
        fflush(stdout);
    }

    raw_output.push_back(0);
    stdout_child_str_output = aRibeiro::StringUtil::trim((char*)raw_output.data());

#else
#endif


    

    int exit_code = 0;
    printf("waiting process to exit...\n");
    if (process.waitExit(&exit_code, UINT32_MAX)) {
        printf("Application Exit Detected:\n"
            "    ExitCode: %i\n"
            "    Output: %s\n", 
            exit_code, 
            stdout_child_str_output.c_str());
    }
    else {
        printf("Application Exit Detection Error.");
    }


    delete thread;
    return 0;
}
