#include "zmq.hpp"
#include <iostream>
#include <string>
#include <csignal>
#include "../DataGraph.hh"
#include "jobPacket.h"

volatile sig_atomic_t terminate_flag = 0; // Flag for graceful shutdown

void signal_handler(int /* signal */) {
    terminate_flag = 1;
}

int main() {
    std::cout << "Worker starting up..." << std::endl;

    // Set up signal handling for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Initialize ZeroMQ context and sockets
    zmq::context_t context(1);
    zmq::socket_t pull_socket(context, ZMQ_PULL);
    pull_socket.connect("tcp://127.0.0.1:5555");

    zmq::socket_t push_socket(context, ZMQ_PUSH);
    push_socket.connect("tcp://127.0.0.1:5556");

    // Send initial ready message
    zmq::message_t ready_msg("Ready", 5);
    push_socket.send(ready_msg);
    std::cout << "Worker sent initial ready message." << std::endl;

    bool shouldTerminate = false; // Flag to track termination signal

    while (!terminate_flag && !shouldTerminate) {
        zmq::message_t message;
        if (pull_socket.recv(&message)) {
            std::string received_data(static_cast<char*>(message.data()), message.size());
            std::cout << "Worker received message: " << received_data << std::endl;

            // Check for termination signal
            if (received_data == "terminate") {
                std::cout << "Terminate signal received. Finishing current tasks." << std::endl;
                shouldTerminate = true;
                continue; // Continue to finish current task before terminating
            }

            // Process job packet
            // ... (Processing logic here)
            std::cout << "Worker processing job packet." << std::endl;

            // Check for termination flag again after processing
            if (!terminate_flag) {
                // Send ready message for next job packet
                zmq::message_t ready_msg("Ready", 5);
                push_socket.send(ready_msg);
                std::cout << "Worker sent ready message for next job packet." << std::endl;
            } else {
                std::cout << "Worker received shutdown signal during processing." << std::endl;
            }
        }

        // If termination signal is received during processing, acknowledge and exit
        if (shouldTerminate) {
            zmq::message_t ack_msg("Terminated", 10);
            push_socket.send(ack_msg);
            std::cout << "Worker sent termination acknowledgment." << std::endl;
            break; // Exit the loop
        }
    }

    std::cout << "Worker terminating." << std::endl;
    return 0;
}#include "zmq.hpp"
#include <iostream>
#include <string>
#include <csignal>
#include "../DataGraph.hh"
#include "jobPacket.h"

volatile sig_atomic_t terminate_flag = 0; // Flag for graceful shutdown

void signal_handler(int /* signal */) {
    terminate_flag = 1;
}

int main() {
    std::cout << "Worker starting up..." << std::endl;

    // Set up signal handling for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Initialize ZeroMQ context and sockets
    zmq::context_t context(1);
    zmq::socket_t pull_socket(context, ZMQ_PULL);
    pull_socket.connect("tcp://127.0.0.1:5555");

    zmq::socket_t push_socket(context, ZMQ_PUSH);
    push_socket.connect("tcp://127.0.0.1:5556");

    // Send initial ready message
    zmq::message_t ready_msg("Ready", 5);
    push_socket.send(ready_msg);
    std::cout << "Worker sent initial ready message." << std::endl;

    bool shouldTerminate = false; // Flag to track termination signal

    while (!terminate_flag && !shouldTerminate) {
        zmq::message_t message;
        if (pull_socket.recv(&message)) {
            std::string received_data(static_cast<char*>(message.data()), message.size());
            std::cout << "Worker received message: " << received_data << std::endl;

            // Check for termination signal
            if (received_data == "terminate") {
                std::cout << "Terminate signal received." << std::endl;
                shouldTerminate = true; // Set flag to terminate

                // Send acknowledgment
                zmq::message_t ack_msg("Terminated", 10);
                push_socket.send(ack_msg);
                std::cout << "Worker sent termination acknowledgment." << std::endl;
                break; // Exit the loop
            }

            // Process job packet
            // ... (Processing logic here)
            std::cout << "Worker processing job packet." << std::endl;

            // Send ready message for next job packet
            zmq::message_t ready_msg("Ready", 5);
            push_socket.send(ready_msg);
            std::cout << "Worker sent ready message for next job packet." << std::endl;
        }
    }

    std::cout << "Worker terminating." << std::endl;
    return 0;
}

