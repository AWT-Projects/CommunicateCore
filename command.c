#include <stdio.h>
#include <string.h>
#include "command.h"

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
int recvExtern1Data(const char* pchPayload, char* pchResponse)
{
    EXTERNAL1_DATA* pstRcvExtern1Data = (EXTERNAL1_DATA *)pchPayload;
    SENSOR_DATA* pstSensorData = (SENSOR_DATA *)pchResponse;
    pstSensorData->stExtern1Data.dLatitude  = pstRcvExtern1Data->dLatitude;
    pstSensorData->stExtern1Data.dLongitude = pstRcvExtern1Data->dLongitude;
    pstSensorData->stExtern1Data.dAltitude  = pstRcvExtern1Data->dAltitude;
    return CMD_SUCCESS;
}

int recvExtern2Data(const char* pchPayload, char* pchResponse)
{
    EXTERNAL2_DATA* pstRcvExtern2Data = (EXTERNAL2_DATA *)pchPayload;
    SENSOR_DATA* pstSensorData = (SENSOR_DATA *)pchResponse;
    pstSensorData->stExtern2Data.dRoll  = pstRcvExtern2Data->dRoll;
    pstSensorData->stExtern2Data.dPitch = pstRcvExtern2Data->dPitch;
    pstSensorData->stExtern2Data.dYaw   = pstRcvExtern2Data->dYaw;
    return CMD_SUCCESS;
}

int recvExtern3Data(const char* pchPayload, char* pchResponse)
{
    EXTERNAL3_DATA* pstRcvExtern3Data = (EXTERNAL3_DATA *)pchPayload;
    SENSOR_DATA* pstSensorData = (SENSOR_DATA *)pchResponse;
    pstSensorData->stExtern3Data.dAz = pstRcvExtern3Data->dAz;
    pstSensorData->stExtern3Data.dEl = pstRcvExtern3Data->dEl;
    return CMD_SUCCESS;
}

int recvExtern4Data(const char* pchPayload, char* pchResponse)
{
    EXTERNAL4_DATA* pstRcvExtern4Data = (EXTERNAL4_DATA *)pchPayload;
    SENSOR_DATA* pstSensorData = (SENSOR_DATA *)pchResponse;
    pstSensorData->stExtern4Data.dLatitude  = pstRcvExtern4Data->dLatitude;
    pstSensorData->stExtern4Data.dLongitude = pstRcvExtern4Data->dLongitude;
    pstSensorData->stExtern4Data.dAltitude  = pstRcvExtern4Data->dAltitude;
    return CMD_SUCCESS;
}

int recvProcessingData(const char* pchPayload, char* pchResponse)
{
    PROCESSING_DATA* pstRcvProcessingData = (PROCESSING_DATA *)pchPayload;
    return CMD_SUCCESS;
}

int recvControllData(const char* pchPayload, char* pchResponse)
{
    CONTROLL_DATA* pstRcvControllData = (CONTROLL_DATA *)pchPayload;
    return CMD_SUCCESS;
}

int recvCommandData(const char* pchPayload, char* pchResponse)
{
    COMMAND_DATA* pstRcvCommandData = (COMMAND_DATA *)pchPayload;
    return CMD_SUCCESS;
}
