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




#endif