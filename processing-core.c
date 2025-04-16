#include "process2.h"
#include <sys/time.h>

void* processingSensorDataThread(void* arg) 
{    
    SHARED_SENSOR_DATA *pstSharedSensorData = (SHARED_SENSOR_DATA*)arg;
    struct timeval stNow;
    struct timespec stTimeout;

    while (1) {
        gettimeofday(&stNow, NULL);
        stTimeout.tv_sec = stNow.tv_sec + 1; /**< 1초 타임아웃 설정 */
        stTimeout.tv_nsec = stNow.tv_usec * 1000;
        /**< 데이터 준비 상태 대기 */
        pthread_mutex_lock(&pstSharedSensorData->mutex);
        int ret = pthread_cond_timedwait(&pstSharedSensorData->cond, &pstSharedSensorData->mutex, &stTimeout);
        if (ret != ETIMEDOUT) {
            fprintf(stderr,"Latitude : %lf\n", pstSharedSensorData->stSensorData.stGpsData.dLatitude);
            fprintf(stderr,"Longitude : %lf\n", pstSharedSensorData->stSensorData.stGpsData.dLongitude);
            fprintf(stderr,"Altitude : %lf\n", pstSharedSensorData->stSensorData.stGpsData.dAltitude);

            fprintf(stderr,"Roll : %lf\n", pstSharedSensorData->stSensorData.stImuData.dRoll);
            fprintf(stderr,"Pitch : %lf\n", pstSharedSensorData->stSensorData.stImuData.dPitch);
            fprintf(stderr,"Yaw : %lf\n", pstSharedSensorData->stSensorData.stImuData.dYaw);

            // fprintf(stderr,"Latitude : %lf\n", pstSharedSensorData->stSensorData.stExternData.dLatitude);
            // fprintf(stderr,"Longitude : %lf\n", pstSharedSensorData->stSensorData.stExternData.dLongitude);
            // fprintf(stderr,"Altitude : %lf\n", pstSharedSensorData->stSensorData.stExternData.dAltitude);
            
            fprintf(stderr,"AZ : %lf\n", pstSharedSensorData->stSensorData.stSpData.dAz);
            fprintf(stderr,"EL : %lf\n", pstSharedSensorData->stSensorData.stSpData.dEl);

            // fprintf(stderr,"AZ : %lf\n", pstSharedSensorData->stSensorData.stKeyboardData.dAz);
            // fprintf(stderr,"EL : %lf\n", pstSharedSensorData->stSensorData.stKeyboardData.dEl);
            
            pthread_mutex_unlock(&pstSharedSensorData->mutex);            
        }
    }

    return NULL;
}