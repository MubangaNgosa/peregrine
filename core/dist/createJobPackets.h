#ifndef PEREGRINE_CREATEJOBPACKETS_H
#define PEREGRINE_CREATEJOBPACKETS_H

#include "../DataGraph.hh" // Include the DataGraph header
#include "jobPacket.h"  // Include the JobPacket header
#include <vector>

namespace Peregrine {

    // Function to create job packets from a DataGraph
    std::vector<JobPacket> createJobPackets(const DataGraph& dataGraph);

}

#endif //PEREGRINE_CREATEJOBPACKETS_H
