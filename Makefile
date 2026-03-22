# Compiler and flags
CXX = g++
INC_DIR = -I./httplib -I./common -I./auth
CXXFLAGS = -std=c++17 $(INC_DIR) -Wall -Wextra -pthread
LDFLAGS += -pthread -lssl -lcrypto -lmysqlclient

# Source and output
SRCS = wstory.cpp receivermain.cpp common/Logger.cpp \
       auth/SignInSignUp.cpp auth/MySqlUserStore.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = wstory

# Default rule
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
