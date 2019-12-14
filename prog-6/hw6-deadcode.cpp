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


namespace {



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
                dumpCounts();
                //done = checkIfDone();
                resetCounts();
            }
            return true;
                
        }
    };

}


char DeadCodePass::ID = 0;
static RegisterPass<DeadCodePass> X("efunc", "My Liveness Set Pass");
