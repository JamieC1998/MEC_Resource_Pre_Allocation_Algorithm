//
// Created by Jamie Cotter on 21/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_VERTEXDATA_H
#define FIRSTHOPMOBILEOFFLOADING_VERTEXDATA_H

#include <enums/NodeTypes.h>
#include <Models/ComputationNode/ComputationNode.h>
#include <Models/EdgeNode/EdgeNode.h>
#include <Models/MobileNode/MobileNode.h>

struct VertexData {
    node_type type;
    boost::optional<ComputationNode> comp;
    boost::optional<EdgeNode> edgeNode;
    boost::optional<MobileNode> mobileNode;
};

#endif //FIRSTHOPMOBILEOFFLOADING_VERTEXDATA_H
