#ifndef COMMAND_H
#define COMMAND_H


#define CMD_SUCCESS     0
#define CMD_FAILURE     -1

/**
 * @brief SND_KEEPALIVE 구조체
 * 제어PC와 TCP/IP통신에 사용되는 데이터 포멧
 */
typedef struct __attribute__((__packed__)){
	char chTmp;
}RCV_KEEP_ALIVE;
typedef struct __attribute__((__packed__)){
	char chResult;
}SND_KEEP_ALIVE;


typedef struct __attribute__((__packed__)){
	char chIbit;
}RCV_IBIT;
typedef struct __attribute__((__packed__)){
	char chBitTotResult;
	char chPositionResult;
}SND_IBIT;


typedef struct __attribute__((__packed__)){
	char chRbit;
}RCV_RBIT;
typedef struct __attribute__((__packed__)){
	char chBitTotResult;
	char chPositionResult;
}SND_RBIT;


typedef struct __attribute__((__packed__)){
	char chCbit;
}RCV_CBIT;
typedef struct __attribute__((__packed__)){
	char chBitTotResult;
	char chPositionResult;
}SND_CBIT;


// 명령별 핸들러 함수 선언
/**
 * @brief KeepAlive 응답 메시지를 생성합니다.
 * 
 * @param pchPayload 수신된 페이로드 (미사용)
 * @param pchResponse 응답 버퍼 (SND_KEEP_ALIVE 구조체 형태)
 * @return CMD_SUCCESS (정상)
 */
int keepAlive(const char* payload, char* response);

/**
 * @brief iBIT 응답 메시지를 생성합니다.
 * 
 * @param pchPayload 수신된 페이로드 (미사용)
 * @param pchResponse 응답 버퍼 (SND_IBIT 구조체 형태)
 * @return CMD_SUCCESS (정상)
 */
int iBit(const char* payload, char* response);

/**
 * @brief rBIT 응답 메시지를 생성합니다.
 * 
 * @param pchPayload 수신된 페이로드 (미사용)
 * @param pchResponse 응답 버퍼 (SND_RBIT 구조체 형태)
 * @return CMD_SUCCESS (정상)
 */
int rBit(const char* payload, char* response);

/**
 * @brief cBIT 요청을 처리하고 응답 메시지를 생성합니다.
 * 
 * @param pchPayload 수신된 페이로드 (RCV_CBIT 구조체)
 * @param pchResponse 응답 버퍼 (SND_CBIT 구조체)
 * @return CMD_SUCCESS (정상)
 */
int cBit(const char* payload, char* response);

/* UDS */
typedef struct __attribute__((__packed__)){
	double dLatitude;
    double dLongitude;
    double dAltitude;
}EXTERNAL1_DATA;

typedef struct __attribute__((__packed__)){
	double dRoll;
    double dPitch;
    double dYaw;
}EXTERNAL2_DATA;

typedef struct __attribute__((__packed__)){
	double dAz;
    double dEl;
}EXTERNAL3_DATA;

typedef struct __attribute__((__packed__)){
	double dLatitude;
    double dLongitude;
    double dAltitude;
}EXTERNAL4_DATA;

typedef struct __attribute__((__packed__)){
	double dAz;
    double dEl;
}PROCESSING_DATA;

typedef struct __attribute__((__packed__)){
	double dAz;
    double dEl;
}CONTROLL_DATA;

typedef struct __attribute__((__packed__)){
	char chCmd;
}COMMAND_DATA;

/**
 * @brief 외부 시스템으로부터 받은 위치 데이터(EXTERNAL1_DATA)를 저장합니다.
 * 
 * @param pchPayload 수신된 위치 데이터 (EXTERNAL1_DATA)
 * @param pchResponse 응답 버퍼 (SENSOR_DATA에 저장됨)
 * @return CMD_SUCCESS (정상)
 */
int recvExtern1Data(const char* pchPayload, char* pchResponse);

int recvExtern2Data(const char* pchPayload, char* pchResponse);
int recvExtern3Data(const char* pchPayload, char* pchResponse);
int recvExtern4Data(const char* pchPayload, char* pchResponse);
int recvProcessingData(const char* pchPayload, char* pchResponse);
int recvControllData(const char* pchPayload, char* pchResponse);
int recvCommandData(const char* pchPayload, char* pchResponse);



typedef struct __attribute__((__packed__)){
	EXTERNAL1_DATA stExtern1Data;
    EXTERNAL2_DATA stExtern2Data;
    EXTERNAL3_DATA stExtern3Data;
    EXTERNAL4_DATA stExtern4Data;
}SENSOR_DATA;
#endif