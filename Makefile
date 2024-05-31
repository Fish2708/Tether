run: build
	@echo "---------BEGIN_OUTPUT---------"
	@cd build && ./main
	@echo "----------END_OUTPUT----------"

build:
	g++ src/main.cpp -o main -Iinclude -Llib -llua54 -lraylib
	@mv main build/
