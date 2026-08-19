# linux only
COMPILEPATH := "./build/"

all: build_includes build_main build_final

build_includes:
	@echo "Compiling & assembling includes..."
	@mkdir -p $(COMPILEPATH)
	clang++ -c input.cpp -o $(COMPILEPATH)/input.o

build_main:
	@echo "Compiling & assembling application..."
	@mkdir -p $(COMPILEPATH)
	clang++ -c main.cpp -o $(COMPILEPATH)/main.o

build_final:
	@echo "Linking modules to application..."
	clang++ $(COMPILEPATH)/main.o $(COMPILEPATH)/input.o -o $(COMPILEPATH)/main

clean:
	@echo "Removing compiled files..."
	rm -r $(COMPILEPATH)
