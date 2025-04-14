PROCESS1_APP 			= process1
GTEST_TARGET 			= gtest-recv-thread

SRC_DIR = .
# 구글테스트 관련 설정
GTEST_DIR = /home/pcw1029/googletest
GTEST_INCLUDE_DIR = $(GTEST_DIR)/googletest/include
GTEST_LIB_DIR = $(GTEST_DIR)/build/lib
MY_GTEST_DIR = gtest

# 데스크탑용 설정
DESKTOP_LDFLAGS = LDFLAGS + -L../libtcpsock

# 테스트에서 제외할 .c 파일들 (main 함수 포함)
EXCLUDE_SRCS = $(SRC_DIR)/process1.c $(SRC_DIR)/send-thread.c 

# 테스트 대상 .c 파일 목록 (process1.c 제외)
FOR_GTEST_SRCS = $(filter-out $(EXCLUDE_SRCS), $(wildcard $(SRC_DIR)/*.c))

# 각 .c 파일을 대응하는 *_gtest.o 객체로 변환
FOR_GTEST_OBJS = $(patsubst %.c, %_gtest.o, $(FOR_GTEST_SRCS))

# GTest 전용 테스트 소스
MY_GTEST_SRCS = $(MY_GTEST_DIR)/gtest-recv-thread.cc
MY_GTEST_OBJS = $(patsubst %.cc, %.o, $(MY_GTEST_SRCS))

PROCESS1_APP_SRC	= $(wildcard $(SRC_DIR)/*.c)
PROCESS1_APP_OBJS	= $(patsubst %.c, %.o, $(PROCESS1_APP_SRC))


# 컴파일러 (Yocto에서 CC, CXX 전달 받음)
CC ?= gcc
CXX ?= g++
GTEST_CFLAGS = -Wall -g -I$(INCLUDE_DIR) -I$(GTEST_INCLUDE_DIR) -std=c++11
GTEST_LDFLAGS = -L$(GTEST_LIB_DIR) -lgtest -lgtest_main -lpthread


all: $(PROCESS1_APP)

$(PROCESS1_APP): $(PROCESS1_APP_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(PROCESS1_APP_OBJS) $(LDLIBS) -I./  -lpthread -ltcpsock_desktop -luds


$(OBJS_DIR)/%.o: %.c
	$(CC) -c  $(PROCESS1_APP_SRC) -o $@ 

# 구글테스트 빌드 및 실행
gtest: $(MY_GTEST_OBJS) $(FOR_GTEST_OBJS)
	$(CXX) $(GTEST_CFLAGS) -o $(GTEST_TARGET) $(MY_GTEST_OBJS) $(FOR_GTEST_OBJS) $(GTEST_LDFLAGS)

# .c → .gtest.o 규칙
%_gtest.o: %.c
	$(CXX) $(GTEST_CFLAGS) -c $< -o $@


clean:
	-rm -f $(PROCESS1_APP) $(PROCESS1_APP_OBJS) $(MY_GTEST_OBJS) $(FOR_GTEST_OBJS) $(GTEST_TARGET)
	