.PHONY: clean

CC = clang
CXX = clang++

LLVM_MODULES = core jit mcjit interpreter nativecodegen bitreader bitwriter irreader ipo linker selectiondag asmparser instrumentation objcarcopts option

CPPFLAGS = -std=c++11 `llvm-config --cppflags $(LLVM_MODULES)`
LDFLAGS = `llvm-config --ldflags $(LLVM_MODULES) --system-libs`
LIBS = -ldl -lpthread  -lclangFrontendTool -lclangFrontend -lclangDriver -lclangSerialization -lclangCodeGen -lclangParse -lclangSema -lclangStaticAnalyzerFrontend -lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore -lclangAnalysis -lclangARCMigrate -lclangRewriteFrontend -lclangRewrite -lclangEdit -lclangAST -lclangLex -lclangBasic `llvm-config --libs $(LLVM_MODULES)`

test: all
	./clang-interpreter stack.c

all: clang-interpreter

%.o: %.cc
	$(CXX) -c $< $(CPPFLAGS)

clang-interpreter: main.o
	$(CXX) -o clang-interpreter main.o $(LIBS) $(LDFLAGS)

clean:
	rm -rf *.o *~

