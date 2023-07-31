#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <utility>
#include <regex>
#include <iomanip>
#include <cmath>

constexpr char TOK = '#';
constexpr short leftFlag =      0b00001;
constexpr short rightFlag =     0b00010;
constexpr short topFlag =       0b00100;
constexpr short botFlag =       0b01000;
constexpr short internalFlag =  0b10000;

bool parseArguments(const std::vector<std::string>& args, std::string& fileName, std::array<double,3>& params);

void printInitialData(std::string fileName, double totalWidth, double totalHeight, double bendingMoment_Nm);

bool checkFile(std::string fileName);

std::pair<short,short> getSectionLimitLines(std::string fileName);

void computeQualifiedBlocks(std::string fileName, 
                            std::vector<std::pair<short,std::vector<short>>>& blocks, 
                            std::pair<short, short> sectionLimitLines);
                            
void computeRefinedBlocks(const std::vector<std::pair<short,std::vector<short>>>& qualifiedBlocks, 
                                std::vector<std::array<double,4>>& refinedBlocks,
                                double blockWidth,
                                double blockHeight);

std::vector<short> categorizeLine(  const std::string& curLine, 
                                    const std::string& prevLine, 
                                    const std::string& nexLine,
                                    bool isFirstLine,
                                    bool isLastLine);
                                    
short calculateMaxWidthBlocks(const std::vector<std::pair<short, 
                               std::vector<short>>>& blocks);
                               
double calculateVerticalCenter(const std::array<double,4>& sides);

double calculatArea(const std::array<double,4>& sides);
                                    
double calculateCentroid(const std::vector<std::array<double,4>>& refinedBlocks);
                              
double calculateInertia(const std::vector<std::array<double,4>>& refinedBlocks, double centroidYPosition);

void showResults(double centroidY, double inertiaY, double maxStress);

//Function for debugging
void printMapping(const std::vector<std::pair<short, std::vector<short>>>& blocks);
                    
#endif
