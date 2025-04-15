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
int keepAlive(const char* payload, char* response);
int iBit(const char* payload, char* response);
int rBit(const char* payload, char* response);
int cBit(const char* payload, char* response);

/* UDS */
typedef struct __attribute__((__packed__)){
	double dLatitude;
    double dLongitude;
    double dAltitude;
}GPS_DATA;

typedef struct __attribute__((__packed__)){
	double dRoll;
    double dPitch;
    double dYaw;
}IMU_DATA;

typedef struct __attribute__((__packed__)){
	double dAz;
    double dEl;
}SP_DATA;

typedef struct __attribute__((__packed__)){
	double dLatitude;
    double dLongitude;
    double dAltitude;
}EXTERN_DATA;

typedef struct __attribute__((__packed__)){
	double dAz;
    double dEl;
}KEYBOARD_DATA;

typedef struct __attribute__((__packed__)){
	double dAz;
    double dEl;
}ACU_DATA;

typedef struct __attribute__((__packed__)){
	GPS_DATA stGpsData;
    IMU_DATA stImuData;
    SP_DATA stSpData;
    EXTERN_DATA stExternData;
    KEYBOARD_DATA stKeyboardData;
    ACU_DATA stAcuData;
}SENSOR_DATA;

int recvGpsData(const char* pchPayload, char* pchResponse);
int recvImuData(const char* pchPayload, char* pchResponse);
int recvSpData(const char* pchPayload, char* pchResponse);
int recvExternData(const char* pchPayload, char* pchResponse);
int recvKeyboardData(const char* pchPayload, char* pchResponse);

#endif