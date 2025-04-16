#include <stdio.h>
#include <string.h>
#include "command.h"

int handle_bit(const char* chpPayload, char* chResponse) {
    snprintf(chResponse, 128, "BIT OK");
    return 0;
}

int keepAlive(const char* pchPayload, char* pchResponse)
{
    SND_KEEP_ALIVE* pstSndKeepAlive = (SND_KEEP_ALIVE *)pchResponse;
    pstSndKeepAlive->chResult = 0x01;
    return CMD_SUCCESS;
}

int iBit(const char* pchPayload, char* pchResponse)
{
    SND_IBIT* pstSndIBit = (SND_IBIT *)pchResponse;
    pstSndIBit->chBitTotResult = 0x01;
    pstSndIBit->chPositionResult = 0x01;
    return CMD_SUCCESS;
}

int rBit(const char* pchPayload, char* pchResponse)
{
    SND_RBIT* pstSndRBit = (SND_RBIT *)pchResponse;
    pstSndRBit->chBitTotResult = 0x01;
    pstSndRBit->chPositionResult = 0x01;
    return CMD_SUCCESS;
}

int cBit(const char* pchPayload, char* pchResponse)
{
    RCV_CBIT* pstRcvCBit = (RCV_CBIT *)pchPayload;
    SND_CBIT* pstSndCBit = (SND_CBIT *)pchResponse;    
    pstSndCBit->chBitTotResult = 0x01;
    pstSndCBit->chPositionResult = 0x01;
    return CMD_SUCCESS;
}



/* UDS */
int recvGpsData(const char* pchPayload, char* pchResponse)
{
    GPS_DATA* pstRcvGpsData = (GPS_DATA *)pchPayload;
    SENSOR_DATA* pstSensorData = (SENSOR_DATA *)pchResponse;
    pstSensorData->stGpsData.dLatitude  = pstRcvGpsData->dLatitude;
    pstSensorData->stGpsData.dLongitude = pstRcvGpsData->dLongitude;
    pstSensorData->stGpsData.dAltitude  = pstRcvGpsData->dAltitude;
    fprintf(stderr, "##### %s():%d #####\n",__func__,__LINE__);
    fprintf(stderr,"Latitude : %lf\n", pstSensorData->stGpsData.dLatitude);
    fprintf(stderr,"Longitude : %lf\n", pstSensorData->stGpsData.dLongitude);
    fprintf(stderr,"Altitude : %lf\n", pstSensorData->stGpsData.dAltitude);
    return CMD_SUCCESS;
}

int recvImuData(const char* pchPayload, char* pchResponse)
{
    IMU_DATA* pstRcvImuData = (IMU_DATA *)pchPayload;
    SENSOR_DATA* pstSensorData = (SENSOR_DATA *)pchResponse;
    pstSensorData->stImuData.dRoll = pstRcvImuData->dRoll;
    pstSensorData->stImuData.dPitch = pstRcvImuData->dPitch;
    pstSensorData->stImuData.dYaw = pstRcvImuData->dYaw;
    fprintf(stderr, "##### %s():%d #####\n",__func__,__LINE__);
    fprintf(stderr,"Roll : %lf\n", pstSensorData->stImuData.dRoll);
    fprintf(stderr,"Pitch : %lf\n", pstSensorData->stImuData.dPitch);
    fprintf(stderr,"Yaw : %lf\n", pstSensorData->stImuData.dYaw);
    return CMD_SUCCESS;
}

int recvSpData(const char* pchPayload, char* pchResponse)
{
    SP_DATA* pstRcvSpData = (SP_DATA *)pchPayload;
    SENSOR_DATA* pstSensorData = (SENSOR_DATA *)pchResponse;
    pstSensorData->stSpData.dAz = pstRcvSpData->dAz;
    pstSensorData->stSpData.dEl = pstRcvSpData->dEl;
    fprintf(stderr, "##### %s():%d #####\n",__func__,__LINE__);
    fprintf(stderr,"AZ : %lf\n", pstSensorData->stSpData.dAz);
    fprintf(stderr,"EL : %lf\n", pstSensorData->stSpData.dEl);
    return CMD_SUCCESS;
}

int recvExternData(const char* pchPayload, char* pchResponse)
{
    EXTERN_DATA* pstRcvExternData = (EXTERN_DATA *)pchPayload;
    SENSOR_DATA* pstSensorData = (SENSOR_DATA *)pchResponse;
    pstSensorData->stExternData.dLatitude = pstRcvExternData->dLatitude;
    pstSensorData->stExternData.dLongitude = pstRcvExternData->dLongitude;
    pstSensorData->stExternData.dAltitude = pstRcvExternData->dAltitude;
    return CMD_SUCCESS;
}

int recvKeyboardData(const char* pchPayload, char* pchResponse)
{
    KEYBOARD_DATA* pstRcvKeyboardData = (KEYBOARD_DATA *)pchPayload;
    SENSOR_DATA* pstSensorData = (SENSOR_DATA *)pchResponse;
    pstSensorData->stKeyboardData.dAz = pstRcvKeyboardData->dAz;
    pstSensorData->stKeyboardData.dEl = pstRcvKeyboardData->dEl;
    return CMD_SUCCESS;
}

int recvAcuData(const char* pchPayload, char* pchResponse)
{
    ACU_DATA* pstRcvAcuData = (ACU_DATA *)pchPayload;
    SENSOR_DATA* pstSensorData = (SENSOR_DATA *)pchResponse;
    pstSensorData->stAcuData.dAz = pstRcvAcuData->dAz;
    pstSensorData->stAcuData.dEl = pstRcvAcuData->dEl;
    return CMD_SUCCESS;
}