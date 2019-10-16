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
      errs() << "Function Name: " <<  F.getName() << "\n";

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

#if 0
namespace {
  // Hello2 - The second implementation with getAnalysisUsage implemented.
  struct Hello2 : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    Hello2() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      ++HelloCounter;
      errs() << "Hello: ";
      errs().write_escaped(F.getName()) << '\n';
      return false;
    }

    // We don't modify the program, so we preserve all analyses.
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
    }
  };
}

char Hello2::ID = 0;
static RegisterPass<Hello2>
Y("hello2", "Hello World Pass (with getAnalysisUsage implemented)");
#endif
