#ifndef TCP_FRAME_FORMAT_H
#define TCP_FRAME_FORMAT_H


#define CRC_START_POINT 		6
#define EXCLUDING_CRC_POINT		3
#define PACKET_STX				0xF0F0
#define PACKET_SRC_ID			0x01
#define PACKET_DST_ID			0x02
#define PACKET_ETX				0xFFFF


typedef enum{
	PACKET_FORMAT_OK		= 0x00,
	PACKET_STX_ERROR 		= 0x01,
	PACKET_SRC_ID_ERROR		= 0x02,
	PACKET_DST_ID_ERROR 	= 0x04,
	PACKET_CRC_ERROR 		= 0x08,
	PACKET_ETX_ERROR 		= 0x10,
	PACKET_SIZE_ERROR 		= 0x20
}PACKET_FORMAT_STATUS;

/**
 * @brief FRAME_HEADER 구조체
 * 제어PC와 TCP/IP통신에 사용되는 데이터 포멧
 */
typedef struct __attribute__((__packed__)){
	short 	nStx;			/**< 제어PC와 TCP/IP통신시 사용되는 데이터 프레임의 시작 */
	int 	iLength;		/**< 제어PC와 TCP/IP통신시 전송되는 데이터의 크기 */
	char 	chSrcId;		/**< 제어PC와 TCP/IP통신시 송신하는 장치ID */
	char 	chDstId;		/**< 제어PC와 TCP/IP통신시 수신하는 장치ID */
	char 	chSubModule;	/**< 안테나 제어장치에서는 사용되지 않음 */
	short 	nCmd;			/**< 제어PC와 TCP/IP통신시 데이터의 명령 종류 */
}FRAME_HEADER;

/**
 * @brief FRAME_PACKET 구조체
 * 제어PC와 TCP/IP통신에 사용되는 데이터 프레임 정보를 가진다.
 */
typedef struct __attribute__((__packed__)){
	char 	chCrc;			    /**< 제어PC와 TCP/IP통신시 사용되는 데이터의 CRC값 */
	short 	nEtx;			    /**< 제어PC와 TCP/IP통신시 사용되는 데이터 프레임의 끝 */
}FRAME_TAIL;


void makeSendPacket(short nCmd, const char* chMsgId, const char* pchData);
void makeRecvPacket(short nCmd, const char* chMsgId, const char* pchData);
unsigned int checkPacketFormat(char* pchRecvData, int iPacketSize);
int getTotalRecvSize(char* pchRecvData, int iRecvSize);
#endif