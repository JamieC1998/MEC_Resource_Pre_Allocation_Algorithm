//
// Created by Jamie Cotter on 21/01/2022.
//

#ifndef UNTITLED2_NETWORKTOPOLOGYSERVICES_H
#define UNTITLED2_NETWORKTOPOLOGYSERVICES_H


#include "../../Graph/Graph.h"
#include "../../Model/ComputationNode/ComputationNode.h"
#include "../../Graph/EdgeData/EdgeData.h"

const int SERVERS_PER_CLOUDLET = 1;

class NetworkTopologyServices {
public:
    static Graph<ComputationNode, std::shared_ptr<EdgeData>> generateNetwork();

    static std::vector<int> fetchIdsByType(node_type type, Graph<ComputationNode, std::shared_ptr<EdgeData>> &nG);

    static std::pair<int, std::shared_ptr<std::pair<float, float>>> findLinkSlot(const std::shared_ptr<EdgeData>& edge, float parent_upload_start, float dataOut);

    static void addUploadsToLink(const std::shared_ptr<TaskMapping>& mapping, Graph<ComputationNode, std::shared_ptr<EdgeData>> &nG);
};


#endif //UNTITLED2_NETWORKTOPOLOGYSERVICES_H
