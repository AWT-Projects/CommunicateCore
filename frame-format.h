#ifndef FRAME_FORMAT_H
#define FRAME_FORMAT_H

/**
 * @def CRC_START_POINT
 * @brief CRC 계산을 시작하는 오프셋 위치 (바이트 단위)
 */
#define CRC_START_POINT 		6

/**
 * @def EXCLUDING_CRC_POINT
 * @brief CRC 계산에서 제외할 마지막 바이트 수 (CRC 및 ETX 등)
 */
#define EXCLUDING_CRC_POINT		3

/**
 * @def PACKET_STX
 * @brief 패킷의 시작(STX)을 나타내는 고정 값
 */
#define PACKET_STX				0xF0F0

/**
 * @def UNKNOWN_ID
 * @brief 응답이 필요 없는 경우 사용
 */
#define UNKNOWN_ID				0x00

/**
 * @def CTRL_PC_ID
 * @brief 제어 PC의 송신자 ID
 */
#define CTRL_PC_ID				0x01

/**
 * @def MY_TCP_ID
 * @brief 수신 대상 장치의 ID
 */
#define MY_TCP_ID				0x02

/**
 * @def UDS2_ID
 * @brief 수신 대상 장치의 ID
 */
#define UDS2_ID					0x10

/**
 * @def UDS3_ID
 * @brief 수신 대상 장치의 ID
 */
#define UDS33_ID				0x11

/**
 * @def UDS4_ID
 * @brief 수신 대상 장치의 ID
 */
#define UDS4_ID					0x12

/**
 * @def EXTERN1_ID
 * @brief 수신 대상 장치의 ID
 */
#define EXTERN1_ID				0x21

/**
 * @def EXTERN2_ID
 * @brief 수신 대상 장치의 ID
 */
#define EXTERN2_ID				0x22

/**
 * @def EXTERN3_ID
 * @brief 수신 대상 장치의 ID
 */
#define EXTERN3_ID				0x23

/**
 * @def EXTERN4_ID
 * @brief 수신 대상 장치의 ID
 */
#define EXTERN4_ID				0x24

/**
 * @def PACKET_ETX
 * @brief 패킷의 끝(ETX)을 나타내는 고정 값
 */
#define PACKET_ETX				0xFFFF

/**
 * @enum PACKET_FORMAT_STATUS
 * @brief 수신된 패킷의 포맷 검사 결과를 나타내는 열거형
 */
typedef enum{
	PACKET_FORMAT_OK		= 0x00,	/**< 포맷 이상 없음 */
	PACKET_STX_ERROR 		= 0x01,	/**< STX 오류 (시작 바이트 불일치) */
	PACKET_SRC_ID_ERROR		= 0x02, /**< 송신 ID 오류 */
	PACKET_DST_ID_ERROR 	= 0x04,	/**< 수신 ID 오류 */
	PACKET_CRC_ERROR 		= 0x08,	/**< CRC 오류 */
	PACKET_ETX_ERROR 		= 0x10, /**< ETX 오류 (종료 바이트 불일치) */
	PACKET_SIZE_ERROR 		= 0x20 	/**< 패킷 길이 오류 */
}PACKET_FORMAT_STATUS;

/**
 * @struct MSG_ID
 * @brief 송/수신자 ID 정보를 포함하는 구조체
 *
 * 통신 시 송신자와 수신자의 ID를 나타냅니다.
 */
typedef struct __attribute__((__packed__)){
	char 			chSrcId;		/**< 송신자 ID (Source ID) */
	char 			chDstId;		/**< 수신자 ID (Destination ID) */
}MSG_ID;

/**
 * @struct FRAME_HEADER
 * @brief 데이터 프레임의 헤더 구조체
 *
 * 패킷의 시작 정보, 메시지 식별자, 데이터 길이 및 명령 번호를 포함합니다.
 */
typedef struct __attribute__((__packed__)) {
    unsigned short unStx;       /**< 프레임 시작 바이트 (STX) */
    int            iLength;     /**< 데이터 필드의 길이 (Payload 크기) */
    MSG_ID         stMsgId;     /**< 송신자 및 수신자 ID 정보 */
    char           chSubModule; /**< 서브모듈 ID (해당 시스템에서는 사용되지 않음) */
    short          nCmd;        /**< 명령 코드 */
} FRAME_HEADER;

/**
 * @struct FRAME_TAIL
 * @brief 데이터 프레임의 끝 구조체
 *
 * CRC 및 프레임 종료 바이트를 포함합니다.
 */
typedef struct __attribute__((__packed__)) {
    char           chCrc;       /**< 데이터에 대한 CRC 값 */
    unsigned short unEtx;       /**< 프레임 종료 바이트 (ETX) */
} FRAME_TAIL;


/**
 * @brief 수신한 데이터에서 전체 패킷 크기를 계산합니다.
 *
 * @param pchRecvData 수신된 데이터 버퍼
 * @param iRecvSize 수신된 전체 데이터 크기
 * @return 전체 패킷 크기, 실패 시 -1
 */
int getTotalRecvSize(const char* pchRecvData, int iRecvSize);

/**
 * @brief 송신용 패킷을 생성합니다.
 *
 * @param nCmd 명령 코드
 * @param pstMsgId 송/수신 ID 정보
 * @param pchData 패킷이 생성될 버퍼 (프리할당 필요)
 */
void makeSendPacket(short nCmd, const MSG_ID* pstMsgId, const char* pchData);

/**
 * @brief 단위 테스트용 수신 패킷을 생성합니다.
 *
 * @param nCmd 명령 코드
 * @param pstMsgId 송/수신 ID 정보
 * @param pchData 패킷이 생성될 버퍼 (프리할당 필요)
 */
void makeRecvPacket(short nCmd, const MSG_ID* pstMsgId, const char* pchData);

/**
 * @brief 수신된 패킷의 포맷이 정상인지 검사합니다.
 *
 * @param pchRecvData 수신된 패킷 데이터
 * @param pstMsgId 기대되는 송/수신 ID 정보
 * @param iPacketSize 수신된 전체 패킷 크기
 * @return 포맷 오류 결과 (비트 OR 형식, PACKET_FORMAT_STATUS 사용)
 */
unsigned int checkPacketFormat(const char* pchRecvData, const MSG_ID* pstMsgId, int iPacketSize);

/**
 * @brief 패킷 포맷 검사 결과를 출력합니다.
 *
 * @param uiResult checkPacketFormat에서 반환된 결과 값
 */
void printPacketFormatError(unsigned int uiResult);


#endif