#include <aRibeiroCore/aRibeiroCore.h>
#include <aRibeiroPlatform/aRibeiroPlatform.h>
using namespace aRibeiro;
#include <algorithm>

void signal_handler(int signal)
{
    printf("   ****************** signal_handler **********************\n");

    PlatformThread::getMainThread()->interrupt();
}

template <typename T>
void check_sorting(const std::vector<T> &array)
{
    printf("    check_sorting: ");
    for (int i = 0; i < array.size() - 1; i++)
    {
        if (array[i] > array[i + 1])
        {
            printf("ERROR\n\n");
            return;
        }
    }
    printf("OK\n\n");
}

void CustomAbortFNC(const char* file, int line, const char* format, ...) {
        
    va_list args;

    // std::vector<char> char_buffer;
    // va_start(args, format);
    // char_buffer.resize(vsnprintf(NULL, 0, format, args) + 1);
    // va_end(args);

    // va_start(args, format);
    // int len = vsnprintf(&char_buffer[0], char_buffer.size(), format, args);
    // va_end(args);

    fprintf(stderr, "CUSTOM_ABORT_FNC\n");
    va_start(args, format);
    aRibeiro::DefaultAbortFNC(file,line,format,args);
    va_end(args);
    exit(-1);
}

void SignalAbortFNC1(const char* file, int line, const char* message){
    
    fprintf(stderr, "SignalAbortFNC1\n");

    //ARIBEIRO_ABORT(true, "Second level error...");

}

void SignalAbortFNC2(const char* file, int line, const char* message){
    
    fprintf(stderr, "SignalAbortFNC2\n");

}

int main(int argc, char *argv[])
{

    setGlobalThreadPriority(GlobalThreadPriority_Realtime);

    PlatformSignal::Set(signal_handler);
    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));
    // initialize self referencing of the main thread.
    PlatformThread::getMainThread();

    printf("TEST OnAbort FNC\n"
           "\n"
           "For this is how God loved the world :\n"
           "he gave his only Son, so that everyone\n"
           "who believes in him may not perish\n"
           "but may have eternal life.\n"
           "\n"
           "John 3:16\n\n");

    // first method:
    //aRibeiro::OnAbortFNC = CustomAbortFNC;

    // second method:
    PlatformSignal::OnAbortEvent()->add(SignalAbortFNC1);
    PlatformSignal::OnAbortEvent()->add(SignalAbortFNC2);

    ARIBEIRO_ABORT(true, "Custom Exception Test Message.");

    printf("normal exit code\n");

    return 0;
}
