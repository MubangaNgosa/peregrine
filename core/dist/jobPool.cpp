#include "zmq.hpp"
#include <iostream>
#include <string>
#include <queue>
#include "../DataGraph.hh"
#include "createJobPackets.h"
#include "jobPacket.h"

int main() {
    // Initialize DataGraph from a specified path
    std::string dataGraphPath = "../../data/citeseer"; // Adjust this path as necessary
    Peregrine::DataGraph dataGraph(dataGraphPath);
    std::cout << "DataGraph initialized from " << dataGraphPath << std::endl;

    // Generate job packets from the DataGraph
    std::vector<Peregrine::JobPacket> jobPackets = Peregrine::createJobPackets(dataGraph);
    std::cout << "Finished creating " << jobPackets.size() << " job packets." << std::endl;

    // Initialize ZeroMQ context and sockets
    zmq::context_t context(1);
    zmq::socket_t push_socket(context, ZMQ_PUSH);
    push_socket.bind("tcp://127.0.0.1:5555"); // Bind to PUSH socket for sending job packets

    zmq::socket_t pull_socket(context, ZMQ_PULL);
    pull_socket.bind("tcp://127.0.0.1:5556"); // Bind to PULL socket for receiving responses

    std::cout << "jobPool is ready, listening on ports 5556 for sending and 5555 for responses." << std::endl;

    std::queue<int> readyWorkers;
    int packetNumber = 0;
    int workerID = 1; // Start with Worker ID 1

    while (!jobPackets.empty() || !readyWorkers.empty()) {
        zmq::message_t worker_ready_msg;
        if (pull_socket.recv(&worker_ready_msg)) {
            std::string message(static_cast<char*>(worker_ready_msg.data()), worker_ready_msg.size());

            // Check if the message is 'Ready'
            if (message == "Ready") {
                readyWorkers.push(workerID++);
            } else {
                std::cerr << "Unexpected message received: " << message << std::endl;
            }
        }

        if (!jobPackets.empty() && !readyWorkers.empty()) {
            int assignedWorkerID = readyWorkers.front();
            readyWorkers.pop();

            // Serialize and send the job packet along with workerID
            std::string serializedPacket = std::to_string(assignedWorkerID) + "|" + jobPackets.back().serialize();
            zmq::message_t job_msg(serializedPacket.begin(), serializedPacket.end());
            push_socket.send(job_msg);
            jobPackets.pop_back(); // Remove the packet after sending

            // Announce after sending each job packet
            std::cout << "Sent job packet " << ++packetNumber << " to worker " << assignedWorkerID << std::endl;
        }
    }

    return 0;
}
