#include <vector>
#include <array>
#include "utilities.h"

//****** PROGRAM INPUT ********//
double totalHeight = 80; //[mm]
double totalWidth = 46;  //[mm]
double bendingMoment_Nm = 100; //[Nm]
std::string fileName = ""sd0/Signaloid/BeamCalculator/data.txt";
//*****************************//

int main()
{
    //Data structures
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
