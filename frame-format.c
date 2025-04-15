#include "frame-format.h"
#include "cmd-handler.h"
#include <stdio.h>

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

int checkPacketCrc(const char* pchData, int iSize)
{
	unsigned char uchCrc=calculatePacketCrc(pchData, iSize);

	if((uchCrc & 0x000000FF) == (pchData[iSize-EXCLUDING_CRC_POINT] & 0x000000FF))
		return 0;

	return -1;
}

int getTotalRecvSize(char* pchRecvData, int iRecvSize)
{
    FRAME_HEADER* pstFrameHeader;
    if(iRecvSize < sizeof(FRAME_HEADER)){
        return -1;
    }
    pstFrameHeader = (FRAME_HEADER *)pchRecvData;
    return sizeof(FRAME_HEADER) + pstFrameHeader->iLength + sizeof(FRAME_TAIL);
}

void makeSendPacket(short nCmd, const char* chMsgId, const char* pchData)
{
    FRAME_HEADER *pstFrameHeader    = (FRAME_HEADER *)pchData;
    int iCmdSize                    = getSendCmdSize((cmd_id_t)nCmd);
    FRAME_TAIL *pstFrameTaile       = (FRAME_TAIL *)(pchData + sizeof(FRAME_HEADER) + iCmdSize);
    pstFrameHeader->unStx            = PACKET_STX;
    pstFrameHeader->chSrcId         = PACKET_DST_ID;
    pstFrameHeader->chDstId         = PACKET_SRC_ID;
    pstFrameHeader->chSubModule     = 0x00;
    pstFrameHeader->iLength         = getSendCmdSize((cmd_id_t)nCmd);
    pstFrameHeader->nCmd            = nCmd;

    pstFrameTaile->chCrc            = calculatePacketCrc(pchData, sizeof(FRAME_HEADER) + iCmdSize + sizeof(FRAME_TAIL));
    pstFrameTaile->unEtx             = PACKET_ETX;
}

/* for Google test*/
void makeRecvPacket(short nCmd, const char* chMsgId, const char* pchData)
{
    FRAME_HEADER *pstFrameHeader    = (FRAME_HEADER *)pchData;
    int iCmdSize                    = getSendCmdSize((cmd_id_t)nCmd);
    FRAME_TAIL *pstFrameTaile       = (FRAME_TAIL *)(pchData + sizeof(FRAME_HEADER) + iCmdSize);
    pstFrameHeader->unStx            = PACKET_STX;
    pstFrameHeader->chSrcId         = PACKET_SRC_ID;
    pstFrameHeader->chDstId         = PACKET_DST_ID;
    pstFrameHeader->chSubModule     = 0x00;
    pstFrameHeader->iLength         = getRecvCmdSize((cmd_id_t)nCmd);
    pstFrameHeader->nCmd            = nCmd;

    pstFrameTaile->chCrc            = calculatePacketCrc(pchData, sizeof(FRAME_HEADER) + iCmdSize + sizeof(FRAME_TAIL));
    pstFrameTaile->unEtx             = PACKET_ETX;
}


unsigned int checkPacketFormat(char* pchRecvData, int iPacketSize) 
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
        fprintf(stderr,"%d(%x) %d(%x)\n", pstHeader->unStx, pstHeader->unStx, PACKET_STX, PACKET_STX);
        uiResult |= PACKET_STX_ERROR;
    }
    
    if(pstHeader->chSrcId != PACKET_SRC_ID){
        uiResult |= PACKET_SRC_ID_ERROR;
    }

    if(pstHeader->chDstId != PACKET_DST_ID){
        uiResult |= PACKET_DST_ID_ERROR;
    }

    if(checkPacketCrc(pchRecvData, iTotalPacketSize)!= 0){
        uiResult |= PACKET_CRC_ERROR;
    }

    if(pstTail->unEtx != PACKET_ETX){
        fprintf(stderr,"%d(%x) %d(%x)\n", pstTail->unEtx, pstTail->unEtx, PACKET_ETX, PACKET_ETX);
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
