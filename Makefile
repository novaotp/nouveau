build:
	g++ -Isource/headers ./source/*.cpp -o ./bin/main.exe

run:
	./bin/main.exe

test:
	g++ -Isource/headers -Itests/extras ./source/file.cpp ./source/lexer.cpp ./tests/extras/catch_amalgamated.cpp ./tests/test_runner.cpp -o ./bin/test_runner.exe
	./bin/test_runner.exe
