COMPILEPATH := "./build/"

all: build_input build_main build_final

build_input:
	@echo "Compiling & assembling source..."
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
