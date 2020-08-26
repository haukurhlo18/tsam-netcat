all:
	g++ -std=c++11 src/client.cpp -o bin/client
	g++ -std=c++11 src/server.cpp -o bin/server
