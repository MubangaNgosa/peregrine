#include "zmq.hpp"
#include <iostream>
#include <string>
#include <csignal>
#include <chrono>
#include <thread>
#include "../DataGraph.hh"
#include "jobPacket.h"

volatile std::sig_atomic_t terminate_flag = 0;

void signal_handler(int /* signal */) {
    terminate_flag = 1;
}

int main() {
    // Set up signal handling for graceful shutdown
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // Initialize ZeroMQ context and sockets
    zmq::context_t context(1);
    zmq::socket_t pull_socket(context, ZMQ_PULL);
    pull_socket.connect("tcp://127.0.0.1:5555");

    zmq::socket_t push_socket(context, ZMQ_PUSH);
    push_socket.connect("tcp://127.0.0.1:5556");

    // Send initial ready message
    zmq::message_t ready("Ready", 5);
    push_socket.send(ready.data(), ready.size());

    while (!terminate_flag) {
        zmq::message_t message;
        if (pull_socket.recv(&message)) {
            std::string received_data(static_cast<char*>(message.data()), message.size());
            size_t delimiterPos = received_data.find('|');
            if (delimiterPos != std::string::npos) {
                std::string serializedPacket = received_data.substr(delimiterPos + 1);
                Peregrine::JobPacket jobPacket = Peregrine::JobPacket::deserialize(serializedPacket);

                // Debugging: Print the received job packet data
                std::cout << "Received job packet:\n";
                std::cout << "  Task ID: " << jobPacket.taskId << "\n";
                std::cout << "  Vertices:";
                for (const auto& vertex : jobPacket.vertices) {
                    std::cout << " " << vertex;
                }
                std::cout << "\n  Edges:";
                for (const auto& edge : jobPacket.edges) {
                    std::cout << " (" << edge.first << "," << edge.second << ")";
                }
                std::cout << std::endl;

                // Process the job packet
                // ... (Insert your processing logic here)

                // Send a readiness signal after processing the job packet
                zmq::message_t ready_signal("Ready", 5);
                push_socket.send(ready_signal.data(), ready_signal.size());
            }
        }
    }

    return 0;
}
