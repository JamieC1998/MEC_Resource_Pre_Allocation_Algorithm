
#include "Model/ComputationNode/ComputationNode.h"
#include "Graph/EdgeData/EdgeData.h"
#include "Graph/Graph.h"
#include "Services/NetworkTopologyServices/NetworkTopologyServices.h"
#include "Model/Task/Task.h"
#include "Services/ApplicationTopologyServices/ApplicationTopologyServices.h"
#include "Services/SimulatorServices/SimulatorServices.h"
#include "main.h"
#include "Constants/AlgorithmMode/AlgorithmMode.h"
#include "utils/JsonOutputFunctions/JsonOutputFunctions.h"

int main(int argc, char* argv[]) {
    const char *input_file_name = "/Users/jamiecotter/CLionProjects/untitled2/application_topology_batch_1_1";
    const char *output_file_name = "/Users/jamiecotter/CLionProjects/untitled2/output_sol";
    const char *application_mode = "preallocation";
    const char *lower_bound_file_name = "/Users/jamiecotter/CLionProjects/untitled2/lower_bound.txt";

    if(argc > 3){
        input_file_name = argv[1];
        output_file_name = argv[2];
        application_mode = argv[3];
    }

    AlgorithmMode::setAlgorithmMode(application_mode);

    float simulation_time = 20;
    Graph<ComputationNode, std::shared_ptr<EdgeData>> nG = NetworkTopologyServices::generateNetwork();
    std::vector<int> mobile_ids = NetworkTopologyServices::fetchIdsByType(node_type::mobile, nG);
    std::vector<std::pair<float, std::shared_ptr<Graph<Task, bool>>>> incoming_applications = ApplicationTopologyServices::generateApplications(
            input_file_name, simulation_time, mobile_ids[0]);
    std::vector<std::shared_ptr<Graph<Task, bool>>> applications;

    std::vector<std::shared_ptr<TaskMapping>> finished = SimulatorServices::programLoop(nG, incoming_applications,
                                                                                        simulation_time, applications);
    JsonOutputFunctions::outputJson(finished, applications, incoming_applications, output_file_name, nG);
    return 0;
}
