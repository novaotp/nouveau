build:
	g++ -Isource/headers ./source/*.cpp -o ./bin/nouveau.exe

test:
	g++ -Isource/headers -Itests/extras ./source/utils.cpp ./source/lexer.cpp ./source/parser.cpp ./source/semer.cpp ./tests/extras/catch_amalgamated.cpp ./tests/test_runner.cpp -o ./bin/test_runner.exe
	./bin/test_runner.exe
