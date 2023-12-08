#include "zmq.hpp"
#include <iostream>
#include <string>
#include <unordered_set>
#include <queue>
#include "../DataGraph.hh"
#include "createJobPackets.h"
#include "jobPacket.h"

int main() {
    // Initialize DataGraph from a specified path
    std::string dataGraphPath = "../../data/citeseer"; // Adjust path as necessary
    Peregrine::DataGraph dataGraph(dataGraphPath);
    std::cout << "DataGraph initialized from " << dataGraphPath << std::endl;

    // Generate job packets from the DataGraph
    std::vector<Peregrine::JobPacket> jobPackets = Peregrine::createJobPackets(dataGraph);
    std::cout << "Finished creating " << jobPackets.size() << " job packets." << std::endl;

    // Initialize ZeroMQ context and sockets
    zmq::context_t context(1);
    zmq::socket_t push_socket(context, ZMQ_PUSH);
    push_socket.bind("tcp://127.0.0.1:5555");

    zmq::socket_t pull_socket(context, ZMQ_PULL);
    pull_socket.bind("tcp://127.0.0.1:5556");

    std::cout << "jobPool is ready, listening on ports 5555 and 5556." << std::endl;

    std::unordered_set<int> activeWorkers;
    std::queue<int> readyWorkers;
    int workerID = 0;
    int packetNumber = 0;

    while (!jobPackets.empty() || !readyWorkers.empty()) {
        zmq::message_t worker_ready_msg;
        if (pull_socket.recv(&worker_ready_msg)) {
            std::string message(static_cast<char*>(worker_ready_msg.data()), worker_ready_msg.size());
            std::cout << "Received message: " << message << std::endl;

            if (message == "Ready") {
                workerID++; // Increment workerID for each new worker
                activeWorkers.insert(workerID);
                readyWorkers.push(workerID);
                std::cout << "Worker " << workerID << " is ready." << std::endl;
            }
        }

        if (!jobPackets.empty() && !readyWorkers.empty()) {
            int assignedWorkerID = readyWorkers.front();
            readyWorkers.pop();

            std::string serializedPacket = std::to_string(assignedWorkerID) + "|" + jobPackets.back().serialize();
            zmq::message_t job_msg(serializedPacket.begin(), serializedPacket.end());
            push_socket.send(job_msg);
            jobPackets.pop_back();

            std::cout << "Sent job packet " << ++packetNumber << " to worker " << assignedWorkerID << std::endl;
        }
    }

    // Send termination signal to all active workers
    for (int worker : activeWorkers) {
        zmq::message_t terminate_msg("terminate", 9);
        push_socket.send(terminate_msg);
        std::cout << "Sent termination signal to worker " << worker << std::endl;
    }

    std::cout << "jobPool terminating." << std::endl;
    return 0;
}
