all:
		llvm-rc resources.rc
		clang-cl main.cpp functions.cpp resources.res /Fe:main.exe 