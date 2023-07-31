#include "utilities.h"

#ifdef UNCERTAIN
#include <uxhw.h>
#endif

bool parseArguments(const std::vector<std::string>& args, std::string& fileName, std::array<double,3>& params)
{
    std::array<std::string, 3> flags{"-w", "-h", "-m"};
    if (args.size() < 2) {
        std::cout << "Too few command-line arguments, you have to provide at least the section file name" << std::endl;
        return false;
    }
    if(args[1][0] == '-') {
        std::cout << "Missing section file name" << std::endl;
        return false;
    }
    fileName = args[1];
    short index = 0;
    
    for(auto flag : flags) {
        std::vector<std::string>::const_iterator it= std::find(args.cbegin(), args.cend(), flag);
        if (it != args.cend()) {
            try {
                params[index] = std::stod(std::string(*(it+1)));
            }
            catch (...) { std::cout << "Could not parse the argument associated with " << flag 
                                    << ", falling back to default" << std::endl; }
        }
        index++;
    }
    
    return true;
}

void printInitialData(std::string fileName, double totalWidth, double totalHeight, double bendingMoment_Nm) {
    std::cout << "Analisis started with the following data:\n";
    std::cout << "Section: " << fileName << std::endl;
    std::cout << "Section width: " << totalWidth << " mm" << std::endl;
    std::cout << "Section height: " << totalHeight << " mm" << std::endl;
    std::cout << "Bending moment: " << bendingMoment_Nm << " Nm" << std::endl << std::endl;
}

bool checkFile(std::string fileName) {
    std::ifstream is(fileName);
    if (!is) {
        std::cout << "File cannot be opened" << std::endl;
        return false;
    }
    bool fileIsEmpty = true;
    std::string line;
    std::string matchString = std::string("[^\n\r\t") + TOK + " ]";
    int lineNr = 1;
    while(std::getline(is, line)) {
        if (std::regex_search(line, std::regex(matchString))) {
        std::cout << "File contains foreign characters at line " << lineNr << std::endl;
        return false;
        }
        lineNr++;
    }
    is.clear();
    is.seekg(0);
    while(std::getline(is, line)) {
        if(line.find(TOK) != std::string::npos) {
            fileIsEmpty = false;
            break;
        }
    }
    if (fileIsEmpty) {
        std::cout << "The file is empty" << std::endl;
        return false;
    }
    return  true;
}

std::pair<short,short> getSectionLimitLines(std::string fileName) {
    std::ifstream is(fileName);
    short initial = -1;
    short final = 0;
    short counter = 0;
    short accumulatedEmptyLines = 0;
    std::string line;
    while(std::getline(is, line)) {
        std::size_t pos = line.find(TOK);
        if(pos!=std::string::npos) {
            if(initial == -1) initial = counter;
            accumulatedEmptyLines = 0;
        }
        else {
            accumulatedEmptyLines++;
        }
        counter++;
    }
    final = counter - accumulatedEmptyLines -1;
    return std::make_pair(initial,final);
}

void computeQualifiedBlocks( std::string fileName,
                             std::vector<std::pair<short,std::vector<short>>>& qualifiedBlocks, 
                             std::pair<short, short> sectionLimitLines) {
                        
    std::ifstream is(fileName);
    std::string line1;
    std::string line2;
    std::string line3;
    std::string line = std::string();
    short activeLines = sectionLimitLines.second - sectionLimitLines.first + 1;
    short cuerrentHeight;
    
    //Move to the first line with tokens
    for(int i = 0; i<sectionLimitLines.first; i++) {
        std::getline(is, line);
    }

    std::getline(is, line2);
    line1 = std::string(line2.size(), ' ');
    bool terminateLoop = false;
    short numberOfLines = 0;
    
    bool isFirstLine = true;
 
    while(true) {
        if(is.eof()) {
            line3 = std::string(line2.size(), ' ');
            terminateLoop = true;
        }
        else {
            std::getline(is, line3);
        }
        std::size_t pos = line2.find(TOK);
        cuerrentHeight = activeLines-numberOfLines;
        if(pos!=std::string::npos) {
            qualifiedBlocks.push_back(std::make_pair(cuerrentHeight, 
                                            categorizeLine(line1, line2, line3,isFirstLine, terminateLoop)
                                            ));
        }
        numberOfLines++;
        if(isFirstLine) isFirstLine=false;
        if(terminateLoop) break;
        line1 = line2;
        line2 = line3;
    }                        
}

std::vector<short> categorizeLine(  const std::string& rawPrevLine, 
                                    const std::string& curLine, 
                                    const std::string& rawNextLine,
                                    bool isFirstLine,
                                    bool isLastLine) {
        std::size_t firstPos = curLine.find(TOK);
        std::size_t lastPos = curLine.rfind(TOK);
        if(firstPos == std::string::npos) return std::vector<short>();
        //std::cout << "numero elementos: " << lastPos - firstPos +1 << std::endl;
        
        std::string prevLine = rawPrevLine;
        std::string nextLine = rawNextLine;
        
        short dif1 = curLine.size() - rawPrevLine.size();
        if(dif1 > 0) prevLine = rawPrevLine + std::string(' ', dif1);
        short dif2 = curLine.size() - rawNextLine.size();
        if(dif2 > 0) nextLine = rawNextLine + std::string(' ', dif2);
        
        short blockDescription;
        std::vector<short> lineBlocks;

        for(std::size_t index = firstPos; index <= lastPos; index++) {
            if(curLine[index] != TOK) continue;
            blockDescription = 0;
            if(index!=0 && curLine[index-1] != TOK)                     blockDescription |= leftFlag;
            if((index!=curLine.size()-1) && curLine[index+1] != TOK)    blockDescription |= rightFlag;
            if(!isFirstLine && prevLine[index] != TOK)                  blockDescription |= topFlag;
            if(!isLastLine && nextLine[index] != TOK)                   blockDescription |= botFlag;
            if(blockDescription==0)                                     blockDescription |= internalFlag;
            lineBlocks.push_back(blockDescription);
            //std::cout << "blockDescription = " << blockDescription << std::endl;
        }

        return lineBlocks;
}

void computeRefinedBlocks(const std::vector<std::pair<short,std::vector<short>>>& qualifiedBlocks, 
                                std::vector<std::array<double,4>>& refinedBlocks, 
                                double blockWidth,
                                double blockHeight) {
    for(auto p : qualifiedBlocks) {
        short nominalTopHeight = p.first;
        for (short blockDescription : p.second) {
            double leftSide  = -blockWidth/2;
            double rightSide = blockWidth/2;
            double topSide   = blockHeight*nominalTopHeight;
            double botSide   = blockHeight*(nominalTopHeight-1);
            #ifdef UNCERTAIN
                if(blockDescription & topFlag) topSide += UxHwDoubleUniformDist(-1, 1)*blockHeight/2;
                if(blockDescription & botFlag) botSide -= UxHwDoubleUniformDist(-1, 1)*blockHeight/2;
                if(blockDescription & leftFlag) leftSide += UxHwDoubleUniformDist(-1, 1)*blockWidth/2;
                if(blockDescription & rightFlag) rightSide += UxHwDoubleUniformDist(-1, 1)*blockWidth/2;
            #endif
            refinedBlocks.push_back(std::array<double,4>{topSide,rightSide,botSide,leftSide});
        }
    }
}

short calculateMaxWidthBlocks( const std::vector<std::pair<short, std::vector<short>>>& qualifiedBlocks) {
    short maxBlocks = 0;
    for(auto p : qualifiedBlocks) {
        if (p.second.size() > maxBlocks) maxBlocks = p.second.size();
    }
    return maxBlocks;
}

double calculateVerticalCenter( const std::array<double,4>& sides) {
    return (sides[0] + sides[2])/2;
}

double calculateArea(const std::array<double,4>& sides) {
    return (sides[0] - sides[2])*(sides[1] - sides[3]);
}

double calculateCentroid( const std::vector<std::array<double,4>>& refinedBlocks) {
    double accumulatedMoment = 0;
    double accumulatedArea = 0;
    double area;
    double height;
    for(std::array<double,4> sides : refinedBlocks) {
            area = calculateArea(sides);
            height = calculateVerticalCenter(sides);
            accumulatedArea += area;
            accumulatedMoment += area*height;
    }
    return accumulatedMoment/accumulatedArea;
}

double calculateInertia(const std::vector<std::array<double,4>>& refinedBlocks, double centroidYPosition) {
    double accumulatedIntertia = 0;
    double basePart;
    double steinerPart;
    for(std::array<double,4> sides : refinedBlocks) {
        
            basePart = (sides[1] - sides[3])*std::pow(sides[0] - sides[2],3)/12;
            steinerPart = calculateArea(sides)*std::pow(calculateVerticalCenter(sides)-centroidYPosition,2);
            accumulatedIntertia += basePart + steinerPart;
    }
    return accumulatedIntertia;
}

void showResults(double centroidY, double inertiaY, double maxStress) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Baricenter height: \t" << centroidY << "\tmm" << std::endl;
    std::cout << std::scientific;
    std::cout << "Moment of inertia: \t" << inertiaY << "mm^4" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Maximum stress: \t" << maxStress << "\tMPa" << std::endl;
    return;
}

//Function for debugging
void printMapping(const std::vector<std::pair<short, std::vector<short>>>& qualifiedBlocks) {
    for(auto p : qualifiedBlocks) {
        for(short bl : p.second) {
            switch(bl) {
                case topFlag:
                    std::cout << "t";
                    break;
                case (topFlag | leftFlag):
                    std::cout << "1";
                    break;
                case (topFlag | rightFlag):
                    std::cout << "2";
                    break;
                case (topFlag | botFlag | leftFlag):
                    std::cout << "C";
                    break;
                case (topFlag | botFlag | rightFlag):
                    std::cout << "D";
                    break;
                case botFlag:
                    std::cout << "b";
                    break;
                case (botFlag | leftFlag):
                    std::cout << "4";
                    break;
                case (botFlag | rightFlag):
                    std::cout << "3";
                    break;
                case leftFlag:
                    std::cout << "l";
                    break;
                case rightFlag:
                    std::cout << "r";
                    break;
                case rightFlag | leftFlag:
                    std::cout << "v";
                    break;
                case topFlag | botFlag:
                    std::cout << "h";
                    break;
                case internalFlag:
                    std::cout << "o";
                    break;
            }
        }
        std::cout << std::endl;
    }
}
