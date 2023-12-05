#include "jobPacket.h"
#include "createJobPackets.h"
#include <vector>
#include <iostream>

namespace Peregrine {

    std::vector<JobPacket> createJobPackets(const DataGraph& dataGraph) {
        std::cout << "Creating job packets..." << std::endl;
        std::vector<JobPacket> jobPackets;

        const uint32_t vertexGroupSize = 30;  // Number of vertices in each packet
        uint64_t taskId = 0;

        // Loop through the vertices in groups of 30
        for (uint32_t v = 1; v <= dataGraph.get_vertex_count(); v += vertexGroupSize) {
            JobPacket packet;
            packet.taskId = ++taskId;

            // Add up to 30 vertices to the packet
            for (uint32_t i = 0; i < vertexGroupSize && (v + i) <= dataGraph.get_vertex_count(); ++i) {
                uint32_t currentVertex = v + i;
                packet.vertices.push_back(currentVertex); // Add the vertex to the packet

                // Add edges associated with the current vertex to the packet
                auto adjList = dataGraph.get_adj(currentVertex);
                for (uint32_t j = 0; j < adjList.length; ++j) {
                    uint32_t u = adjList.ptr[j]; // Adjacent vertex
                    if (u > currentVertex) { // Avoid duplicate edges
                        packet.edges.emplace_back(currentVertex, u);
                    }
                }
            }

            jobPackets.push_back(packet);
        }

        std::cout << "Finished creating " << jobPackets.size() << " job packets." << std::endl;
        return jobPackets;
    }
}
