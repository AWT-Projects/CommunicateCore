PROCESS1_APP 			= process1
PROCESS2_APP 			= process2
GTEST_TARGET 			= gtest-recv-thread

SRC_DIR = .
# 구글테스트 관련 설정
GTEST_DIR = /home/pcw1029/googletest
GTEST_INCLUDE_DIR = $(GTEST_DIR)/googletest/include
GTEST_LIB_DIR = $(GTEST_DIR)/build/lib
MY_GTEST_DIR = gtest


# 테스트에서 제외할 .c 파일들 (main 함수 포함)
EXCLUDE_SRCS = $(SRC_DIR)/process1.c $(SRC_DIR)/process2.c
EXCLUDE_TCP_SRCS = $(SRC_DIR)/send-thread.c $(SRC_DIR)/uds-sensor-data-recv.c $(MY_GTEST_DIR)/gtest-uds-recv-thread.cc
EXCLUDE_UDS_SRCS = $(SRC_DIR)/send-thread.c $(SRC_DIR)/recv-thread.c $(MY_GTEST_DIR)/gtest-recv-thread.cc

# 테스트 대상 .c 파일 목록 (process1.c 제외)
FOR_GTEST_TCP_SRCS = $(filter-out $(EXCLUDE_SRCS) $(EXCLUDE_TCP_SRCS), $(wildcard $(SRC_DIR)/*.c))
FOR_GTEST_UDS_SRCS = $(filter-out $(EXCLUDE_SRCS) $(EXCLUDE_UDS_SRCS), $(wildcard $(SRC_DIR)/*.c))

# 각 .c 파일을 대응하는 *_gtest.o 객체로 변환
FOR_GTEST_TCP_OBJS = $(patsubst %.c, %_gtest.o, $(FOR_GTEST_TCP_SRCS))
FOR_GTEST_UDS_OBJS = $(patsubst %.c, %_gtest.o, $(FOR_GTEST_UDS_SRCS))

# GTest 전용 테스트 소스
MY_TCP_GTEST_SRCS = $(MY_GTEST_DIR)/gtest-recv-thread.cc
MY_UDS_GTEST_SRCS = $(MY_GTEST_DIR)/gtest-uds-recv-thread.cc

MY_TCP_GTEST_OBJS = $(patsubst %.cc, %.o, $(MY_TCP_GTEST_SRCS))
MY_UDS_GTEST_OBJS = $(patsubst %.cc, %.o, $(MY_UDS_GTEST_SRCS))

PROCESS1_APP_SRC	= $(wildcard $(SRC_DIR)/*.c)
PROCESS1_APP_OBJS	= $(patsubst %.c, %.o, $(PROCESS1_APP_SRC))


# 컴파일러 (Yocto에서 CC, CXX 전달 받음)
CC ?= gcc
CXX ?= g++
GTEST_CFLAGS = -Wall -g -I$(INCLUDE_DIR) -I$(GTEST_INCLUDE_DIR) -std=c++11
GTEST_LDFLAGS = -L$(GTEST_LIB_DIR) -lgtest -lgtest_main -lpthread


all: $(PROCESS1_APP) $(PROCESS2_APP)

$(PROCESS1_APP): $(PROCESS1_APP_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(PROCESS1_APP_OBJS) $(LDLIBS) -I./  -lpthread -ltcpsock_desktop

$(PROCESS2_APP): $(PROCESS2_APP_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(PROCESS2_APP_OBJS) $(LDLIBS) -I./  -lpthread -luds

$(OBJS_DIR)/%.o: %.c
	$(CC) -c  $(PROCESS1_APP_SRC) -o $@ 

# 구글테스트 빌드 및 실행
tcp-gtest: $(MY_TCP_GTEST_OBJS) $(FOR_GTEST_TCP_OBJS)
	$(CXX) $(GTEST_CFLAGS) -o $@ $(MY_TCP_GTEST_SRCS) $(FOR_GTEST_TCP_OBJS) $(GTEST_LDFLAGS)

uds-gtest: $(MY_UDS_GTEST_OBJS) $(FOR_GTEST_UDS_OBJS)
	$(CXX) $(GTEST_CFLAGS) -o $@ $(MY_UDS_GTEST_SRCS) $(FOR_GTEST_UDS_OBJS) $(GTEST_LDFLAGS) -luds $(LDLIBS) -I./

# .c → .gtest.o 규칙
%_gtest.o: %.c
	$(CXX) $(GTEST_CFLAGS) -c $< -o $@


clean:
	-rm -f $(PROCESS1_APP) $(PROCESS1_APP_OBJS) $(MY_TCP_GTEST_OBJS) $(MY_UDS_GTEST_OBJS) $(FOR_GTEST_TCP_OBJS) $(FOR_GTEST_UDS_OBJS) $(GTEST_TARGET) tcp-gtest uds-gtest
	