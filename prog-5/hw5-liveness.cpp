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

    struct Liveness : public FunctionPass {
        static char ID; // Pass identification, replacement for typeid
        Liveness() : FunctionPass(ID) {}

        bool runOnFunction(Function &F) override {
            //implement the liveness analysis algorithm here.
            InstLinkMap prevInstMap;
            insertPrevInsts(prevInstMap, F);
            for (BasicBlock &BB : F) {
                for (Instruction &I : BB) {
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
                }
            }
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