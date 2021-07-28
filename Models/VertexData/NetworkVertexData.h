//
// Created by Jamie Cotter on 21/07/2021.
//

#ifndef FIRSTHOPMOBILEOFFLOADING_NETWORKVERTEXDATA_H
#define FIRSTHOPMOBILEOFFLOADING_NETWORKVERTEXDATA_H

#include <enums/NodeTypes.h>
#include <Models/ComputationNode/ComputationNode.h>
#include <Models/EdgeNode/EdgeNode.h>
#include <Models/MobileNode/MobileNode.h>
#include <boost/optional.hpp>

struct NetworkVertexData {
    node_type type;
    boost::optional<ComputationNode> comp;
    boost::optional<EdgeNode> edgeNode;
    boost::optional<MobileNode> mobileNode;
};

#endif //FIRSTHOPMOBILEOFFLOADING_NETWORKVERTEXDATA_H
