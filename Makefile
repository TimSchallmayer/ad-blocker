all:
		llvm-rc resources.rc
		clang-cl main.cpp resources.res /Fe:main.exe 