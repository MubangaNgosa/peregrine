#ifndef PEREGRINE_JOBPACKET_H
#define PEREGRINE_JOBPACKET_H

#include <string>
#include <vector>
#include <utility> // For std::pair

namespace Peregrine {

    struct JobPacket {
        uint64_t taskId;    // Unique identifier for the task
        uint32_t vgsi;      // Vertex Group Size Index
        std::vector<uint32_t> vertices; // Vertices included in the packet
        std::vector<std::pair<uint32_t, uint32_t>> edges; // Edges included in the packet

        // Serialization
        std::string serialize() const;

        // Deserialization
        static JobPacket deserialize(const std::string& data);
    };

}

#endif // PEREGRINE_JOBPACKET_H
