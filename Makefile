PROCESS1_APP 				= process1
PROCESS2_APP 				= process2
GTEST_TCP_RECV_SEND			= gtest-tcp-recv-send
GTEST_SENSOR_INTEGRATION	= gtest-sensor-integration
GTEST_CONTROLL_PIPELINE		= gtest-controll-pipeline

SRC_DIR = .
# 구글테스트 관련 설정
GTEST_DIR = /home/pcw1029/googletest
GTEST_INCLUDE_DIR = $(GTEST_DIR)/googletest/include
GTEST_LIB_DIR = $(GTEST_DIR)/build/lib
MY_GTEST_DIR = gtest


# GTest 전용 테스트 소스
GTEST_TCP_RECV_SEND_SRCS 			= $(MY_GTEST_DIR)/gtest-tcp-recv-send.cc
GTEST_SENSOR_INTEGRATION_SRCS 		= $(MY_GTEST_DIR)/gtest-sensor-integration.cc
GTEST_CONTROLL_PIPELINE_SRCS 		= $(MY_GTEST_DIR)/gtest-controll-pipeline.cc

# 테스트에서 제외할 .c 파일들 (main 함수 포함)
EXCLUDE_SRCS 						= $(SRC_DIR)/process1.c $(SRC_DIR)/process2.c $(SRC_DIR)/process3.c
EXCLUDE_TCP_RECV_SEND 				= $(GTEST_SENSOR_INTEGRATION_SRCS) $(GTEST_CONTROLL_PIPELINE_SRCS) $(SRC_DIR)/sensor-processing.c $(SRC_DIR)/sensor-collector.c $(SRC_DIR)/recv-command-data.c $(SRC_DIR)/recv-processing-data.c
EXCLUDE_SENSOR_INTEGRATION			= $(GTEST_TCP_RECV_SEND_SRCS) $(GTEST_CONTROLL_PIPELINE_SRCS) $(SRC_DIR)/send-thread.c $(SRC_DIR)/recv-thread.c
EXCLUDE_CONTROLL_PIPELINE			= $(GTEST_TCP_RECV_SEND_SRCS) $(GTEST_SENSOR_INTEGRATION_SRCS) $(SRC_DIR)/send-thread.c $(SRC_DIR)/recv-thread.c

# 테스트 대상 .c 파일 목록 (process1.c 제외)
FOR_GTEST_TCP_RECV_SEND_SRCS 		= $(filter-out $(EXCLUDE_SRCS) $(EXCLUDE_TCP_RECV_SEND), $(wildcard $(SRC_DIR)/*.c))
FOR_GTEST_SENSOR_INTEGRATION_SRCS	= $(filter-out $(EXCLUDE_SRCS) $(EXCLUDE_SENSOR_INTEGRATION), $(wildcard $(SRC_DIR)/*.c))
FOR_GTEST_CONTROLL_PIPELINE_SRCS	= $(filter-out $(EXCLUDE_SRCS) $(EXCLUDE_CONTROLL_PIPELINE), $(wildcard $(SRC_DIR)/*.c))

# 각 .c 파일을 대응하는 *_gtest.o 객체로 변환
FOR_GTEST_TCP_RECV_SEND_OBJS 		= $(patsubst %.c, %_gtest.o, $(FOR_GTEST_TCP_RECV_SEND_SRCS))
FOR_GTEST_SENSOR_INTEGRATION_OBJS 	= $(patsubst %.c, %_gtest.o, $(FOR_GTEST_SENSOR_INTEGRATION_SRCS))
FOR_GTEST_CONTROLL_PIPELINE_OBJS 	= $(patsubst %.c, %_gtest.o, $(FOR_GTEST_CONTROLL_PIPELINE_SRCS))

GTEST_TCP_RECV_SEND_OBJS 			= $(patsubst %.cc, %.o, $(GTEST_TCP_RECV_SEND_SRCS))
GTEST_SENSOR_INTEGRATION_OBJS 		= $(patsubst %.cc, %.o, $(GTEST_SENSOR_INTEGRATION_SRCS))
GTEST_CONTROLL_PIPELINE_OBJS 		= $(patsubst %.cc, %.o, $(GTEST_CONTROLL_PIPELINE_SRCS))

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
gtest-tcp: $(GTEST_TCP_RECV_SEND_OBJS) $(FOR_GTEST_TCP_RECV_SEND_OBJS)
	$(CXX) $(GTEST_CFLAGS) -o $(GTEST_TCP_RECV_SEND) $(GTEST_TCP_RECV_SEND_SRCS) $(FOR_GTEST_TCP_RECV_SEND_OBJS) $(GTEST_LDFLAGS) -ltcpsock_desktop

gtest-sensor: $(GTEST_SENSOR_INTEGRATION_OBJS) $(FOR_GTEST_SENSOR_INTEGRATION_OBJS)
	$(CXX) $(GTEST_CFLAGS) -g -o $(GTEST_SENSOR_INTEGRATION) $(GTEST_SENSOR_INTEGRATION_SRCS) $(FOR_GTEST_SENSOR_INTEGRATION_OBJS) $(GTEST_LDFLAGS) -luds $(LDLIBS) -I./

gtest-controll: $(GTEST_CONTROLL_PIPELINE_OBJS) $(FOR_GTEST_CONTROLL_PIPELINE_OBJS)
	$(CXX) $(GTEST_CFLAGS) -g -o $(GTEST_CONTROLL_PIPELINE) $(GTEST_CONTROLL_PIPELINE_SRCS) $(FOR_GTEST_CONTROLL_PIPELINE_OBJS) $(GTEST_LDFLAGS) -luds $(LDLIBS) -I./

# .c → .gtest.o 규칙
%_gtest.o: %.c
	$(CXX) $(GTEST_CFLAGS) -c $< -o $@


clean:
	-rm -f $(PROCESS1_APP) $(PROCESS1_APP_OBJS) \
		$(GTEST_TCP_RECV_SEND) $(GTEST_TCP_RECV_SEND_OBJS) $(FOR_GTEST_TCP_RECV_SEND_OBJS) \
		$(GTEST_SENSOR_INTEGRATION) $(GTEST_SENSOR_INTEGRATION_OBJS) $(FOR_GTEST_SENSOR_INTEGRATION_OBJS) \
		$(GTEST_CONTROLL_PIPELINE) $(GTEST_CONTROLL_PIPELINE_OBJS) $(FOR_GTEST_CONTROLL_PIPELINE_OBJS) 
	 	   
	