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
    push_socket.send(ready, zmq::send_flags::none);

    int workerID = -1;

    while (!terminate_flag) {
        zmq::message_t message;
        if (pull_socket.recv(&message)) {
            std::string received_data(static_cast<char*>(message.data()), message.size());
            size_t delimiterPos = received_data.find('|');

            if (delimiterPos != std::string::npos) {
                // Extract worker ID if not already assigned
                if (workerID == -1) {
                    workerID = std::stoi(received_data.substr(0, delimiterPos));
                    std::cout << "Assigned Worker ID: " << workerID << std::endl;
                }

                std::string serializedPacket = received_data.substr(delimiterPos + 1);
                Peregrine::JobPacket jobPacket = Peregrine::JobPacket::deserialize(serializedPacket);

                // Print the deserialized job packet for debugging
                std::cout << "Worker ID " << workerID << " received packet:\n";
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
                // (Your processing logic goes here)

                // Wait for one second before notifying the job pool
                std::this_thread::sleep_for(std::chrono::seconds(1));

                // Notify the job pool that this worker is ready for another packet
                zmq::message_t ready_signal("Ready", 5);
                push_socket.send(ready_signal, zmq::send_flags::none);
            }
        }
    }

    return 0;
}
