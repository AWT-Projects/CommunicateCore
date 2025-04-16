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


// 서버 쓰레드를 gtest에서 실행
class UDSServerTestEnv : public ::testing::Environment {
public:
    pthread_t server_tid;
    pthread_t processing_tid;
    SHARED_SENSOR_DATA stSharedSensorData;

    void SetUp() override {
        // 뮤텍스 및 조건변수 초기화
        pthread_mutex_init(&stSharedSensorData.mutex, nullptr);
        pthread_cond_init(&stSharedSensorData.cond, nullptr);

        pthread_create(&server_tid, nullptr, sensorServerThread, &stSharedSensorData);
        pthread_create(&processing_tid, nullptr, processingSensorDataThread, &stSharedSensorData);
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // 서버 준비 대기
    }

    void TearDown() override {
        // 서버 무한 루프이므로 실제 종료 처리는 테스트 용도로는 생략
        // 실제 시스템에서는 signal or flag로 종료 처리
    }
};

// 클라이언트 동작
void sendDataRepeated(const char* name, const char* pchData, int iSize, int interval_ms) {
    int sock = createUdsClientSocket("/tmp/sensor_recv_data.sock");
    if (sock < 0) 
        perror(name);

    ASSERT_GE(sock, 0) << name << " 소켓 연결 실패";
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    for (int i = 0; i < 10; ++i) {
        std::cout << "[" << name << "] 전송 " << i + 1 << "/10" << std::endl;
        udsSendMsg(sock, (char*)pchData, iSize);
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
    }

    close(sock);
}

TEST(UDSSocketMultiClientTest, SimultaneousClientDataSend10Times) {
    usleep(400);
    int iFrameSize = sizeof(FRAME_HEADER)+sizeof(FRAME_TAIL);  
    char achGpsData[sizeof(FRAME_HEADER)+sizeof(FRAME_TAIL)+sizeof(GPS_DATA)];
    char achImuData[sizeof(FRAME_HEADER)+sizeof(FRAME_TAIL)+sizeof(IMU_DATA)];
    char achSpData[sizeof(FRAME_HEADER)+sizeof(FRAME_TAIL)+sizeof(SP_DATA)];
    char achExternData[sizeof(FRAME_HEADER)+sizeof(FRAME_TAIL)+sizeof(EXTERN_DATA)];
    char achKeyboardData[sizeof(FRAME_HEADER)+sizeof(FRAME_TAIL)+sizeof(KEYBOARD_DATA)];

    char chMsgId[2] = {0x01, 0x02};
    makeRecvPacket(CMD_UDS_GPS_DATA, chMsgId, achGpsData);
    GPS_DATA *pstGps = (GPS_DATA *)(achGpsData + sizeof(FRAME_HEADER));
    pstGps->dLatitude = 1.1;
    pstGps->dLongitude = 2.1;
    pstGps->dAltitude = 3.1;
    makeRecvPacket(CMD_UDS_IMU_DATA, chMsgId, achImuData);
    IMU_DATA *pstImu = (IMU_DATA *)(achImuData + sizeof(FRAME_HEADER));
    pstImu->dRoll = 4.1;
    pstImu->dPitch = 5.1;
    pstImu->dYaw = 6.1;

    makeRecvPacket(CMD_UDS_SP_DATA, chMsgId, achSpData);
    SP_DATA *pstSp = (SP_DATA *)(achSpData + sizeof(FRAME_HEADER));
    pstSp->dAz = 7.1;
    pstSp->dEl = 8.1;
    makeRecvPacket(CMD_UDS_EXTERN_DATA, chMsgId, achExternData);
    EXTERN_DATA *pstExtern = (EXTERN_DATA *)(achExternData + sizeof(FRAME_HEADER));

    makeRecvPacket(CMD_UDS_KEYBOARD_DATA, chMsgId, achKeyboardData);
    KEYBOARD_DATA *pstKeyboard = (KEYBOARD_DATA *)(achKeyboardData + sizeof(FRAME_HEADER));

    std::thread t1(sendDataRepeated, "GPS",         achGpsData,         sizeof(GPS_DATA)+iFrameSize,       20);
    std::thread t2(sendDataRepeated, "IMU",         achImuData,         sizeof(IMU_DATA)+iFrameSize,       20);
    std::thread t3(sendDataRepeated, "SP",          achSpData,          sizeof(SP_DATA)+iFrameSize,        20);
    // std::thread t4(sendDataRepeated, "EXTERN",      achExternData,      sizeof(EXTERN_DATA)+iFrameSize,    20);
    // std::thread t5(sendDataRepeated, "KEYBOARD",    achKeyboardData,    sizeof(KEYBOARD_DATA)+iFrameSize,  100);

    t1.join();
    t2.join();
    t3.join();
    // t4.join();
    // t5.join();

    SUCCEED();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    // 테스트 환경에 서버 추가
    ::testing::AddGlobalTestEnvironment(new UDSServerTestEnv());

    return RUN_ALL_TESTS();
}
