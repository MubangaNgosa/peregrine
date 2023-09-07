#include <zmq.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "../DataGraph.hh"
#include "createJobPackets.h"
#include "jobPacket.h"

int main() {
    // Initialize DataGraph
    std::string dataGraphPath = "../../data/citeseer"; // Adjust path as necessary
    Peregrine::DataGraph dataGraph(dataGraphPath);
    std::cout << "DataGraph initialized from " << dataGraphPath << std::endl;

    // Generate job packets from the DataGraph
    std::vector<Peregrine::JobPacket> jobPackets = Peregrine::createJobPackets(dataGraph);

    // Initialize ZeroMQ context and sockets
    zmq::context_t context(1);
    zmq::socket_t push_socket(context, ZMQ_PUSH);
    push_socket.bind("tcp://127.0.0.1:5555"); // Bind to PUSH socket for sending job packets

    zmq::socket_t pull_socket(context, ZMQ_PULL);
    pull_socket.bind("tcp://127.0.0.1:5556 "); // Bind to PULL socket for receiving responses

    std::cout << "jobPool is ready, listening on ports 5556 for sending and 5555 for responses." << std::endl;

    int workerID = 0;

    while (!jobPackets.empty()) {
        zmq::message_t worker_ready_msg;
        if (pull_socket.recv(&worker_ready_msg)) {
            workerID++;

            // Serialize and send the entire packet along with workerID
            std::string serializedPacket = std::to_string(workerID) + "|" + jobPackets.back().serialize();
            zmq::message_t job_msg(serializedPacket.begin(), serializedPacket.end());
            push_socket.send(job_msg);
            jobPackets.pop_back(); // Remove the packet after sending

            std::cout << "Packet sent to worker " << workerID << std::endl;
        }
    }

    return 0;
}
