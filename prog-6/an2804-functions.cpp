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
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/IR/ValueSymbolTable.h"
#include <iostream>
#include <sstream>
#include <set>
#include <vector>
#include <map>

using namespace llvm;
using namespace std;

typedef std::vector<Instruction*> InstVector;
typedef std::map<Instruction*, InstVector*> InstLinkMap;


namespace {

    std::string variable_to_string(Value * v);
    std::string getDefined(Instruction &I);
    std::set<std::string>* getUsed(Instruction &I);
    std::set<std::string>* getUsed(PHINode &I, BasicBlock* BB);
    void insertPrevInsts(InstLinkMap& map, Function &F);
    InstVector* getExits(InstLinkMap& map);

    unsigned int globalIdNum = 0;

    class Node {
    public:
        unsigned int idNum;
        Instruction *inst;
        std::set<Instruction*> children;

        bool exitNode = false;
        bool branchNode = false;
        bool phiNode = false;

        bool isExitNode() const { return exitNode; }
        bool isBranchNode() const { return branchNode; }
        bool isPhiNode() const { return phiNode; }

        Node(Instruction *I) : inst(I) {
           idNum = globalIdNum;
           globalIdNum++;
        }
        std::set<Instruction*> getChildren() { return children; }
        void addChild(Instruction *I) { children.insert(I); }
        Instruction* getInst() const { return inst; }
        unsigned int getIdNum() const { return idNum; }
        
        
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

        void removeLiveOut(std::string str) const { liveOut.erase(str); }

        void insertLiveOut(std::set<std::string> strSet) const { liveOut.insert(strSet.begin(), strSet.end()); }
    };
    
    class Graph {
    public:
        std::set <Node> g;
        void addNode(Node &n) { g.insert(n); }
        void clear() { g.clear(); }

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
            //re order the nodes based on ID in the map
            std::map<int, std::string> outputMap;
            for(auto n : g) {
                int mapKey = n.getIdNum();
                std::stringstream ss;
                std::string instString;
                raw_string_ostream rso(instString);
                n.getInst()->print(rso);

                ss << "Instruction: \t";
                ss << instString;
                ss << "-->\n";
                ss << "  liveness OUT: {";
                for(auto v : n.liveOut) {
                    ss << v << " ";
                }
                ss << "}\n";
                outputMap[mapKey] = ss.str();
            }
            //output the strings
            for (auto it : outputMap) {
                errs() << it.second;
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

                        if(!child->isPhiNode()) n.insertLiveOut(child->getLiveIn());
                        else {
                            // insert the live in variables from the phi node
                            n.insertLiveOut(child->getLiveIn());
                            //insert the live variable from this edge to the PHI node
                            BasicBlock *BB = n.getInst()->getParent();
                            std::stringstream ss;
                            ss << "%" << BB->getName().str();
                            
                            PHINode *pI = dyn_cast<PHINode>(i);
                            for (unsigned int i = 0; i < pI->getNumIncomingValues(); i++) {
                                std::string block = variable_to_string(pI->getIncomingBlock(i));
                                auto val = variable_to_string(pI->getIncomingValue(i));
                                if(block == ss.str()) {
                                    n.insertLiveOut(val);
                                } else {
                                    n.removeLiveOut(val);    
                                }
                            }
                        }
                    }

                    //  check for ending
                    if(finished && (tmpLiveIn != n.getLiveIn() || tmpLiveOut != n.getLiveOut())) { finished = false; }

                }
            }
        }
        std::set<Instruction*> removeDeadVariables() {
            std::set <Instruction*> res;
            for(auto &n : g) {
                std::string defined = n.def;
                //if nothing is defined ignore this instruction
                if(n.def == "") continue;

                auto outSet = n.getLiveOut();
                if(outSet.find(n.def) == outSet.end()) {
                    errs() << "Should be removed: " << n.def << "\n";
                    auto I = n.getInst();
                    I->replaceAllUsesWith(UndefValue::get(I->getType()));
                    I->eraseFromParent();
                    res.insert(I);
                    errs() << "Removed declaration of: " << n.def << "\n";
                }
            }
            return res;
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
            PHINode *pI = dyn_cast<PHINode>(&I);
            BranchInst *bI = dyn_cast<BranchInst>(&I);
            bool isTerminator = I.isTerminator();
            if (pI) {
                n.phiNode = true;
                n.addChild(I.getNextNode());
            } else if(bI || isTerminator) {
                if(bI) n.branchNode = true;
                if(isTerminator) n.exitNode = true;
                for (unsigned int i = 0; i < I.getNumSuccessors(); i++) {
                    auto &instList = I.getSuccessor(i)->getInstList();
                    auto beginInst = instList.begin();
                    if (beginInst == instList.end()) {
                        continue;
                    }
                    n.addChild(&*beginInst);
                }
                n.branchNode = true;

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
            bool done = false;
            //implement the liveness analysis algorithm here.
            #if 0
            InstLinkMap prevInstMap;
            insertPrevInsts(prevInstMap, F);
            #endif
            std::set<Instruction*> removedInstructions;
            while(!done) {
                for (BasicBlock &BB : F) {
                    for (Instruction &I : BB) {
                        if(removedInstructions.find(&I) == removedInstructions.end())
                            addToGraph(I);
                    }
                }
                //flowGraph.dump();
                flowGraph.doLivenessAnalysis();
               // errs() << " ---------------------------------------- \n\n ";
                flowGraph.dumpLiveOut();
                std::set<Instruction*> tmp = flowGraph.removeDeadVariables();
                if(tmp.empty()) done = true;

                removedInstructions.insert(tmp.begin(), tmp.end());
                flowGraph.clear();
            }
            return true;
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
    std::set<std::string>* getUsed(PHINode &pI, BasicBlock *BB) {
        std::set<std::string>* usedSet = new std::set<std::string>();
        Value* val = pI.getIncomingValueForBlock(BB);
        usedSet->insert(variable_to_string(val));
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

char Liveness::ID = 0;
static RegisterPass<Liveness> Y("einst", "removes unused instructions");

/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

/* This is the start of the pass for the first part of the homework */
    struct DeadCodePass : public ModulePass {
        static char ID;
        std::map<Function*, int> callCounts;
        std::set<Function*> removedFuncs;

        DeadCodePass() : ModulePass(ID) {}

        void dumpCounts() {
            for(auto pair : callCounts) {
                errs() << pair.first << " : " << pair.second << "\n";
            }
        }

        bool removeFunction(Function* F) {
            bool res = false;
            if(F->getName() != "main") {
                errs() << "removing: " << F->getName() << "\n";
                F->replaceAllUsesWith(UndefValue::get(F->getType()));
                F->eraseFromParent();
                res = true;
                removedFuncs.insert(F);
            }
            return res;
        }

        /* returns true if something was removed and false otherwise */
        bool removeUncalled() {
            bool res = false;
            for(auto pair : callCounts) {
                if(!pair.second) {
                    if(removeFunction(pair.first))
                        res = true;
                }
            }
            return res;
        }

        void resetCounts() {
            callCounts.clear();
        }


        bool runOnModule(Module &M) override {
            bool done = false;
            while(!done) {
                for(Function &F : M.getFunctionList()) { 
                    if(removedFuncs.find(&F) != removedFuncs.end()) continue;

                    std::pair<Function*, int> pair(&F, 0);
                    callCounts.insert(pair);
                    
                    for(auto &BB : F) {
                        for(auto &I : BB) {
                            if(auto *callInst = dyn_cast<CallInst>(&I)) {
                                auto targetFunction = callInst->getCalledFunction();
                                if(callCounts.find(targetFunction) == callCounts.end()) {
                                    callCounts[targetFunction] = 1;
                                } else {
                                    callCounts[targetFunction]++;
                                }
                            }
                        }
                    }
                }
                done = !removeUncalled();
                //dumpCounts();
                //done = checkIfDone();
                resetCounts();
            }
            return true;
                
        }
    };


char DeadCodePass::ID = 0;
static RegisterPass<DeadCodePass> X("efunc", "removes unused functions");

}// end namespace
