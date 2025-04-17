#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

#include "../frame-format.h"
#include "../process1.h"
#include "../cmd-handler.h"
#include "../command.h"

class TcpTestManager : public ::testing::Test {
protected:
    CLIENT_INFO info;
    int sv[2]; // UNIX 도메인 소켓 쌍
    pthread_t recv_tid, send_tid;

    void SetUp() override {
        ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
        memset(&info, 0, sizeof(info));
        info.iClientSock = sv[1];

        pthread_mutex_init(&info.stSharedData.mutex, NULL);
        pthread_cond_init(&info.stSharedData.cond, NULL);
        pthread_mutex_init(&info.exitFlagMutex, NULL);

        pthread_create(&recv_tid, nullptr, receiveThread, &info);
        pthread_create(&send_tid, nullptr, sendThread, &info);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 서버 준비 대기
    }

    void TearDown() override {
        shutdown(sv[0], SHUT_RDWR);
        close(sv[0]);

        pthread_join(recv_tid, nullptr);
        pthread_join(send_tid, nullptr);

        pthread_mutex_destroy(&info.stSharedData.mutex);
        pthread_cond_destroy(&info.stSharedData.cond);
        pthread_mutex_destroy(&info.exitFlagMutex);
    }

    void sendCommand(const char* data, int size) {
        send(sv[0], data, size, 0);
    }

    int receiveResponse(char* buffer, int maxSize) {
        return recv(sv[0], buffer, maxSize, 0);
    }
};


TEST_F(TcpTestManager, CMD_KEEP_ALIVE_Test) {
    MSG_ID stMsgId;
    RCV_KEEP_ALIVE* pstRcvKeepAlive;
    int iSendSize = sizeof(FRAME_HEADER) + sizeof(RCV_KEEP_ALIVE) + sizeof(FRAME_TAIL);
    int iRecvSize = sizeof(FRAME_HEADER) + sizeof(SND_KEEP_ALIVE) + sizeof(FRAME_TAIL);
    char achPacket[128] = {0};

    stMsgId.chSrcId = 0x01;
    stMsgId.chDstId = 0x02;
    pstRcvKeepAlive = (RCV_KEEP_ALIVE *)(achPacket + sizeof(FRAME_HEADER));
    pstRcvKeepAlive->chTmp = 0x01;

    makeRecvPacket(CMD_KEEP_ALIVE, &stMsgId, achPacket);
    sendCommand(achPacket, iSendSize);

    char achResponse[128] = {0};
    int len = receiveResponse(achResponse, sizeof(achResponse));
    fprintf(stderr,"recv size is %d\n", len);

    EXPECT_EQ(len, iRecvSize);

    SND_KEEP_ALIVE* pResp = (SND_KEEP_ALIVE*)(achResponse + sizeof(FRAME_HEADER));
    EXPECT_EQ(pResp->chResult, 0x01);
}

TEST_F(TcpTestManager, CMD_IBIT_Test) {
    MSG_ID stMsgId;
    RCV_IBIT* pstRcvIBit;
    int iSendSize = sizeof(FRAME_HEADER) + sizeof(RCV_IBIT) + sizeof(FRAME_TAIL);
    int iRecvSize = sizeof(FRAME_HEADER) + sizeof(SND_IBIT) + sizeof(FRAME_TAIL);
    char achPacket[128] = {0};

    stMsgId.chSrcId = 0x01;
    stMsgId.chDstId = 0x02;
    pstRcvIBit = (RCV_IBIT *)(achPacket + sizeof(FRAME_HEADER));
    pstRcvIBit->chIbit = 0x01;

    makeRecvPacket(CMD_IBIT, &stMsgId, achPacket);
    sendCommand(achPacket, iSendSize);

    char achResponse[128] = {0};
    int len = receiveResponse(achResponse, sizeof(achResponse));
    fprintf(stderr,"recv size is %d\n", len);

    EXPECT_EQ(len, iRecvSize);

    SND_IBIT* pResp = (SND_IBIT*)(achResponse + sizeof(FRAME_HEADER));
    EXPECT_EQ(pResp->chBitTotResult, 0x01);
    EXPECT_EQ(pResp->chPositionResult, 0x01);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
