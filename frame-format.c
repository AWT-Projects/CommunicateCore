#include "frame-format.h"
#include "cmd-handler.h"
#include <stdio.h>

/**
 * @brief 패킷의 CRC 값을 계산합니다.
 *
 * @param pchData CRC를 계산할 데이터 버퍼
 * @param iSize 데이터의 총 크기
 * @return 계산된 CRC 값 (1바이트)
 */
char calculatePacketCrc(const char* pchData, int iSize)
{
	char chCrc=0x00;
	int i;
	for(i=CRC_START_POINT; i < iSize-EXCLUDING_CRC_POINT; i++){
		chCrc = chCrc + pchData[i];
	}
	if((chCrc & 0x000000FF)==0xFF)
		chCrc = 0x00;
	return chCrc;
}

/**
 * @brief 패킷의 CRC를 확인하여 유효성을 검사합니다.
 *
 * @param pchData 검사할 데이터 버퍼
 * @param iSize 데이터의 총 크기
 * @return CRC 일치 시 0, 불일치 시 -1 반환
 */
int checkPacketCrc(const char* pchData, int iSize)
{
	unsigned char uchCrc=calculatePacketCrc(pchData, iSize);

	if((uchCrc & 0x000000FF) == (pchData[iSize-EXCLUDING_CRC_POINT] & 0x000000FF))
		return 0;

	return -1;
}

int getTotalRecvSize(const char* pchRecvData, int iRecvSize)
{
    FRAME_HEADER* pstFrameHeader;
    if(iRecvSize < sizeof(FRAME_HEADER)){
        return -1;
    }
    pstFrameHeader = (FRAME_HEADER *)pchRecvData;
    return sizeof(FRAME_HEADER) + pstFrameHeader->iLength + sizeof(FRAME_TAIL);
}

void makeSendPacket(short nCmd, const MSG_ID* pstMsgId, const char* pchData)
{
    FRAME_HEADER *pstFrameHeader    = (FRAME_HEADER *)pchData;
    int iCmdSize                    = getSendCmdSize((cmd_id_t)nCmd);
    FRAME_TAIL *pstFrameTaile       = (FRAME_TAIL *)(pchData + sizeof(FRAME_HEADER) + iCmdSize);
    pstFrameHeader->unStx           = PACKET_STX;
    pstFrameHeader->stMsgId.chSrcId = pstMsgId->chDstId;
    pstFrameHeader->stMsgId.chDstId = pstMsgId->chSrcId;
    pstFrameHeader->chSubModule     = 0x00;
    pstFrameHeader->iLength         = getSendCmdSize((cmd_id_t)nCmd);
    pstFrameHeader->nCmd            = nCmd;

    pstFrameTaile->chCrc            = calculatePacketCrc(pchData, sizeof(FRAME_HEADER) + iCmdSize + sizeof(FRAME_TAIL));
    pstFrameTaile->unEtx             = PACKET_ETX;
}

void makeRecvPacket(short nCmd, const MSG_ID* pstMsgId, const char* pchData)
{
    FRAME_HEADER *pstFrameHeader    = (FRAME_HEADER *)pchData;
    int iCmdSize                    = getSendCmdSize((cmd_id_t)nCmd);
    FRAME_TAIL *pstFrameTaile       = (FRAME_TAIL *)(pchData + sizeof(FRAME_HEADER) + iCmdSize);
    pstFrameHeader->unStx           = PACKET_STX;
    pstFrameHeader->stMsgId.chSrcId = pstMsgId->chSrcId;
    pstFrameHeader->stMsgId.chDstId = pstMsgId->chDstId;
    pstFrameHeader->chSubModule     = 0x00;
    pstFrameHeader->iLength         = getRecvCmdSize((cmd_id_t)nCmd);
    pstFrameHeader->nCmd            = nCmd;

    pstFrameTaile->chCrc            = calculatePacketCrc(pchData, sizeof(FRAME_HEADER) + iCmdSize + sizeof(FRAME_TAIL));
    pstFrameTaile->unEtx             = PACKET_ETX;
}


unsigned int checkPacketFormat(const char* pchRecvData, const MSG_ID* pstMsgId, int iPacketSize) 
{
    FRAME_HEADER* pstHeader = (FRAME_HEADER*)pchRecvData;
    int iPayloadSize = pstHeader->iLength;
    FRAME_TAIL* pstTail = (FRAME_TAIL*)(pchRecvData + sizeof(FRAME_HEADER) + iPayloadSize);
    int iTotalPacketSize = getTotalRecvSize(pchRecvData, iPacketSize);
    unsigned int uiResult=PACKET_FORMAT_OK;
    
    if(iTotalPacketSize != iPacketSize){        
        uiResult |= PACKET_SIZE_ERROR;
        return uiResult;
    }

    if(pstHeader->unStx != PACKET_STX){
        uiResult |= PACKET_STX_ERROR;
    }
    
    if(pstHeader->stMsgId.chSrcId != pstMsgId->chSrcId){
        uiResult |= PACKET_SRC_ID_ERROR;
    }

    if(pstHeader->stMsgId.chDstId != pstMsgId->chDstId){
        uiResult |= PACKET_DST_ID_ERROR;
    }

    if(checkPacketCrc(pchRecvData, iTotalPacketSize)!= 0){
        uiResult |= PACKET_CRC_ERROR;
    }

    if(pstTail->unEtx != PACKET_ETX){
        uiResult |= PACKET_ETX_ERROR;
    }

    return uiResult;
}

void printPacketFormatError(unsigned int uiResult) 
{
    printf("패킷 포맷 오류:\n");

    if (uiResult & PACKET_SIZE_ERROR) {
        printf(" - 패킷 크기 불일치\n");
    }
    if (uiResult & PACKET_STX_ERROR) {
        printf(" - STX 오류 (시작 바이트)\n");
    }
    if (uiResult & PACKET_SRC_ID_ERROR) {
        printf(" - 송신 ID 오류\n");
    }
    if (uiResult & PACKET_DST_ID_ERROR) {
        printf(" - 수신 ID 오류\n");
    }
    if (uiResult & PACKET_CRC_ERROR) {
        printf(" - CRC 오류\n");
    }
    if (uiResult & PACKET_ETX_ERROR) {
        printf(" - ETX 오류 (종료 바이트)\n");
    }
}
