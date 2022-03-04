//
// Created by Jamie Cotter on 21/01/2022.
//

#ifndef UNTITLED2_NETWORKTOPOLOGYSERVICES_H
#define UNTITLED2_NETWORKTOPOLOGYSERVICES_H


#include "../../Graph/Graph.h"
#include "../../Model/ComputationNode/ComputationNode.h"
#include "../../Graph/EdgeData/EdgeData.h"

namespace NetworkTopologyServices {
    Graph<ComputationNode, std::shared_ptr<EdgeData>> generateNetwork();

    std::vector<int> fetchIdsByType(node_type type, Graph<ComputationNode, std::shared_ptr<EdgeData>> &nG);

    std::pair<int, std::shared_ptr<std::pair<float, float>>> findLinkSlot(const std::shared_ptr<EdgeData>& edge, float parent_upload_start, float dataOut);

    void addUploadsToLink(const std::shared_ptr<TaskMapping>& mapping, Graph<ComputationNode, std::shared_ptr<EdgeData>> &nG);
};


#endif //UNTITLED2_NETWORKTOPOLOGYSERVICES_H
