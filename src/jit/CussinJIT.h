#ifndef CUSSINJIT_H
#define CUSSINJIT_H

#include <iostream>
#include <memory>

#include <llvm/ADT/StringRef.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>


namespace llvm {
    namespace orc {

		class CussinJIT {
		private:
		    std::unique_ptr<ExecutionSession> ES;

		    DataLayout DL;
		    MangleAndInterner Mangle;

		    RTDyldObjectLinkingLayer ObjectLayer;
		    IRCompileLayer CompileLayer;

		    JITDylib& MainJD;

		public:
		    CussinJIT(std::unique_ptr<ExecutionSession> ES, JITTargetMachineBuilder JTMB, DataLayout DL)
		        : ES(std::move(ES)), DL(std::move(DL)), Mangle(*this->ES, this->DL),
		        ObjectLayer(*this->ES,
		            []() { return std::make_unique<SectionMemoryManager>(); }),
		        CompileLayer(*this->ES, ObjectLayer,
		            std::make_unique<ConcurrentIRCompiler>(std::move(JTMB))),
		        MainJD(this->ES->createBareJITDylib("<main>")) {
		        MainJD.addGenerator(
		            cantFail(DynamicLibrarySearchGenerator::GetForCurrentProcess(
		                DL.getGlobalPrefix())));
		        if (JTMB.getTargetTriple().isOSBinFormatCOFF()) {
		            ObjectLayer.setOverrideObjectFlagsWithResponsibilityFlags(true);
		            ObjectLayer.setAutoClaimResponsibilityForObjectSymbols(true);
		        }
		    }

			~CussinJIT() = default;

			/*
			~CussinJIT() {
				//if (auto Err = ES->endSession())
				   // ES->reportError(std::move(Err));
			};
			*/
		    static Expected<std::unique_ptr<CussinJIT>> Create() {
		        auto EPC = SelfExecutorProcessControl::Create();
		        if (!EPC)
		            return EPC.takeError();

		        auto ES = std::make_unique<ExecutionSession>(std::move(*EPC));

		        JITTargetMachineBuilder JTMB(
		            ES->getExecutorProcessControl().getTargetTriple());

		        auto DL = JTMB.getDefaultDataLayoutForTarget();
		        if (!DL)
		            return DL.takeError();

		        return std::make_unique<CussinJIT>(std::move(ES), std::move(JTMB),
		            std::move(*DL));
		    }

		    const DataLayout& getDataLayout() const { return DL; }

		    JITDylib& getMainJITDylib() { return MainJD; }

		    Error addModule(ThreadSafeModule TSM, ResourceTrackerSP RT = nullptr) {
		        if (!RT)
		            RT = MainJD.getDefaultResourceTracker();
		        return CompileLayer.add(RT, std::move(TSM));
		    }

		    Expected<JITEvaluatedSymbol> lookup(StringRef Name) {
		        return ES->lookup({ &MainJD }, Mangle(Name.str()));
		    }
		};
	} 
}


#endif