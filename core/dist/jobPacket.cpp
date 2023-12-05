#include "jobPacket.h"
#include <sstream>
#include <iostream>

namespace Peregrine {

    // Serialization
    std::string JobPacket::serialize() const {
        std::ostringstream ss;
        ss << taskId;

        // Serialize vertices
        for (const auto& vertex : vertices) {
            ss << "|v" << vertex;  // Prefix 'v' for vertices
        }

        // Serialize edges
        for (const auto& edge : edges) {
            ss << "|e" << edge.first << "-" << edge.second;  // Prefix 'e' for edges
        }

        return ss.str();
    }

    // Deserialization
    JobPacket JobPacket::deserialize(const std::string& data) {
        std::istringstream ss(data);
        JobPacket packet;
        std::string token;

        // Deserialize taskId
        std::getline(ss, token, '|');
        packet.taskId = std::stoull(token);

        // Deserialize vertices and edges
        while (std::getline(ss, token, '|')) {
            if (token[0] == 'v') {  // Vertex
                token.erase(0, 1);  // Remove 'v' prefix
                packet.vertices.push_back(std::stoul(token));
            } else if (token[0] == 'e') {  // Edge
                token.erase(0, 1);  // Remove 'e' prefix
                std::istringstream edgeStream(token);
                std::string vertex;
                std::getline(edgeStream, vertex, '-');
                uint32_t u = std::stoul(vertex);
                std::getline(edgeStream, vertex);
                uint32_t v = std::stoul(vertex);
                packet.edges.emplace_back(u, v);
            }
        }

        return packet;
    }

}
