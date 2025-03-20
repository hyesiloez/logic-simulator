  /*
 * AnalysisEDA.cpp
 *
 * This file contains the implementation of the simulator.
 */

#include "AnalysisEDA.h"
#include "Circuit/Circuit.h"
#include <iostream>
#include <algorithm>
#include <regex>
#include <string.h>       
#include <unordered_map>    
#include <queue>            
#include <vector>           


//Calc helper methods for later

Logic notGatter (Logic inp){
    Logic outValue;
     if (inp == Logic::logicX){
        outValue = Logic::logicX;
     } 
     if (inp == Logic::logic1){
        outValue = Logic::logic0;
     } 
     if (inp == Logic::logic0){
        outValue = Logic::logic1;
    }
    return outValue;         
};

Logic orgatter(Logic inp1,Logic inp2){
    Logic outValue;
    if (inp1== Logic::logic1 || inp2 == Logic::logic1){
        outValue = Logic::logic1;
    } else if ((inp1== Logic::logicX && inp2 == Logic::logicX) || 
               (inp2 == Logic::logicX && inp1 == Logic::logicX)){
        outValue = Logic::logicX;
    } else if ((inp1 == Logic::logicX && inp2 == Logic::logic0) || 
               (inp2 == Logic::logicX && inp1== Logic::logic0)){
        outValue = Logic::logicX;
    } else if (inp1== Logic::logic0 && inp2 == Logic::logic0){
        outValue = Logic::logic0;
    }
    return outValue;
};

Logic andgatter(Logic inp1,Logic inp2){
    Logic outValue;
    if (inp1 == Logic::logic0 || inp2== Logic::logic0){
            outValue = Logic::logic0;
    } else if ((inp1 == Logic::logic1 && inp2== Logic::logic1)){
            outValue = Logic::logic1;
    } else if ((inp1 == Logic::logicX && inp2== Logic::logic1) || 
                (inp1 == Logic::logic1 && inp2== Logic::logicX)){
            outValue = Logic::logicX;
    } else{
            outValue = Logic::logicX;
    }
    return outValue;
}

//sort elements 
std::vector<const Element*> sortElements(const Circuit* circuit){

    auto elements = circuit->getAllElements();

    
    std::unordered_map<const Element*, int> inDegree;
    std::queue<const Element*> zeroInDegreeQueue;
    std::vector<const Element*> topoOrder;

    
    for (const Element* elem : elements) {
        inDegree[elem] = 0;
    }

   
    for (const Element* elem : elements) {
        for (const Net* inNet : elem->getInNets()) {
            if (inNet->getInElement()) {
                inDegree[elem]++;
            }
        }
    }

    
    for (const Element* elem : elements) {
        if (inDegree[elem] == 0) {
            zeroInDegreeQueue.push(elem);
        }
    }

    while (!zeroInDegreeQueue.empty()) {
        const Element* current = zeroInDegreeQueue.front();
        zeroInDegreeQueue.pop();
        topoOrder.push_back(current);

        for (const Net* outNet : current->getOutNets()) {
            for (const Element* nextElem : outNet->getOutElements()) {
                inDegree[nextElem]--;
                if (inDegree[nextElem] == 0) {
                    zeroInDegreeQueue.push(nextElem);
                }
            }
        }
    }
    
    if (topoOrder.size() != elements.size()) {
        std::sort(elements.begin(), elements.end(), [](const auto& a, const auto& b) {
             return std::stoi(a->getName().substr(1)) < std::stoi(b->getName().substr(1));
        });
        
        topoOrder = elements;
    }
    return topoOrder;
}






void AnalysisEDA::run() {

    auto nets = circuit->getAllNets();
    auto elements = circuit->getAllElements();
    elements = sortElements(circuit);
    size_t netsNum = nets.size();
    std::vector<Logic> netsOutputs;

    
    for (size_t i = 0;  i < netsNum; i++){
        netsOutputs.emplace_back(Logic::logicX);
    } 
   
    
    for (const auto& inputVector : inputData){

        std::vector<Logic> outputs;
        
   
        std::vector<const Element*> flipflops;
        
        for (const Element* elem : elements){
            if (elem->getElementInfo()->getType() == ElementType::Dff){
                flipflops.emplace_back(elem);
            } 
        } 

        
        for (const Element* element : flipflops){
            auto NetsIn = element->getInNets();
            auto outNets = element->getOutNets();
            const Net* inNet2 = NetsIn.at(1);
            size_t inNet2ID = inNet2->getId();
            Logic in2Value = netsOutputs.at(inNet2ID);
            auto outNet = outNets.at(0);
            size_t outNetID = outNet->getId();
            netsOutputs.at(outNetID) = in2Value;  
        }

        size_t ind = 0;
        for (size_t i = 0; i < netsNum ; i++){
            if (nets[i]->getInElement() == nullptr){
                Logic input = inputVector.at(ind);
                netsOutputs[i] = input;
                ind++;
            } 
        } 

        
        for (const Element* elem : elements){
            if (elem->getElementInfo()->getType() != ElementType::Dff){
                ElementType type = elem->getElementInfo()->getType();
                auto inNets = elem->getInNets();
                
                size_t size = inNets.size();
                auto outNets = elem->getOutNets();
                
                const Net* inNet1 = inNets.at(0);
                const Net* inNet2;
                size_t inNet2ID;
                auto outNet = outNets.at(0);
                size_t outNetID = outNet->getId();
                size_t inNet1ID = inNet1->getId();
                Logic in2Value;
                if (size > 1){
                    inNet2 = inNets.at(1);
                    inNet2ID = inNet2->getId();
                    in2Value = netsOutputs.at(inNet2ID);
                } 
                Logic in1Value = netsOutputs.at(inNet1ID);
                Logic outValue;
                switch (type){
                    case ElementType::Not:
                        outValue = notGatter(in1Value);
                        break;
                    case ElementType::Or:
                        outValue = orgatter(in1Value,in2Value);
                        break;
                    case ElementType::And:
                        outValue = andgatter(in1Value,in2Value);
                        break;
                    default:
                        break;
                }

                netsOutputs.at(outNetID) = outValue;
            }
        }
        
        for (auto net : nets){
            if (net->getOutElements()[0] == nullptr){
                size_t netId = net->getId();
                outputs.push_back(netsOutputs.at(netId));
            }  
        } 

        for (size_t i = 0; i < outputs.size(); i++){
            std::cout <<  outputs.at(i) << " ";
            
        } 
        std::cout << std::endl;
    }
}