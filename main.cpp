//
// Created by Jamie Cotter on 20/07/2021.
//

#include "main.h"
#include <Services/NetworkTopologyServices/NetworkTopologyServices.h>
#include <Services/ApplicationTopologyServices/ApplicationTopologyServices.h>


using namespace std;

int main() {
    NetworkTopology network = NetworkTopologyServices::generateNetwork();
    NetworkTopologyServices::logInfo(network);
    cout << endl << "NAVIGATOR " << endl;
    ApplicationTopology navigator = ApplicationTopologyServices::generateNavigator();
    ApplicationTopologyServices::logInfo(navigator);



    return 0;
}