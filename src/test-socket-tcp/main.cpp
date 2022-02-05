#include <aRibeiroCore/aRibeiroCore.h>
#include <aRibeiroPlatform/aRibeiroPlatform.h>
using namespace aRibeiro;

void signal_handler(int signal) {
    printf("   ****************** signal_handler **********************\n");
    PlatformThread::getMainThread()->interrupt();
}

bool use_blocking_sockets = true;

void receive_thread(PlatformSocketTCP *socket) {
    
    socket->setNoDelay(true);

    //valid client socket
    printf("New Client!!!\n");
    printf("          TCP Client Addr: %s\n", inet_ntoa(socket->getAddr().sin_addr));
    printf("                     Port: %u\n", ntohs(socket->getAddr().sin_port));


    // handshake... read 10 characters
    printf("Handshake init...\n");

    char initial_string[10] = {0};
    uint32_t read_feedback;
    if (!socket->read_buffer((uint8_t*)initial_string, 10, &read_feedback)) {
        printf("Connection or thread interrupted with the read feedback: %u\n", read_feedback);
        
        socket->close();
        delete socket;
        printf("   Client Thread End.\n");
        return;
    }

    if (strncmp(initial_string, "init", 4) != 0) {
        
        printf("Handshake failed...\n");

        socket->close();
        delete socket;
        printf("   Client Thread End.\n");
        return;
    }

    char response[] = "Handshake OK!!!\r\n";

    uint32_t write_feedback;
    socket->write_buffer(
        (uint8_t*)response,
        strlen(response)+1,
        &write_feedback
    );

    printf("Handshake OK!!!: Write FeedBack: %u\n", write_feedback);

    char command[] = "exit";
    int index = 0;
    char input;
    //while (socket->read_uint8((uint8_t*)&input)) {
    while (!PlatformThread::isCurrentThreadInterrupted()) {
        if (use_blocking_sockets) {
            if (!socket->read_uint8((uint8_t*)&input))
                break;
        }
        else {
            if (!socket->read_uint8((uint8_t*)&input, false)) {
                if (socket->isSignaled() || socket->isClosed())
                    break;
                printf(".");
                fflush(stdout);
                PlatformSleep::sleepMillis(1000);
                continue;
            }
        }

        //printf("%c", input);
        char txt[2] = { input , '\0'};
        printf("%s", txt);

        if (input == command[index])
            index++;
        else
            index = 0;
        if (index == strlen(command))
            break;
    }

    printf("\n");

    //PlatformSleep::sleepMillis(3000);
    socket->close();
    delete socket;
    printf("   Client Thread End.\n");
}

int main(int argc, char* argv[]) {
    PlatformSignal::Set( signal_handler );

    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));
    // initialize self referencing of the main thread.
    PlatformThread::getMainThread();

    printf("SOCKET TEST\n"
        "\n"
        "For this is how God loved the world :\n"
        "he gave his only Son, so that everyone\n"
        "who believes in him may not perish\n"
        "but may have eternal life.\n"
        "\n"
        "John 3:16\n\n");

    //std::string path = PlatformPath::getDocumentsPath("OpenGLStarter", "RTMPBox");
    //std::string file_path = path + PlatformPath::SEPARATOR + std::string("config.cfg");

    if (argc == 3 && (strcmp(argv[1], "connect") == 0)) {
        const char* addr = argv[2];

        //if (argc == 1) {
        //const char* addr = "127.0.0.1";

        PlatformSocketTCP clientSocket;

        if (!clientSocket.connect(addr, NetworkConstants::PUBLIC_PORT_START)) {
            printf("Connect failed!!!... interrupting current thread\n");
            PlatformThread::getCurrentThread()->interrupt();
        }

        clientSocket.setNoDelay(true);

        char init[10] = "init";
        printf("init size: %lu\n",sizeof(init));
        if (!clientSocket.write_buffer((uint8_t*)&init, 10)) {
            printf("Failed to send handshake...\n");
            PlatformThread::getCurrentThread()->interrupt();
        }

        char response[18];
        uint32_t read_feedback;
        if (!clientSocket.read_buffer((uint8_t*)&response, 18, &read_feedback)) {
            printf("Failed to receive handshake...read feedback: %u\n", read_feedback);
            printf("Thread interrupted: %i\n", PlatformThread::isCurrentThreadInterrupted());
            PlatformThread::getCurrentThread()->interrupt();
        } else
            printf("Connected!!!\n%s\n", response);
        
        while (!PlatformThread::isCurrentThreadInterrupted() && !clientSocket.isSignaled()) {

            int c = EOF;

#if defined(_WIN32)
            if (WaitForSingleObject(GetStdHandle(STD_INPUT_HANDLE), 0) == WAIT_OBJECT_0) {
                //key press...
                INPUT_RECORD irInBuf[1];
                DWORD cNumRead;
                ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), irInBuf, 1, &cNumRead);
                if (cNumRead == 1 &&
                    irInBuf->EventType == KEY_EVENT &&
                    irInBuf->Event.KeyEvent.bKeyDown) {

                    char txt[2];
                    txt[0] = irInBuf->Event.KeyEvent.uChar.AsciiChar;
                    txt[1] = '\0';

                    if (txt[0] == 13)
                        txt[0] = '\n';

                    printf("%s", txt);

                    c = txt[0];
                }
            }
#else
            c = getc(stdin);
#endif
            
            uint8_t chr_net;
            clientSocket.read_uint8(&chr_net, false);// always try to read a char...

            if (c == EOF) {
                //printf(".");
                PlatformSleep::sleepMillis(50);
                continue;
            }

            if (!clientSocket.write_buffer((uint8_t*)&c, 1)) {
                if (clientSocket.isSignaled() || clientSocket.isClosed())
                    break;
                //PlatformSleep::sleepMillis(1000);
            }
        }

        clientSocket.close();

    }
    else if (
        (argc == 1)||
        (argc == 2 && (strcmp(argv[1], "server") == 0))  ||
        (argc == 3 && (strcmp(argv[1], "server") == 0))
        ) {


        if (argc == 2 && (strcmp(argv[1], "non-block") == 0))
            use_blocking_sockets = false;
        if (argc == 3 && (strcmp(argv[2], "non-block") == 0))
            use_blocking_sockets = false;

        printf("use_blocking_sockets:%i\n", use_blocking_sockets);

        PlatformSocketTCPAccept serverSocket(
            use_blocking_sockets, //blocking
            true, //reuseAddress
            true //noDelay
        );
        if (!serverSocket.bindAndListen(
            "INADDR_ANY", // interface address
            NetworkConstants::PUBLIC_PORT_START, // port
            10 // input queue
        )) {
            PlatformThread::getCurrentThread()->interrupt();
        }

        //PlatformTCPSocket clientSocket;

        PlatformSocketTCP *clientSocket = new PlatformSocketTCP();

        std::vector<PlatformThread*> threads;

        while (!PlatformThread::isCurrentThreadInterrupted()) {

            if (use_blocking_sockets) {
                // blocking mode
                if (!serverSocket.accept(clientSocket)) {
                    if (serverSocket.isSignaled() || serverSocket.isClosed() || !serverSocket.isListening())
                        break;
                    continue;
                }
            }
            else {
                // non-blocking mode
                if (!serverSocket.accept(clientSocket)) {
                    printf("*");
                    fflush(stdout);
                    PlatformSleep::sleepMillis(1000);
                    continue;
                }
            }

            if (threads.size() > 0) {
                //clear deleted threads
                for (int i = threads.size() - 1; i >= 0; i--) {
                    if (!threads[i]->isAlive()) {
                        printf("removing closed thread...\n");
                        threads[i]->interrupt();
                        threads[i]->wait();
                        delete threads[i];
                        threads.erase(threads.begin() + i);
                    }
                }
            }

            PlatformThread *aux = new PlatformThread(&receive_thread, clientSocket);
            threads.push_back(aux);
            aux->start();

            clientSocket = new PlatformSocketTCP();
        }

        serverSocket.close();

        delete clientSocket;

        for (size_t i = 0; i < threads.size(); i++) {
            threads[i]->interrupt();
            threads[i]->wait();
            delete threads[i];
        }
        threads.clear();

    }

    printf("main end.\n");
    
    PlatformSignal::Reset();

    return 0;
}
