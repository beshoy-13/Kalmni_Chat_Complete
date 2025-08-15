#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <memory>

using boost::asio::ip::tcp;
using namespace std;

const unsigned short PORT = 8080;
vector<shared_ptr<tcp::socket>> clients;
mutex clients_mutex;
atomic<bool> running(true);

void broadcast(const string& msg, shared_ptr<tcp::socket> sender_socket) {
    lock_guard<mutex> lock(clients_mutex);
    for (auto& client : clients) {
        if (client == sender_socket) continue;
        boost::asio::write(*client, boost::asio::buffer(msg));
    }
}

void handle_client(shared_ptr<tcp::socket> socket) {
    try {
        while (running) {
            char buffer[1024];
            boost::system::error_code error;
            size_t bytes = socket->read_some(boost::asio::buffer(buffer), error);

            if (error == boost::asio::error::eof) break;
            if (error) throw boost::system::system_error(error);

            buffer[bytes] = '\0';
            cout << "[User " << socket->remote_endpoint() << "] " << buffer;

            broadcast(string(buffer), socket);
        }
    } catch (exception& e) {
        cerr << "Client error: " << e.what() << endl;
    }

    {
        lock_guard<mutex> lock(clients_mutex);
        clients.erase(remove(clients.begin(), clients.end(), socket), clients.end());
    }
    cout << "User disconnected: " << socket->remote_endpoint() << endl;
}

void on_sigint(int) {
    running = false;
    cout << "\nShutting down server..." << endl;
}

int main() {
    signal(SIGINT, on_sigint);

    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), PORT));

    cout << "Kalmni server started on port " << PORT << "..." << endl;

    while (running) {
        auto socket = make_shared<tcp::socket>(io_context);
        acceptor.accept(*socket);

        {
            lock_guard<mutex> lock(clients_mutex);
            clients.push_back(socket);
        }
        cout << "New client connected: " << socket->remote_endpoint() << endl;
        thread(handle_client, socket).detach();
    }

    {
        lock_guard<mutex> lock(clients_mutex);
        for (auto& client : clients) client->close();
        clients.clear();
    }
    return 0;
}
