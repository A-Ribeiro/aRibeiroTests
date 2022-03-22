#include <aRibeiroCore/aRibeiroCore.h>
#include <aRibeiroPlatform/aRibeiroPlatform.h>
using namespace aRibeiro;

struct GlobalInfo {
    uint32_t instance_count;
};

bool to_exit = false;

void signal_handler(int signal) {
    printf("   ****************** signal_handler **********************\n");
    to_exit = true;

    PlatformThread::getMainThread()->interrupt();
}

int main(int argc, char* argv[]){
    PlatformSignal::Set( signal_handler );
    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));
    // initialize self referencing of the main thread.
    PlatformThread::getMainThread();

    printf("TEST InterProcess Communication\n"
        "\n"
        "For this is how God loved the world :\n"
        "he gave his only Son, so that everyone\n"
        "who believes in him may not perish\n"
        "but may have eternal life.\n"
        "\n"
        "John 3:16\n\n");

    //std::string path = PlatformPath::getDocumentsPath("OpenGLStarter", "RTMPBox");
    //std::string file_path = path + PlatformPath::SEPARATOR + std::string("config.cfg");

    
    PlatformBufferIPC global_info_ipc("app_name", sizeof(GlobalInfo));
    {
        GlobalInfo *globalInfo = (GlobalInfo *)global_info_ipc.data;
        if (global_info_ipc.isFirstProcess()) {
            // initialize the global data
            globalInfo->instance_count = 1;
        }
        else {
            // update the global data
            globalInfo->instance_count++;
        }
        printf("Global Initialization (count: %i)\n", globalInfo->instance_count);
        fflush(stdout);
        global_info_ipc.finishInitialization();
    }

    if ( argc == 2 && argv[1][0] == 'w' ) {
        printf("Openning a queue (normal write)\n");
        PlatformQueueIPC queue("default", PlatformQueueIPC_WRITE, 8, sizeof(float)*2);

        ObjectBuffer buffer;

        buffer.setSize(sizeof(float)*2);
        float *data = (float *)buffer.data;
        data[0] = 0.0f;
        data[1] = 100.0f;

        uint32_t count = 0;
        fflush(stdout);
        while (!to_exit) {
            //PlatformSleep::sleepMillis(500);
            printf("writing: %f %f (%u) ... ", data[0], data[1], count++);
            fflush(stdout);
            if (!queue.write(buffer))
                break;
            printf("     done\n");
            data[0] += 0.001f;
            data[1] += 0.002f;

            //queue.printStats();
        }

    }
    else if ( argc == 2 && argv[1][0] == 'r' ) {
        printf("Openning a queue (normal read)\n");
        PlatformQueueIPC queue("default", PlatformQueueIPC_READ, 8, sizeof(float)*2);

        ObjectBuffer buffer;

        uint32_t count = 0;
        fflush(stdout);
        while (!to_exit) {
            if (!queue.read( &buffer ))
                break;
            ARIBEIRO_ABORT(buffer.size != 8, "error to get value from the queue...\n");

            float *data = (float *)buffer.data;
            printf("reading: %f %f (%u)\n", data[0], data[1], count++);
            fflush(stdout);
        }

    } else if ( argc == 2 && ( strcmp ( argv[1], "low_latency_w") == 0) ) {
        printf("Openning a queue (low latency write)\n");
        PlatformLowLatencyQueueIPC queue("default", PlatformQueueIPC_WRITE, 8, sizeof(float)*2);

        ObjectBuffer buffer;

        buffer.setSize(sizeof(float)*2);
        float *data = (float *)buffer.data;
        data[0] = 0.0f;
        data[1] = 100.0f;

        uint32_t count = 0;
        fflush(stdout);
        while (!to_exit) {
            //PlatformSleep::sleepMillis(500);
            printf("writing: %f %f (%u) ... ", data[0], data[1], count++);
            fflush(stdout);
            if (!queue.write(buffer))
                break;
            printf("     done\n");
            data[0] += 0.001f;
            data[1] += 0.002f;

            //queue.printStats();
        }

    }
    else if ( argc == 2 &&  ( strcmp ( argv[1], "low_latency_r") == 0) ) {
        printf("Openning a queue (low latency read)\n");
        PlatformLowLatencyQueueIPC queue("default", PlatformQueueIPC_READ, 8, sizeof(float)*2);

        ObjectBuffer buffer;

        uint32_t count = 0;
        fflush(stdout);
        while (!to_exit) {
            bool has_data = queue.read( &buffer );
            if (queue.isSignaled())
                break;

            ARIBEIRO_ABORT(!has_data, "Error, the queue returned without any data...\n");
            ARIBEIRO_ABORT(buffer.size != 8, "error to get value from the queue...\n");

            float *data = (float *)buffer.data;
            printf("reading: %f %f (%u)\n", data[0], data[1], count++);
            fflush(stdout);
        }

    }
    
    {
        global_info_ipc.lock();
        GlobalInfo *globalInfo = (GlobalInfo *)global_info_ipc.data;
        globalInfo->instance_count -= 1;
        printf("Global Finalization (count: %i)\n", globalInfo->instance_count);
        fflush(stdout);
        global_info_ipc.unlock();
    }

    printf("main end.\n");

    return 0;
}
