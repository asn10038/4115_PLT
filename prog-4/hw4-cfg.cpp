
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/FileSystem.h"
#include <vector>
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

};

typedef std::map<std::string, Node*> NodeMap;
void writeGraph(Function &F, NodeMap &map);
void writeNode(raw_ostream &O, Node* node);

namespace {
  struct HW4CFG : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    

    HW4CFG() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      NodeMap map;
      unsigned int id = 0;

      for (BasicBlock &BB : F) {
        auto nodePtr = new Node();
        nodePtr->id = id++;
        nodePtr->name = BB.getName();
        map[BB.getName()] = nodePtr;
      }

      for (BasicBlock &BB : F) {
        auto nodePtr = map[BB.getName()];
        Instruction* inst = BB.getTerminator();
        if (auto *br = dyn_cast<BranchInst>(inst)) {
          Node* succPtr;
          if (br->isConditional()) {
            succPtr = map[br->getSuccessor(0)->getName()];
            nodePtr->edges.push_back(new Edge("T", succPtr->id));
            succPtr = map[br->getSuccessor(1)->getName()];
            nodePtr->edges.push_back(new Edge("F", succPtr->id));
          } else {
            succPtr = map[br->getSuccessor(0)->getName()];
            nodePtr->edges.push_back(new Edge("", succPtr->id));
          }
        }
      }
      writeGraph(F, map);
      return false;
    }

    bool doFinalization(Module &M) override {
      return false;
    }
  };
}

char HW4CFG::ID = 0;
static RegisterPass<HW4CFG> X("hw4-cfg", "hw4 q2 pass");


void writeGraph(Function &F, NodeMap &map) {
  std::string Filename = (F.getName() + ".dot").str();
  errs() << "Writing '" << Filename << "'...";
  std::error_code EC;
  raw_fd_ostream File(Filename, EC, sys::fs::OF_Text);

  File << "digraph \"CFG for \'" << F.getName() << "\' function\" {\n";
  File << "label=\"CFG for \'" << F.getName() << "\' function\";\n";

  for (auto const& entry : map) {
    writeNode(File, entry.second);
  }
  File << "}\n";
  errs() << "\tDone\n";
}

void writeNode(raw_ostream &O, Node* node) {
  O << "\tNode" << node->id << " [shape=record, ";
  if (node->edges.size() < 2) {
    O << "label=\"{" << node->name << "}\"];\n";
  } else {
    O << "label=\"{" << node->name << "|{<s0>" << node->edges[0]->key << "|<s1>" << node->edges[1]->key << "}}\"];\n";
  }

  if (node->edges.size() == 1) {
    O << "\tNode" << node->id << " -> " << "Node" << node->edges[0]->dest << ";\n";
  } else if (node->edges.size() == 2) {
      O << "\tNode" << node->id << ":s0" << " -> " << "Node" << node->edges[0]->dest << ";\n";
      O << "\tNode" << node->id << ":s1" << " -> " << "Node" << node->edges[1]->dest << ";\n";
  }
}