
#include <stdio.h>
#include "../../Include/server_app.h"
#include "../../Include/data_net.h"

#define FAIL -1
#define OK 0

int main()
{
    ServerAppManager *server;

    if ((server = CreateAppManager(PORT)) == NULL)
    {
        return FAIL;
    }

    if (ServerManagerRun(server) != SERVER_APP_SUCCESS)
    {
        return FAIL;
    }

    DestroyAppManager(&server);

    return OK;
}