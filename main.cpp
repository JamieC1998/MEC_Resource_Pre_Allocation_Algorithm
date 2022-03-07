
#include "Model/ComputationNode/ComputationNode.h"
#include "Graph/EdgeData/EdgeData.h"
#include "Graph/Graph.h"
#include "Services/NetworkTopologyServices/NetworkTopologyServices.h"
#include "Model/Task/Task.h"
#include "Services/ApplicationTopologyServices/ApplicationTopologyServices.h"
#include "Services/SimulatorServices/SimulatorServices.h"
#include "utils/JsonOutputFunctions/JsonOutputFunctions.h"
#include "Constants/AlgorithmFlag/AlgorithmFlag.h"

int main(int argc, char *argv[]) {
    const char *input_file_name = "/Users/jamiecotter/CLionProjects/untitled2/application_topology_batch_1_1";
    const char *output_file_name = "/Users/jamiecotter/CLionProjects/untitled2/output_sol";
    const char *application_mode = "preallocation";
    const char *lower_bound_file_name = "/Users/jamiecotter/CLionProjects/untitled2/lower_bound.txt";

    float simulation_time = 20;

    if (argc > 3) {
        input_file_name = argv[1];
        output_file_name = argv[2];
        application_mode = argv[3];
    }

    AlgorithmFlag::setAlgorithmMode(application_mode);

    auto networkGraph = NetworkTopologyServices::generateNetwork();
    auto mobile_ids = NetworkTopologyServices::fetchIdsByType(node_type::mobile, networkGraph);
    auto incoming_applications = ApplicationTopologyServices::generateApplications(
            input_file_name, simulation_time, mobile_ids[0]);

    std::vector<std::shared_ptr<Graph<Task, bool>>> applications;

    auto finished = SimulatorServices::programLoop(networkGraph, incoming_applications, simulation_time, applications);
    JsonOutputFunctions::outputJson(finished, applications, incoming_applications, output_file_name, networkGraph);
    return 0;
}
