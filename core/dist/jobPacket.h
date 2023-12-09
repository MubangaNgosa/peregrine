#ifndef JOBPACKET_H
#define JOBPACKET_H

#include <vector>
#include <string>
#include <utility> // For std::pair

namespace Peregrine {

    struct JobPacket {
        unsigned long long taskId; // Unique identifier for the task
        std::vector<uint32_t> vertices; // List of vertices
        std::vector<std::pair<uint32_t, uint32_t>> edges; // List of edges (pairs of vertex IDs)

        // Serialize the JobPacket to a string
        std::string serialize() const;

        // Deserialize a string to a JobPacket
        static JobPacket deserialize(const std::string& data);
    };

}

#endif // JOBPACKET_H
