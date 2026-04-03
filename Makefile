# Compiler and flags
CXX = g++
INC_DIR = -I./httplib -I./common -I./auth -I./compare -I../dmp
CXXFLAGS = -std=c++17 $(INC_DIR) -Wall -Wextra -pthread
LDFLAGS += -pthread -lssl -lcrypto -lmysqlclient

# dmp static library
DMP_LIB = ../dmp/libdiff_match_patch.a

# Source and output
SRCS = wstory.cpp receivermain.cpp common/Logger.cpp \
       auth/SignInSignUp.cpp auth/MySqlUserStore.cpp \
       compare/CompareHandler.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = wstory

# Default rule — build dmp static lib first, then the server
all: $(DMP_LIB) $(TARGET)

# Build dmp static library
$(DMP_LIB):
	$(MAKE) -C ../dmp static

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(DMP_LIB) $(LDFLAGS)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule (does not clean dmp — run make clean inside dmp/ separately)
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
