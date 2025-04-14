#include <stdio.h>
#include "cmd-handler.h"



void logging(int iCmdId, const char* pchStatus)
{
    printf("[LOG] CMD_ID: %d (%s), STATUS: %s\n", iCmdId, getCmdName((cmd_id_t)iCmdId), pchStatus);
}
