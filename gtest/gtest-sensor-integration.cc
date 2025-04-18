#include <gtest/gtest.h>
#include "uds.h"
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "../frame-format.h"
#include "../cmd-handler.h"
#include "../command.h"
#include "../process2.h"
#include "../uds-socket-path.h"
#include <sys/un.h>


// 서버 쓰레드를 gtest에서 실행
class UDSServerTestEnv : public ::testing::Environment {
public:
    pthread_t server_tid;
    pthread_t processing_tid;
    SHARED_SENSOR_DATA stSharedSensorData;
    int iUdsClientSock;

    void SetUp() override {
        // UDS 클라이언트 생성 및 연결
        iUdsClientSock = socket(AF_UNIX, SOCK_STREAM, 0);
        ASSERT_GT(iUdsClientSock, 0);

        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, SENSOR_PROCESSING_SOCK, sizeof(addr.sun_path) - 1);

        // 뮤텍스 및 조건변수 초기화
        pthread_mutex_init(&stSharedSensorData.mutex, nullptr);
        pthread_cond_init(&stSharedSensorData.cond, nullptr);

        pthread_create(&server_tid, nullptr, sensorCollectorThread, &stSharedSensorData);
        pthread_create(&processing_tid, nullptr, sensorProcessingThread, &stSharedSensorData);

        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // 서버 준비 대기
        ASSERT_EQ(connect(iUdsClientSock, (struct sockaddr*)&addr, sizeof(addr)), 0);
    }

    int receiveResult(char* buffer, int maxSize) {
        return recv(iUdsClientSock, buffer, maxSize, 0);
    }

    void TearDown() override {
        // 서버 무한 루프이므로 실제 종료 처리는 테스트 용도로는 생략
        // 실제 시스템에서는 signal or flag로 종료 처리
    }
};
static UDSServerTestEnv* g_pTestEnv = nullptr;

char* getName(char chMsgId){
    if(chMsgId == EXTERN1_ID)
        return "EXTERN1";
    else if(chMsgId == EXTERN2_ID)
        return "EXTERN2";
    else if(chMsgId == EXTERN3_ID)
        return "EXTERN3";
    else if(chMsgId == EXTERN4_ID)
        return "EXTERN4";
}

// 클라이언트 동작
void sendDataRepeated(char chMsgId, const char* pchData, int iSize, int interval_ms) {
    int sock = createUdsClientSocket(SENSOR_COLLECT_SOCK);
    if (sock < 0) 
        perror(getName(chMsgId));

    ASSERT_GE(sock, 0) << getName(chMsgId) << " 소켓 연결 실패";
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    for (int i = 0; i < 10; ++i) {
        std::cout << "[" << getName(chMsgId) << "] 전송 " << i + 1 << "/10" << std::endl;
        udsSendMsg(sock, (char*)pchData, iSize);
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
    }

    close(sock);
}

TEST(UDSSocketMultiClientTest, SimultaneousClientDataSend10Times) {
    usleep(100);
    int iFrameSize = sizeof(FRAME_HEADER)+sizeof(FRAME_TAIL);  
    char achExtern1Data[sizeof(FRAME_HEADER)+sizeof(FRAME_TAIL)+sizeof(EXTERNAL1_DATA)];
    char achExtern2Data[sizeof(FRAME_HEADER)+sizeof(FRAME_TAIL)+sizeof(EXTERNAL2_DATA)];
    char achExtern3Data[sizeof(FRAME_HEADER)+sizeof(FRAME_TAIL)+sizeof(EXTERNAL3_DATA)];
    char achExtern4Data[sizeof(FRAME_HEADER)+sizeof(FRAME_TAIL)+sizeof(EXTERNAL4_DATA)];

    MSG_ID stMsgExtern1Id = {EXTERN1_ID, UDS2_ID};
    makeRecvPacket(UDS2_EXTERN1_DATA, &stMsgExtern1Id, achExtern1Data);
    EXTERNAL1_DATA *pstExtern1 = (EXTERNAL1_DATA *)(achExtern1Data + sizeof(FRAME_HEADER));
    pstExtern1->dLatitude = 1.1;
    pstExtern1->dLongitude = 2.1;
    pstExtern1->dAltitude = 3.1;

    MSG_ID stMsgExtern2Id = {EXTERN2_ID, UDS2_ID};
    makeRecvPacket(UDS2_EXTERN2_DATA, &stMsgExtern2Id, achExtern2Data);
    EXTERNAL2_DATA *pstExtern2 = (EXTERNAL2_DATA *)(achExtern2Data + sizeof(FRAME_HEADER));
    pstExtern2->dRoll = 4.1;
    pstExtern2->dPitch = 5.1;
    pstExtern2->dYaw = 6.1;

    MSG_ID stMsgExtern3Id = {EXTERN3_ID, UDS2_ID};
    makeRecvPacket(UDS2_EXTERN3_DATA, &stMsgExtern3Id, achExtern3Data);
    EXTERNAL3_DATA *pstExtern3 = (EXTERNAL3_DATA *)(achExtern3Data + sizeof(FRAME_HEADER));
    pstExtern3->dAz = 7.1;
    pstExtern3->dEl = 8.1;

    MSG_ID stMsgExtern4Id = {EXTERN4_ID, UDS2_ID};
    makeRecvPacket(UDS2_EXTERN4_DATA, &stMsgExtern4Id, achExtern4Data);
    EXTERNAL4_DATA *pstExtern4 = (EXTERNAL4_DATA *)(achExtern4Data + sizeof(FRAME_HEADER));    
    pstExtern4->dLatitude = 9.1;
    pstExtern4->dLongitude = 10.1;
    pstExtern4->dAltitude = 11.1;

    std::thread t1(sendDataRepeated, EXTERN1_ID,    achExtern1Data, sizeof(EXTERNAL1_DATA)+iFrameSize,  20);
    std::thread t2(sendDataRepeated, EXTERN2_ID,    achExtern2Data, sizeof(EXTERNAL2_DATA)+iFrameSize,  20);
    std::thread t3(sendDataRepeated, EXTERN3_ID,    achExtern3Data, sizeof(EXTERNAL3_DATA)+iFrameSize,  21);
    std::thread t4(sendDataRepeated, EXTERN4_ID,    achExtern4Data, sizeof(EXTERNAL4_DATA)+iFrameSize,  22);
    
    // 결과 수신 및 검증 (테스트 환경으로부터)
    for (int i = 0; i < 10; ++i) {
        char resultBuf[128] = {0};
        int len = g_pTestEnv->receiveResult(resultBuf, sizeof(resultBuf));

        // 결과 구조체 해석
        FRAME_HEADER* pHeader = (FRAME_HEADER*)resultBuf;
        PROCESSING_DATA* pResult = (PROCESSING_DATA*)(resultBuf + sizeof(FRAME_HEADER));
        EXPECT_NEAR(pResult->dAz, 67.1, 0.1);
        EXPECT_NEAR(pResult->dEl, 67.1, 0.1);
    }
    
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    SUCCEED();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    g_pTestEnv = new UDSServerTestEnv();
    ::testing::AddGlobalTestEnvironment(g_pTestEnv);

    return RUN_ALL_TESTS();
}
