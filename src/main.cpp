#include <vector>
#include <array>
#include "utilities.h"

//*********** DEFAULT VALUES **************//
double defaultWidth = 50;             //[mm]
double defaultHeight = 80;            //[mm]
double defaultBendingMoment_Nm = 100; //[Nm]
//*****************************************//

int main(int argc, char* argv[])
{
    // Parsing input data
    std::array<double, 3> parameters{defaultWidth, defaultHeight, defaultBendingMoment_Nm};
    std::vector<std::string> args(argv, argv+argc);
    std::string fileName;
    if(!parseArguments(args, fileName, parameters)) return 1;
    
    double totalWidth =       parameters[0];
    double totalHeight =      parameters[1];
    double bendingMoment_Nm = parameters[2];
    
    printInitialData(fileName, totalWidth, totalHeight, bendingMoment_Nm);

    //Main data structures
    std::vector<std::pair<short,std::vector<short>>> qualifiedBlocks;
    std::vector<std::array<double,4>> refinedBlocks;
    
    //Preliminary file manipulation and data structures filling
    if (!checkFile(fileName)) return 1;
    std::pair<short, short> sectionLimitLines = getSectionLimitLines(fileName);
    computeQualifiedBlocks(fileName, qualifiedBlocks, sectionLimitLines);
    short maxBlocksInALine = calculateMaxWidthBlocks(qualifiedBlocks);
    short activeLines = sectionLimitLines.second - sectionLimitLines.first + 1;
    double blockHeight  = totalHeight / activeLines;
    double blockWidth = totalWidth / maxBlocksInALine;
    computeRefinedBlocks(qualifiedBlocks, refinedBlocks, blockWidth, blockHeight);
    
    //Geometric calculations
    
    double centroidY = calculateCentroid(refinedBlocks);
    double inertiaY = calculateInertia(refinedBlocks, centroidY);
    double maxStress = bendingMoment_Nm*1000*std::max(centroidY,totalHeight-centroidY)/inertiaY; //Formula sigma=M*y/I
    
    //Presenting the results
    
    showResults(centroidY, inertiaY, maxStress);
    
    return 0;
}
