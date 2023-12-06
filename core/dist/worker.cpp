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
    push_socket.send(ready);

    int workerID = -1;

    while (!terminate_flag) {
        zmq::message_t message;
        if (pull_socket.recv(&message)) {
            if (workerID == -1) {
                workerID = std::stoi(std::string(static_cast<char*>(message.data()), message.size()));
                std::cout << "Assigned Worker ID: " << workerID << std::endl;
            } else {
                std::string received_data(static_cast<char*>(message.data()), message.size());
                size_t delimiterPos = received_data.find('|');
                if (delimiterPos != std::string::npos) {
                    std::string serializedPacket = received_data.substr(delimiterPos + 1);
                    Peregrine::JobPacket jobPacket = Peregrine::JobPacket::deserialize(serializedPacket);

                    // Print and process the job packet
                    // ... (existing code for processing job packet)

                    // Wait for one second before notifying the job pool
                    std::this_thread::sleep_for(std::chrono::seconds(1));

                    // Notify the job pool that this worker is ready for another packet
                    zmq::message_t ready_msg("Ready", 5);
                    push_socket.send(ready_msg);
                }
            }
        }
    }

    return 0;
}
