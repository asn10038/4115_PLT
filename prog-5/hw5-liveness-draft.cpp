#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/ilist_node.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/GlobalObject.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/OperandTraits.h"
#include "llvm/IR/SymbolTableListTraits.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/IR/ValueSymbolTable.h"
#include <iostream>
#include <set>
#include <vector>

using namespace llvm;
using namespace std;

typedef std::vector<Instruction*> InstVector;
typedef std::map<Instruction*, InstVector*> InstLinkMap;

namespace {

    std::string variable_to_string(Value * v);
    std::string getDefined(Instruction &I);
    std::set<std::string>* getUsed(Instruction &I);
    void insertPrevInsts(InstLinkMap& map, Function &F);
    InstVector* getExits(InstLinkMap& map);

    class Node {
    public:
        Instruction *inst;
        std::set<Instruction*> children;

        bool exitNode = false;
        bool branchNode = false;
        bool phiNode = false;

        bool isExitNode() { return exitNode; }
        bool isBranchNode() { return branchNode; }
        bool isPhiNode() { return phiNode; }

        Node(Instruction *I) : inst(I) {}
        std::set<Instruction*> getChildren() { return children; }
        void addChild(Instruction *I) { children.insert(I); }
        Instruction* getInst() const { return inst; }
        
        
        std::set<std::string> use;
        std::string def;

        // maybe maintain a live in and live out set here
        mutable std::set<std::string> liveIn;
        mutable std::set<std::string> liveOut;

        // define the < operator so they can go in a set
        // using the pointer addr for now
        bool operator< (const Node &n) const { return (inst < n.getInst()); }

        //interface for pointer issue
        void setUsed(std::set<std::string>* usedSetPtr) { use = *usedSetPtr; }

        void clearLiveIn() { liveIn.clear(); }
        void clearLiveOut() { liveOut.clear(); }

        std::set<std::string> getLiveIn() const { return liveIn; }
        std::set<std::string> getLiveOut() const { return liveOut; }
        std::set<std::string> getUsed() const { return use; }
        std::string getDefd() const { return def; }

        void insertLiveIn(std::string str) const { liveIn.insert(str); }
        void insertLiveOut(std::string str) const { liveOut.insert(str); }

        void insertLiveOut(std::set<std::string> strSet) const { liveOut.insert(strSet.begin(), strSet.end()); }
    };
    
    class Graph {
    public:
        std::set <Node> g;
        void addNode(Node &n) { g.insert(n); }
        void dump() {
            for(auto n : g) {
               n.getInst()->print(errs());
               errs() << "\n";
               for( auto str : n.use ) {
                   errs() <<"\t Uses: " << str << "\n" ;
               }
               if (n.def != "") errs() << "\t \t Defines: " << n.def << "\n";
            }
        }
        void dumpLiveOut() {
            for(auto n : g) {
                errs() << "Instruction: \t";
                n.getInst()->print(errs());
                errs() << "-->\n";
                errs() << "  liveness OUT: {";
                for(auto v : n.liveOut) {
                    errs() << v << " ";
                }
                errs() << "}\n";
            }
        }

        void clearLiveness() {
            for(auto n : g) {
                n.clearLiveIn();
                n.clearLiveOut();
            }
        }

        void doLivenessAnalysis() {
            //initialization
            clearLiveness();
            bool finished = false;
            while(!finished) {
                finished = true;
                for(auto &n : g) {
                    std::set<std::string> tmpLiveIn = n.getLiveIn();
                    std::set<std::string> tmpLiveOut = n.getLiveOut();

                    //add the items from use
                    for(auto str : n.getUsed()) n.insertLiveIn(str);

                    //add the items from liveout that aren't defined
                    for(auto str : n.getLiveOut()) {
                        if(str != n.getDefd()) n.insertLiveIn(str);
                    }

                    //update live out
                    for(auto i : n.children) {
                        auto child = g.find(i);
                        if(child == g.end()) {
                            errs() << "ABORT: [doLivenessAnalysis] Processing poorly constructed graph";
                            exit(1);
                        }
                        n.insertLiveOut(child->getLiveIn());
                    }

                    //  check for ending
                    if(finished && (tmpLiveIn != n.getLiveIn() || tmpLiveOut != n.getLiveOut())) { finished = false; }

                }
            }
        }
    };

    struct Liveness : public FunctionPass {
        static char ID; // Pass identification, replacement for typeid
        Graph flowGraph;
        Liveness() : FunctionPass(ID) {}

        void addToGraph(Instruction &I) {
            Node n(&I);    

            //handle use and def sets
            std::set<std::string>* usedSet = getUsed(I);
            n.setUsed(usedSet); 
            n.def = getDefined(I);

            // handle the children
            // special case designations
            if (PHINode* pI = dyn_cast<PHINode>(&I)) {
                errs() << "UNHANDLED PHI NODE IN GRAPH\n";
                n.phiNode = true;
            } else if(BranchInst* bI = dyn_cast<BranchInst>(&I)) {
                errs() << "UNHANDLED BRANCH INST IN GRAPH\n";
                n.branchNode = true;
            } else if (I.isTerminator()) {
                n.exitNode = true;
                for (unsigned int i = 0; i < I.getNumSuccessors(); i++) {
                    auto &instList = I.getSuccessor(i)->getInstList();
                    auto beginInst = instList.begin();
                    if (beginInst == instList.end()) {
                        continue;
                    }
                    errs() << "Adding child to terminator node";
                    n.addChild(&*beginInst);
                }

                #if 0
                auto &instList = I.getSuccessor(i)->getInstList();
                auto beginInst = instList.begin();
                if (beginInst == instList.end()) {
                    continue;
                }
                map[&*beginInst]->push_back(&I);
                #endif
            } else {
                n.addChild(I.getNextNode());
            }
            
            
            flowGraph.addNode(n);
            
        }

        bool runOnFunction(Function &F) override {
            //implement the liveness analysis algorithm here.
            #if 0
            InstLinkMap prevInstMap;
            insertPrevInsts(prevInstMap, F);
            #endif
            for (BasicBlock &BB : F) {
                for (Instruction &I : BB) {
                    addToGraph(I);
                    #if 0

                    errs() << "instruction: ";
                    I.print(errs());
                    errs() << "\n";

                    if (PHINode* pI = dyn_cast<PHINode>(&I)) {
                        errs() << "is PhiNode" << "\n";
                        for (unsigned int i = 0; i < pI->getNumIncomingValues(); i++) {
                            errs() << "value: " << variable_to_string(pI->getIncomingValue(i)) << " ";
                            errs() << "from Block: " << variable_to_string(pI->getIncomingBlock(i)) << "\n";
                        }
                    }

                    std::string definedValue = getDefined(I);
                    if (definedValue.length() > 0) {
                        errs() << "<Defined>: " << definedValue << "\n";
                    } else {
                        errs() << "Nothing defined!" << "\n";
                    }

                    std::set<std::string>* usedSet = getUsed(I);
                    for (std::set<std::string>::iterator it = usedSet->begin(); it != usedSet->end(); it++) {
                        errs() << "<Used>: " << *it<< "\n";
                    }
                    #endif
                }
            }
            #if 0
            errs() << "\n\n";
            for (auto exitInst: *getExits(prevInstMap)) {
                errs() << "Exit Instruction: ";
                exitInst->print(errs());
                errs() << "\n";
            }
            for (auto const& entry : prevInstMap) {
                errs() << "instruction: ";
                entry.first->print(errs());
                errs() << "\n";
                for (auto ptNextI : *entry.second) {
                    errs() << "previous: ";
                    ptNextI->print(errs());
                    errs() << "\n";
                }
            }
            errs() << " ---------------------------------------- \n\n ";
            #endif
            flowGraph.dump();
            flowGraph.doLivenessAnalysis();
            errs() << " ---------------------------------------- \n\n ";
            flowGraph.dumpLiveOut();
            return false;
        }
    };

    void insertPrevInsts(InstLinkMap& map, Function &F) {
        // build the vector for every instructions
        for (BasicBlock &BB : F) {
             for (Instruction &I : BB) {
                 map[&I] = new InstVector();
            }
        }
        for (BasicBlock &BB : F) {
             for (Instruction &I : BB) {
                    if (I.isTerminator()) {
                        for (unsigned int i = 0; i < I.getNumSuccessors(); i++) {
                            auto &instList = I.getSuccessor(i)->getInstList();
                            auto beginInst = instList.begin();
                            if (beginInst == instList.end()) {
                                continue;
                            }
                            map[&*beginInst]->push_back(&I);
                        }
                    } else {
                        map[I.getNextNode()]->push_back(&I);     
                    }
            }
        }
    }

    InstVector* getExits(InstLinkMap& map) {
        auto exitInstVec = new InstVector();
        for (auto const& entry : map) {
            if (entry.second->empty()) {
                exitInstVec->push_back(entry.first);
            }
        }
        return exitInstVec;
    }

    std::string getDefined(Instruction &I) {
        if (!I.getType()->isVoidTy()) {
            Value* ptrDefinedVal = cast<Value> (&I);
            return variable_to_string(ptrDefinedVal);
        }
        else {
            return "";
        }
    }
    std::set<std::string>* getUsed(Instruction &I) {
        std::set<std::string>* usedSet = new std::set<std::string>();
        User::op_iterator opnd = I.op_begin(), opE = I.op_end();
        for (; opnd != opE; ++opnd) {
            Value *val = *opnd;
            if (isa<Instruction>(val) || isa<Argument>(val)) {
                usedSet->insert(variable_to_string(val));
            }
        }
        return usedSet;
    }

    std::string variable_to_string(Value * v) {
        if (v->getName().str().length() > 0) {
            return "%" + v->getName().str();
        }
        else if (isa<Instruction>(v)) {
            std::string s = "";
            raw_string_ostream * strm = new raw_string_ostream(s);
            v->print(*strm);
            std::string inst = strm->str();
            size_t idx1 = inst.find("%");
            size_t idx2 = inst.find(" ",idx1);
            if (idx1 != std::string::npos && idx2 != std::string::npos) {
                return inst.substr(idx1,idx2-idx1);
            }
            else {
                return "\"" + inst + "\"";
            }
        }
        else if (ConstantInt * cint = dyn_cast<ConstantInt>(v)) {
            std::string s = "";
            raw_string_ostream * strm = new raw_string_ostream(s);
            cint->getValue().print(*strm,true);
            return strm->str();
        }
        else {
            std::string s = "";
            raw_string_ostream * strm = new raw_string_ostream(s);
            v->print(*strm);
            std::string inst = strm->str();
            return "\"" + inst + "\"";
        }
    }
}

char Liveness::ID = 0;
static RegisterPass<Liveness> X("liveness", "My Liveness Set Pass");
