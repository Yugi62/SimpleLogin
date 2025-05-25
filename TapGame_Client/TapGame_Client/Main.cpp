#include <thread>
#include "Client.h"
#include "Gui.h"

int main()
{
    //Client)  
    boost::asio::io_context io;
    Client client(io);
    std::thread clientThread;
    try{clientThread = std::thread([&io]() {io.run(); });}
    catch (const std::exception& e)
    {
        //std::cout << "Client Error : " << e.what() << std::endl;
    }


    //imgui
    Gui gui(client);
    gui.Run();


    //Cleanup
    clientThread.join();    

    return 0;
}