#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <thread>
#include "Signalling.h"

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

// Create a server endpoint
server signalling_server;
std::thread* signalling_server_thread;

bool IsClientConnected = false;
websocketpp::connection_hdl client_connection_hdl;
server::connection_ptr client_connection_ptr;

void on_websocket_connect(server* s, websocketpp::connection_hdl hdl){
    if(IsClientConnected){
        server::connection_ptr con = signalling_server.get_con_from_hdl(client_connection_hdl);
        con->close(websocketpp::close::status::blank, "Client Already Connected");
        return;
    }
    std::cout << "on connect " << std::endl;
    client_connection_hdl = hdl;
    client_connection_ptr = signalling_server.get_con_from_hdl(client_connection_hdl);
    IsClientConnected = true;
}

void on_websocket_disconnect(server* s, websocketpp::connection_hdl hdl){
    std::cout << "on disconnect " << std::endl;
    IsClientConnected = false;
}

// Define a callback to handle incoming messages
void on_websocket_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;

    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
    if (msg->get_payload() == "stop-listening") {
        s->stop_listening();
        return;
    }
}

void Signalling::StartServer(int port){
    try {
        // Set logging settings
        signalling_server.set_access_channels(websocketpp::log::alevel::all);
        signalling_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        signalling_server.init_asio();

        // Register our message handler
        signalling_server.set_open_handler(bind(&on_websocket_connect, &signalling_server, ::_1));
        signalling_server.set_message_handler(bind(&on_websocket_message, &signalling_server, ::_1, ::_2));
        signalling_server.set_close_handler(bind(&on_websocket_disconnect, &signalling_server, ::_1));

        signalling_server.listen(port);

        // Start the server accept loop
        signalling_server.start_accept();

        signalling_server_thread = new std::thread([](){
            // Start the ASIO io_service run loop
            signalling_server.run();
        });
        std::printf("Signalling Server Started!\n");
    } catch (websocketpp::exception const & e) {
        StopServer();
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
}


void Signalling::StopServer(){
    signalling_server.stop();
    signalling_server_thread->join();
    delete signalling_server_thread;
    std::printf("Signalling Server stopped!\n");
}

void Signalling::SendMessage(std::string& msg){
    if(client_connection_hdl.expired())
        return;
    client_connection_ptr->send(msg, websocketpp::frame::opcode::text);
}