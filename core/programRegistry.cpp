/*
Class for adding programs to the registry
*/
#include "programRegistry.hpp"

programRegistry::programRegistry() {
    this->pReg = std::vector<programEntry>();
    this->pAlias = std::vector<programAlias>();
    this->emptyStringList = std::vector<String>();
}

void programRegistry::addProgram(String name, programRegistry::functionPtr program) {
    programEntry entry;
    entry.name = name;
    entry.function = program;
    programRegistry::pReg.push_back(entry);
}

programRegistry::returnState programRegistry::runProgram(pEnv env, String name, std::vector<String>* args) {
    for (int i = 0; i < this->pAlias.size(); i++) {
        if (this->pAlias[i].from == name) {
            name = this->pAlias[i].to->at(0);
            args->insert(args->begin(), this->pAlias[i].to->begin() + 1, this->pAlias[i].to->end());
            break;
        }
    }
    returnState state;
    for (int i = 0; i < this->pReg.size(); i++){
        programEntry entry = this->pReg.at(i);
        if (entry.name == name) {
            state.found = true;
            state.index = i;
            state.returncode = entry.function(env, args);
            delete args;
            return state;
        }
    }
    state.found = false;
    state.index = -1;
    state.returncode = -1;
    delete args;
    return state; // not found 
}

programRegistry::returnState programRegistry::runProgram(pEnv env, String name) {
    return programRegistry::runProgram(env, name, &emptyStringList);
}

programRegistry::returnState programRegistry::runProgram(pEnv env, std::vector<String>* args) {
    String name = args->at(0);
    args->erase(args->begin());
    return programRegistry::runProgram(env, name, args);
}

void programRegistry::removeProgram(String name) {
    for (int i = 0; i < this->pReg.size(); i++){
        programEntry entry = this->pReg.at(i);
        if (entry.name == name) {
            this->pReg.erase(this->pReg.begin() + i);
            return;
        }
    }
}

void programRegistry::addAlias(String from, String to) {
    programRegistry::programAlias alias;
    alias.from = from;
    alias.to = this->parseArgumentList(to);
    this->pAlias.push_back(alias);
}


void programRegistry::removeAlias(String from) {
    for (int i=0;i<this->pAlias.size(); i++) {
        if (this->pAlias[i].from == from) {
            delete this->pAlias[i].to;
            this->pAlias.erase(this->pAlias.begin()+i);
        }
    }
}

std::vector<String>* programRegistry::parseArgumentList(String sList) {
    sList.trim();
    std::vector<String>* output = new std::vector<String>;

    // nothing to see here
    if (sList == "") {
        return output;
    }
        
    sList += " ";
    int start = 0;
    int end = 0;
    int quote = 0;
    int quoteEnd = 0;
    String part = "";
    while (1){
        // Serial.println(String()+"S"+start+" E"+end+" O"+output.size());
        end = sList.indexOf(" ",start);
        if (end == -1) {
            break;
        }
        part = sList.substring(start,end);
        // Serial.println("> "+part);

        quote = part.indexOf("\"");
        if (quote != -1) {
            quoteEnd = sList.indexOf("\"",quote+start+1);
            if (quoteEnd != -1) {
                part = sList.substring(start+1,quoteEnd);
                // Serial.println(">>> "+part);
                end = quoteEnd;
            }
        }
        start = end + 1;
        
        if (part != "")
            output->push_back(part);
    }
    // for (int i=0; i < output.size(); i++) {
    //     Serial.println(String()+i+": \""+output.at(i)+"\"");
    // }
    return output;
}