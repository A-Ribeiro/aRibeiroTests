#include <aRibeiroCore/aRibeiroCore.h>
#include <aRibeiroPlatform/aRibeiroPlatform.h>
using namespace aRibeiro;

void signal_handler(int signal) {
    printf("   ****************** signal_handler **********************\n");
    PlatformThread::getMainThread()->interrupt();
}

void client_thread() {

    printf("CLIENT THREAD\n");

    PlatformSocketUDP socket_write;
    socket_write.createFD(false, true, 1);
    socket_write.bind();// ephemeral port and address...


    struct sockaddr_in server_addr = SocketUtils::mountAddress("127.0.0.1", 5002);

    char buffer[] = "UDP Send.";

    int count = 0;

    while (!PlatformThread::isCurrentThreadInterrupted()){
        uint32_t bytes_written;
        if (socket_write.write_buffer(server_addr,
            (uint8_t*)buffer, strlen(buffer) + 1,
            &bytes_written)) {
            
            //printf("[client] data written. bytes_written: %i, count:%i\n", bytes_written, count++);
        }
        else {
            if (socket_write.isSignaled())
                break;

            //printf("[client] data not sent... bytes_written: %i, count:%i\n", bytes_written, count++);

            printf(".write error.(%i)\n", count++);
            PlatformSleep::sleepMillis(300);
        }

        {
            char buffer[NetworkConstants::UDP_DATA_MTU_MINIMUM];
            struct sockaddr_in server_addr_copy = server_addr;
            uint32_t readed_bytes;
            if (socket_write.read_buffer(&server_addr_copy, (uint8_t*)buffer, NetworkConstants::UDP_DATA_MTU_MINIMUM, &readed_bytes)) {
                buffer[readed_bytes] = '\0';
                printf("    (%i) Data from Server -> %s\n", count++, buffer);
            }
            else {
                printf(".no data from server.(%i)\n", count++);
                PlatformSleep::sleepMillis(300);
            }
        }
    }

    socket_write.close();
    
}

void server_thread() {

    printf("SERVER THREAD\n");

    PlatformSocketUDP socket_server;
    socket_server.createFD(true, true, 1);
    socket_server.bind("INADDR_ANY", 5002);


    struct sockaddr_in in_addr = { 0 };

    char buffer[ NetworkConstants::UDP_DATA_MTU_MINIMUM];

    int count = 0;

    while (!PlatformThread::isCurrentThreadInterrupted()) {
        uint32_t bytes_readed;
        if (socket_server.read_buffer(&in_addr,
            (uint8_t*)buffer, NetworkConstants::UDP_DATA_MTU_MINIMUM,
            &bytes_readed)) {

            //printf("[server] data received. bytes_readed: %i, count:%i\n", bytes_readed, count++);
            
            buffer[bytes_readed] = '\0';
            printf("    Data from Client(%i) -> %s\n", count++, buffer);

            char hello_world[] = "Hello World!!!";
            if (!socket_server.write_buffer(in_addr, (uint8_t*)hello_world, strlen(hello_world) + 1)) {

                printf("[server] WRITE ERROR!!! waiting... (%i) \n", count++);
                PlatformSleep::sleepMillis(10000);
            }

        }
        else {
            if (socket_server.isSignaled())
                break;
            //printf("[server] no data. bytes_readed: %i, count:%i\n", bytes_readed, count++);
            PlatformSleep::sleepMillis(300);
        }
    }

    socket_server.close();

}

int main(int argc, char* argv[]) {
    PlatformSignal::Set(signal_handler);

    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));
    // initialize self referencing of the main thread.
    PlatformThread::getMainThread();

    printf("UDP SOCKET TEST\n"
        "\n"
        "For this is how God loved the world :\n"
        "he gave his only Son, so that everyone\n"
        "who believes in him may not perish\n"
        "but may have eternal life.\n"
        "\n"
        "John 3:16\n\n");

    //std::string path = PlatformPath::getDocumentsPath("OpenGLStarter", "RTMPBox");
    //std::string file_path = path + PlatformPath::SEPARATOR + std::string("config.cfg");

    if (argc == 2 && (strcmp(argv[1], "client") == 0)) {
        client_thread();
        //PlatformThread thread(client_thread);
        //thread.start();
    }
    else
    {
        server_thread();
        //PlatformThread thread(server_thread);
        //thread.start();
    }

    printf("main end.\n");
    PlatformSignal::Reset();
    return 0;
}
