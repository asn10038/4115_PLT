//===- Hello.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
using namespace llvm;

#define DEBUG_TYPE "hw3"

class FuncInfo;
std::map<std::string, FuncInfo> infoMap;
typedef std::map<std::string, unsigned int> StringIntMap;
StringIntMap callSiteMap;

class FuncInfo {
public:
    unsigned int instCounter;
    unsigned int bbCounter;
    unsigned int callCounter;
    std::string name;
    unsigned int argNum;

    FuncInfo() : instCounter(0), bbCounter(0), callCounter(0), name(""), argNum(0) {}

    void fillCallSiteCount(StringIntMap &map) {
        for(auto &pair : map) {
           if(pair.first == name) {
             callCounter = pair.second; 
           }
        }
    }

    void dump() {
        errs() << name;
        if(argNum != 9999)
            errs() << " args=" << argNum;
        else
            errs() << " args=*";
        errs() << " callsites=" << callCounter
         << " basicblocks=" << bbCounter
         << " instructions=" << instCounter
         << " \n";
    }
};

namespace {
  struct CallSiteVisitor : public InstVisitor<CallSiteVisitor> {
    public:
        void visitCallInst(CallInst &I) {
          auto funcName = I.getCalledFunction()->getName();
          callSiteMap[funcName]++;
        }
  };
  // HW3 pass that records all the info 
  struct HW3 : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    

    HW3() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      //Visit and check call sites
      CallSiteVisitor CSV;
      CSV.visit(F);

      FuncInfo fi;
      fi.instCounter = F.getInstructionCount();
      fi.bbCounter = F.getBasicBlockList().size();
      fi.name = F.getName();

      fi.argNum = (F.isVarArg()) ? 9999 : F.arg_size();
      //infoMap.insert(std::pair<std::string, FuncInfo>(fi.name, fi));
      infoMap[fi.name] = fi;
      //fi.dump();
      return false;
    }

    bool doFinalization(Module &M) {
        for (auto &x : infoMap) {
            x.second.fillCallSiteCount(callSiteMap);
            x.second.dump();
        }
        return false;
    }
  };
}

char HW3::ID = 0;
static RegisterPass<HW3> X("hw3", "hw3 pass");

class ClassInfo {
  public:
    std::string name;
    int NumOfArgs;
    int NumOfCallSites;
    int NumOfBasicBlocks;
    int NumOfInstructions;
    ClassInfo() {};
};

typedef std::map<std::string, ClassInfo> StringClassInfoMap;

namespace {
  // Hello2 - The second implementation with getAnalysisUsage implemented.
  struct HW3ModulePass : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
    HW3ModulePass() : ModulePass(ID) {}

    bool runOnModule(Module &M) override {
      StringClassInfoMap map;
      // Get the basic info of a Function and store them in a map
      for (auto Fp = M.getFunctionList().begin(), EndFp = M.getFunctionList().end(); Fp != EndFp; Fp++) {
        ClassInfo info;
        info.name = Fp->getName();
        info.NumOfArgs = Fp->arg_size();
        info.NumOfCallSites = 0;
        info.NumOfBasicBlocks = Fp->getBasicBlockList().size();
        info.NumOfInstructions = Fp->getInstructionCount();
        map[Fp->getName()] = info;
      }

      //Interate the instructions of every function
      for (auto Fp = M.getFunctionList().begin(), EndFp = M.getFunctionList().end(); Fp != EndFp; Fp++) {
        for (inst_iterator inst = inst_begin(*Fp), endInst = inst_end(*Fp); inst != endInst; ++inst) {
          Instruction &I = *inst;
          if (auto *callInst = dyn_cast<CallInst>(&I)) {
            // We know we've encountered a call instruction, so we
            // need to determine if it's a call to the function in the target module
            std::string funcName = callInst->getCalledFunction()->getName();
            if (map.find(funcName) != map.end()) {
              map[funcName].NumOfCallSites++;
            }
          }
        }
      }
      
      for (auto const& entry : map) {
        const ClassInfo& info = entry.second;
        if (info.NumOfInstructions == 0) {
          continue;
        }
        errs() << "Name: " << info.name << '\n';
        errs() << "Number of Argments: " << info.NumOfArgs << '\n';
        errs() << "Number of direct call sites in the same LLVM module: " << info.NumOfCallSites << '\n';
        errs() << "Number of Basic Blocks: " << info.NumOfBasicBlocks << '\n';
        errs() << "Number of Insturctions: " << info.NumOfInstructions << '\n';
        errs() << '\n';
      } 

      return false;
    }

    // We don't modify the program, so we preserve all analyses.
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
    }
  };
}

char HW3ModulePass::ID = 0;
static RegisterPass<HW3ModulePass>
Y("hw3ModulePass", "HW3 with Module Pass");