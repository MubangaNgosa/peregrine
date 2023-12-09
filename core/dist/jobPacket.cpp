#include "jobPacket.h"
#include <sstream>
#include <iostream>

namespace Peregrine {

    // Serialization
    std::string JobPacket::serialize() const {
        std::ostringstream ss;
        ss << taskId;

        for (const auto& vertex : vertices) {
            ss << "|v" << vertex;
        }

        for (const auto& edge : edges) {
            ss << "|e" << edge.first << "-" << edge.second;
        }

        return ss.str();
    }

    // Deserialization
    JobPacket JobPacket::deserialize(const std::string& data) {
        std::istringstream ss(data);
        JobPacket packet;
        std::string token;

        std::getline(ss, token, '|');
        packet.taskId = std::stoull(token);

        while (std::getline(ss, token, '|')) {
            if (token[0] == 'v') {
                token.erase(0, 1);
                packet.vertices.push_back(std::stoul(token));
            } else if (token[0] == 'e') {
                token.erase(0, 1);
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

    // Additional functions can be added here if needed

}
