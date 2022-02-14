//
// Created by Jamie Cotter on 21/01/2022.
//

#include "NetworkTopologyServices.h"
#include "vector"
#include "../../Constants/ReactiveConstants.h"

Graph<ComputationNode, std::shared_ptr<EdgeData>> NetworkTopologyServices::generateNetwork() {
    Graph<ComputationNode, std::shared_ptr<EdgeData>> nG = Graph<ComputationNode, std::shared_ptr<EdgeData>>(false);

    auto *cloudNodeA = new ComputationNode(std::numeric_limits<int>::max(), std::numeric_limits<float>::max(),
                                           std::numeric_limits<float>::max(), cloud);
    auto *edgeNode = new ComputationNode(2 * SERVERS_PER_CLOUDLET, 64 * SERVERS_PER_CLOUDLET,
                                         1000 * SERVERS_PER_CLOUDLET, edge);
    auto *mobileNode = new ComputationNode(1, 8, 128, mobile);
    auto *edgeNodeB = new ComputationNode(2 * SERVERS_PER_CLOUDLET, 64 * SERVERS_PER_CLOUDLET,
                                          1000 * SERVERS_PER_CLOUDLET, edge);
    auto *edgeNodeC = new ComputationNode(2 * SERVERS_PER_CLOUDLET, 64 * SERVERS_PER_CLOUDLET,
                                          1000 * SERVERS_PER_CLOUDLET, edge);

    nG.addVertex(cloudNodeA);
    nG.addVertex(edgeNode);
    nG.addVertex(mobileNode);
    nG.addVertex(edgeNodeB);
    nG.addVertex(edgeNodeC);


    nG.addEdge(mobileNode->getId(), edgeNode->getId(), std::make_shared<EdgeData>(
            EdgeData(std::make_pair(mobileNode->getId(), edgeNode->getId()), 10, 150.0f / 1000)));
    nG.addEdge(mobileNode->getId(), edgeNodeB->getId(), std::make_shared<EdgeData>(
            EdgeData(std::make_pair(mobileNode->getId(), edgeNodeB->getId()), 10, 150.0f / 1000)));
    nG.addEdge(mobileNode->getId(), edgeNodeC->getId(), std::make_shared<EdgeData>(
            EdgeData(std::make_pair(mobileNode->getId(), edgeNodeC->getId()), 10, 150.0f / 1000)));
    nG.addEdge(mobileNode->getId(), cloudNodeA->getId(), std::make_shared<EdgeData>(
            EdgeData(std::make_pair(mobileNode->getId(), cloudNodeA->getId()), 70, 36.0f / 1000)));

    nG.addEdge(edgeNode->getId(), edgeNodeB->getId(), std::make_shared<EdgeData>(
            EdgeData(std::make_pair(edgeNode->getId(), edgeNodeB->getId()), 10, 1024.0f / 1000)));
    nG.addEdge(edgeNode->getId(), edgeNodeC->getId(), std::make_shared<EdgeData>(
            EdgeData(std::make_pair(edgeNode->getId(), edgeNodeB->getId()), 10, 1024.0f / 1000)));
    nG.addEdge(edgeNode->getId(), cloudNodeA->getId(), std::make_shared<EdgeData>(
            EdgeData(std::make_pair(edgeNode->getId(), cloudNodeA->getId()), 20, 1024.0f / 1000)));

    nG.addEdge(edgeNodeB->getId(), edgeNodeC->getId(), std::make_shared<EdgeData>(
            EdgeData(std::make_pair(edgeNodeB->getId(), edgeNodeC->getId()), 10, 1024.0f / 1000)));
    nG.addEdge(edgeNodeB->getId(), cloudNodeA->getId(), std::make_shared<EdgeData>(
            EdgeData(std::make_pair(edgeNodeB->getId(), cloudNodeA->getId()), 20, 1024.0f / 1000)));

    nG.addEdge(edgeNodeC->getId(), cloudNodeA->getId(), std::make_shared<EdgeData>(
            EdgeData(std::make_pair(edgeNodeC->getId(), cloudNodeA->getId()), 20, 1024.0f / 1000)));

    return nG;
}

std::vector<int>
NetworkTopologyServices::fetchIdsByType(node_type type, Graph<ComputationNode, std::shared_ptr<EdgeData>> &nG) {
    std::vector<int> res;

    for (auto const &it: nG.vertices)
        if (it.second->getNodeType() == type)
            res.push_back(it.second->getId());

    return res;
}

std::pair<int, std::shared_ptr<std::pair<float, float>>>
NetworkTopologyServices::findLinkSlot(const std::shared_ptr<EdgeData> &edge, float parent_upload_start, float dataOut) {
    int index = 0;
    float duration = dataOut != 0 ? (dataOut / edge->getBandwidth()) + edge->getLatency() : edge->getLatency();
    std::pair<float, float> time_window;
    if (edge->temp_occupancy_times.empty() ||
        edge->temp_occupancy_times.front()->first > (parent_upload_start + duration))
        time_window = std::make_pair(parent_upload_start, parent_upload_start + duration);
    else {
        for (int i = 0; i < edge->temp_occupancy_times.size(); i++) {
            /* If we are at the end of the time window list */
            if (i == edge->temp_occupancy_times.size() - 1) {
                index = i + 1;
                /* And the potential start of our time window is smaller than the last option
                 * in the occupancy list, then we must set the window floor to the largest time
                 * value in the list */
                if (edge->temp_occupancy_times[i]->second > parent_upload_start)
                    time_window = std::make_pair(edge->temp_occupancy_times[i]->second,
                                                 edge->temp_occupancy_times[i]->second + duration);
                    /* Otherwise, if the potential upload start is greater than the largest existing time value */
                else
                    time_window = std::make_pair(parent_upload_start, parent_upload_start + duration);
                break;
            } else {
                /* If time window A finished before our upload can start,
                 * then we use our upload start as our potential windows
                 * floor */
                float floor = (edge->temp_occupancy_times[i]->second > parent_upload_start)
                              ? edge->temp_occupancy_times[i]->second : parent_upload_start;
                if (edge->temp_occupancy_times[i + 1]->first - floor >= duration) {
                    index = i + 1;
                    time_window = std::make_pair(floor, floor + duration);
                    break;
                }
            }
        }

    }
    return std::make_pair(index, std::make_shared<std::pair<float, float>>(time_window.first, time_window.second));
}

void NetworkTopologyServices::addUploadsToLink(const std::shared_ptr<TaskMapping> &mapping,
                                               Graph<ComputationNode, std::shared_ptr<EdgeData>> &nG) {
    /* We iterate through the upload window object in the mapping provided.
     * This provides us a map, the keys represent node ids and their values are upload windows along the
     * edge from our node id key to the node in nG. */
    for (const auto&[id, upload_window_list]: mapping->upload_times) {
        //If id is -1 it means that we are using the reactive algorithm and need to return result to mobile.
        int source = (id != RETURN_TO_MOBILE) ? id : mapping->getTask()->getSourceMobileId();

        for (auto const &upload_window: upload_window_list) {
            std::shared_ptr<EdgeData> edgeData = nG.out_edges.at(source).at(mapping->getComputationNodeId());
            edgeData->occupancy_times.push_back(upload_window);
            edgeData->sortOccupancyTimes();
        }
    }
}
