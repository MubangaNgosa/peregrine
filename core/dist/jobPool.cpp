#include "zmq.hpp"
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
    pull_socket.bind("tcp://127.0.0.1:5556"); // Bind to PULL socket for receiving worker readiness signals

    std::cout << "jobPool is ready, listening on ports 5556 for worker readiness and 5555 for sending job packets." << std::endl;

    while (!jobPackets.empty()) {
        zmq::message_t worker_ready_msg;
        // Wait for a worker to signal readiness
        if (pull_socket.recv(&worker_ready_msg)) {
            // Get the next job packet
            Peregrine::JobPacket packet = jobPackets.back();
            jobPackets.pop_back();

            // Serialize and send the job packet
            std::string serializedPacket = packet.serialize();
            zmq::message_t job_msg(serializedPacket.begin(), serializedPacket.end());
            push_socket.send(job_msg, zmq::send_flags::none);

            std::cout << "Sent a job packet to a ready worker." << std::endl;
        }
    }

    // Optionally, handle cleanup or additional logic here

    return 0;
}
