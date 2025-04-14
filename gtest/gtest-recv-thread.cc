#include <gtest/gtest.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include "../tcp-frame-format.h"
#include "../process1.h"
#include "../cmd-handler.h"
#include "../command.h"

// ---------- 테스트용 Mock 함수 선언 ----------
extern "C" {
    int recvMsgTimeout(int iSock, void *pvBuffer, size_t iLength, int iTimeoutMsec) {
        // 소켓에서 바로 읽기 (간단한 구현)
        fd_set fds;
        struct timeval tv;
        FD_ZERO(&fds);
        FD_SET(iSock, &fds);

        tv.tv_sec = 0;
        tv.tv_usec = iTimeoutMsec * 1000;
        int ret = select(iSock + 1, &fds, NULL, NULL, &tv);
        if (ret < 0) {
            fprintf(stderr,"### Select Error %d ###\n", ret);
            return ret;
        } else if (ret == 0) {
            fprintf(stderr, "Timeout: no data received within %d seconds.\n", iTimeoutMsec);
            return -1;  // timeout
        }
        ssize_t received = recv(iSock, pvBuffer, iLength, 0);
        if (received < 0) {
            sleep(1);
            perror("recv failed");
            return -1;
        }
        return received; // timeout or error
        
    }

    void handleClientDisconnection(int sock) {
        // 테스트용 빈 함수
    }
}

class ReceiveThreadTest : public ::testing::Test 
{
    protected:
        CLIENT_INFO info;
        int sv[2];
        pthread_t tid;
    
        void SetUp() override {
            ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
            memset(&info, 0, sizeof(info));
            info.iClientSock = sv[1];
            pthread_mutex_init(&info.stSharedData.mutex, NULL);
            pthread_cond_init(&info.stSharedData.cond, NULL);
            pthread_mutex_init(&info.exitFlagMutex, NULL);
            pthread_create(&tid, NULL, receiveThread, &info);
            usleep(100 * 1000);
        }
    
        void TearDown() override {
            close(sv[0]);
            pthread_join(tid, NULL); // 안전하게 쓰레드 종료            
            close(sv[1]);
        }
    
        void sendPacket(uint16_t cmd, const void* pData, size_t dataSize) {
            char chMsgId[2] = {0x01, 0x02};
            int iSendSize = dataSize + sizeof(FRAME_HEADER) + sizeof(FRAME_TAIL);
            char* buffer = (char*)malloc(iSendSize);
            memset(buffer, 0, iSendSize);
            memcpy(buffer + sizeof(FRAME_HEADER), pData, dataSize);
            makeRecvPacket(cmd, chMsgId, buffer);
            
            fprintf(stderr,"\nGTEST SEND RAW DATA\n");
            printHex(buffer, iSendSize);
            
            write(sv[0], buffer, iSendSize);
            free(buffer);
        }
    
        int waitForData() {
            pthread_mutex_lock(&info.stSharedData.mutex);
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 1;
            int result = pthread_cond_timedwait(&info.stSharedData.cond, &info.stSharedData.mutex, &ts);
            pthread_mutex_unlock(&info.stSharedData.mutex);
            return result;
        }
    
        void verifyFrameHeader(uint16_t expectedCmd, int expectedLength) {
            FRAME_HEADER* header = (FRAME_HEADER*)info.stSharedData.chData;
            FRAME_TAIL* tail = (FRAME_TAIL*)(info.stSharedData.chData + sizeof(FRAME_HEADER) + header->iLength);
    
            EXPECT_EQ((uint16_t)header->nStx, PACKET_STX);
            EXPECT_EQ(header->iLength, expectedLength);
            EXPECT_EQ(header->chSrcId, PACKET_DST_ID);
            EXPECT_EQ(header->chDstId, PACKET_SRC_ID);
            EXPECT_EQ(header->chSubModule, 0);
            EXPECT_EQ(header->nCmd, expectedCmd);
            EXPECT_EQ((uint16_t)tail->nEtx, PACKET_ETX);
    
            fprintf(stderr,"\nGTEST RECV RAW DATA\n");
            printHex(info.stSharedData.chData, sizeof(FRAME_HEADER) + header->iLength + sizeof(FRAME_TAIL));
        }
    
        void printHex(const void* data, size_t len) {
            const unsigned char* p = (const unsigned char*)data;
            for (size_t i = 0; i < len; ++i) {
                fprintf(stderr, "%02x ", p[i]);
            }
            fprintf(stderr, "\n");
        }
    };

TEST_F(ReceiveThreadTest, PacketReceiveAndProcessWithKeepAlive) {
    RCV_KEEP_ALIVE recv = { .chTmp = 0x01 };
    sendPacket(CMD_KEEP_ALIVE, &recv, sizeof(recv));
    EXPECT_EQ(waitForData(), 0);
    EXPECT_TRUE(info.stSharedData.hasData);
    verifyFrameHeader(CMD_KEEP_ALIVE, getSendCmdSize(CMD_KEEP_ALIVE));

    SND_KEEP_ALIVE* snd = (SND_KEEP_ALIVE*)(info.stSharedData.chData + sizeof(FRAME_HEADER));
    EXPECT_EQ(snd->chResult, 0x01);
}

TEST_F(ReceiveThreadTest, PacketReceiveAndProcessWithIBit) {
    RCV_IBIT recv = { .chIbit = 0x01 };
    sendPacket(CMD_IBIT, &recv, sizeof(recv));
    EXPECT_EQ(waitForData(), 0);
    EXPECT_TRUE(info.stSharedData.hasData);
    verifyFrameHeader(CMD_IBIT, getSendCmdSize(CMD_IBIT));

    SND_IBIT* snd = (SND_IBIT*)(info.stSharedData.chData + sizeof(FRAME_HEADER));
    EXPECT_EQ(snd->chBitTotResult, 0x01);
    EXPECT_EQ(snd->chPositionResult, 0x01);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
