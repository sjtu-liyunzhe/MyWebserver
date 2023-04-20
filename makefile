CXX = g++
CFLAGS = -std=c++11 -O2 -Wall -g
CXXFLAGS = -std=c++11 -O2 -Wall -g

TARGET := MyWebServer
SOURCE := $(wildcard ../*cpp)
OBJS = ./buffer/buffer.cpp \
	   ./http/HTTPrequest.cpp ./http/HTTPresponse.cpp ./http/HTTPconnection.cpp \
       ./timer/timer.cpp \
	   ./epoller/epoller.cpp \
	   ./server/webserver.cpp \
	   ./main.cpp

$(TARGET):$(OBJS)
	$(CXX) $(CXXFLAGS)  $(OBJS) -o ./bin/$(TARGET) -pthread