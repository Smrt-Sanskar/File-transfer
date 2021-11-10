Readme
Instructions for compiling the code

There are three files
- server.cpp
- client1.cpp
- client2.cpp

Compile them using :
- server.cpp : g++ server.cpp -o server.out -pthread -std=c++11
- client1.cpp : g++ client1.cpp
- client2.cpp : g++ client2.cpp

Then first start the server : ./server.out
Then start the client1 : ./a.out
Then start the client2 : ./a.out
