cmake_minimum_required(VERSION 3.19)
project(FirstHopMobileOffloading)

set(CMAKE_CXX_STANDARD 14)


find_package(Boost REQUIRED COMPONENTS filesystem)
include_directories(. ${Boost_INCLUDE_DIRS})

add_executable(FirstHopMobileOffloading
        main.cpp
        main.h Models/ComputationNode/ComputationNode.cpp
        Models/ComputationNode/ComputationNode.h enums/NodeTypes.h
        Models/EdgeNode/EdgeNode.cpp Models/EdgeNode/EdgeNode.h
        Models/MobileNode/MobileNode.cpp
        Models/MobileNode/MobileNode.h
        Models/VertexData/NetworkVertexData.h
        Models/Task/Task.cpp Models/Task/Task.h
        Services/NetworkTopologyServices/NetworkTopologyServices.cpp
        Services/NetworkTopologyServices/NetworkTopologyServices.h
        Services/ApplicationTopologyServices/ApplicationTopologyServices.cpp
        Services/ApplicationTopologyServices/ApplicationTopologyServices.h
        Models/VertexData/TaskVertexData.h
        Services/SimulatorServices/SimulatorFunctions.cpp
        Services/SimulatorServices/SimulatorFunctions.h
        Services/FileReader/FileReader.cpp Services/FileReader/FileReader.h Models/Mappings/ReservationMapping.h Models/Mappings/TaskMapping.h Models/VertexData/EdgePropertyData.h Models/Mappings/SuperNode.h Models/Mappings/TempTask.h Services/ApplicationGraph/ApplicationGraph.cpp Services/ApplicationGraph/ApplicationGraph.h enums/ProcessingTypes.h)
