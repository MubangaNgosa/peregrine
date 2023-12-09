#include "zmq.hpp"
#include "DataGraph.hh"
#include "jobPacket.h"
#include <iostream>
#include <csignal>
#include <string>

volatile sig_atomic_t terminate_flag = 0; // Flag for graceful shutdown

void signal_handler(int /* signal */) {
    std::cout << "Signal handler triggered." << std::endl;
    terminate_flag = 1;
}

namespace Peregrine {

    SmallGraph createSmallGraphFromJobPacket(const JobPacket& packet) {
        std::cout << "Creating SmallGraph from JobPacket." << std::endl;
        SmallGraph p;
        for (const auto& edge : packet.edges) {
            p.add_edge(edge.first, edge.second);
        }
        // Optional: set labels for vertices if needed
        // for (const auto& vertex : packet.vertices) {
        //     p.set_label(vertex, label_value); // Define label_value as needed
        // }
        return p;
    }

    // Additional functions can be added here if needed

}

int main() {
    std::cout << "Worker starting up..." << std::endl;

    // Set up signal handling for graceful shutdown
    std::cout << "Setting up signal handling." << std::endl;
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Initialize ZeroMQ context and sockets
    std::cout << "Initializing ZeroMQ context and sockets." << std::endl;
    zmq::context_t context(1);
    zmq::socket_t pull_socket(context, ZMQ_PULL);
    pull_socket.connect("tcp://127.0.0.1:5555");

    zmq::socket_t push_socket(context, ZMQ_PUSH);
    push_socket.connect("tcp://127.0.0.1:5556");

    // Send initial ready message
    std::cout << "Sending initial ready message." << std::endl;
    zmq::message_t ready_msg("Ready", 5);
    push_socket.send(ready_msg, zmq::send_flags::none);

    bool shouldTerminate = false;

    while (!terminate_flag && !shouldTerminate) {
        zmq::message_t message;
        std::cout << "Waiting for a message." << std::endl;
        if (pull_socket.recv(message, zmq::recv_flags::none)) {
            std::string received_data(static_cast<char*>(message.data()), message.size());
            //std::cout << "Worker received message: " << received_data << std::endl;

            Peregrine::JobPacket jobPacket = Peregrine::JobPacket::deserialize(received_data);
            Peregrine::SmallGraph smallGraph = Peregrine::createSmallGraphFromJobPacket(jobPacket);

            std::cout << "Processing the SmallGraph." << std::endl;
            // Process the SmallGraph here

            if (received_data == "terminate") {
                std::cout << "Terminate signal received. Finishing current tasks." << std::endl;
                shouldTerminate = true;
                continue;
            }

            if (!terminate_flag) {
                std::cout << "Sending ready message for next job packet." << std::endl;
                zmq::message_t ready_msg("Ready", 5);
                push_socket.send(ready_msg, zmq::send_flags::none);
            } else {
                std::cout << "Worker received shutdown signal during processing." << std::endl;
            }
        }

        if (shouldTerminate) {
            std::cout << "Sending termination acknowledgment." << std::endl;
            zmq::message_t ack_msg("Terminated", 10);
            push_socket.send(ack_msg, zmq::send_flags::none);
            break;
        }
    }

    std::cout << "Worker terminating." << std::endl;
    return 0;
}
