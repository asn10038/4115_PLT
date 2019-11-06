#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/InstVisitor.h"


using namespace llvm;
#define DEBUG_TYPE "hw4"

class Edge {
public:
    std::string key;
    unsigned int dest;
    Edge(std::string key, unsigned int dest) : key(key), dest(dest) {};
};

class Node {
public:
    unsigned int id;
    std::string name;
    std::vector<Edge*> edges;
    Node() {};
    Node(unsigned int id, std::string n) : id(id), name(n) {}
    void addEdge(unsigned int dest) { edges.push_back(new Edge(name, dest)); }

};

typedef std::map<std::string, Node*> NodeMap;
void writeGraph(NodeMap &map);
void writeNode(raw_ostream &O, Node* node);
                            
namespace {
    class CallSiteVisitor : public InstVisitor<CallSiteVisitor> {
        private:
            std::string origin;
            NodeMap map;
        public:
            CallSiteVisitor(std::string origin, NodeMap &map) : origin(origin), map(map) {}            
            void visitCallInst(CallInst &I) {
                auto destName = I.getCalledFunction()->getName();
                auto node = map[origin];
                auto destNode = map[destName];
                node->addEdge(destNode->id);
            }
    };

    class HW4CallGraph : public ModulePass {
    private:
        NodeMap map;
    public:
        ~HW4CallGraph() { 
            writeGraph(map);
        }

        std::vector<Node> nodes;
        static char ID; //Pass identification
        HW4CallGraph() : ModulePass(ID) { }
        virtual bool runOnModule(Module &M) override;
        
    };

}

bool HW4CallGraph::runOnModule(Module &M) {
    unsigned int id = 0;
    for(Function &F : M ) {
        id++;
        auto nodePtr = new Node(id, F.getName());
        map[F.getName()] = nodePtr;
    }
    for(Function &F : M) {
        CallSiteVisitor CSV(F.getName(), map);
        CSV.visit(F);
    }
    
    return false;
}

char HW4CallGraph::ID = 0;
static RegisterPass<HW4CallGraph> X("hw4-callgraph", "hw4 q1 pass");

void writeGraph(NodeMap &map) {
    //std::string Filename = ("CallGraph" + ".dot").str();
    std::string Filename = "callgraph.dot";
    errs() << "Writing '" << Filename << "'...";
    std::error_code EC;
    raw_fd_ostream File(Filename, EC, sys::fs::OF_Text);

    File << "digraph \"CallGraph for \'" << "file"  << "\' \" {\n";
    File << "label=\"CG for \'" << "file" << "\' \";\n";

    for (auto const& entry : map) {
        writeNode(File, entry.second);
    }
    File << "}\n";
    errs() << "\tDone\n";
}

void writeNode(raw_ostream &O, Node* node) {
    O << "\tNode" << node->id << " [shape=record, ";
    O << "label=\"{" << node->name << "}\"];\n";

    for (auto edge : node->edges) {
       O << "\t Node" << node->id << " -> " << "Node" << edge->dest << ";\n";
    }
}
