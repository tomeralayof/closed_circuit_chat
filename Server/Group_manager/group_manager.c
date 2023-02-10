#include <stdlib.h> /* malloc, size_t */
#include <string.h> /* strcpy, itoa */
#include <stdio.h>  /* sprintf */

#include "../../Include/group_manager.h"
#include "../../Include/group.h"
#include "../../Include/limitations.h"
#include "../../Include/hash_map.h"
#include "../../Include/g_queue.h"
#include "../../Include/data_net.h"
#include "../../Include/g_vector.h"

#define MAGIC_NUMBER 942357
#define HASH_SIZE_MAX 30
#define CONVERSION_BASE 10
#define NULL_CHARACTER ('\0')
#define NOT_EQUAL 1
#define EQUAL 0
#define IS_EMPTY 0

/* --------------------------------- Structs ----------------------------  */

struct GroupManager
{
    HashMap *m_hashGroups;
    Queue *m_queueIP;
    int m_magicNumber;
};

struct NetworkData
{
    char m_ip[MAX_IP_LEN];
    char m_port[PORT_SIZE];
};

/* ------------------------ Static Function Declaration ----------------------  */

static int EqualityFunc(void *_firstData, void *_secondData);

static size_t HashFunc(void *_keyName);

static size_t CalcAsciiVal(char *_elem);

static GroupManagerErr CreateIpQueue(GroupManager *_groupManager);

static void CreateIpAndPort(int _index, NetworkData* _netData);

static GroupManagerErr InsertIpToQueue(GroupManager *_groupManager, Group* _group);

static GroupManagerErr InsertGroupToHash(GroupManager *_groupManager, char *_name, NetworkData* _netData, Group** _group);

static void DestroyItemNetData(void *_element);

static void ValDestroyGroup(void *_value);

static void KeyDestroyName(void *_keyName);

static int GetNameActionFunction(const void *_key, void *_value, void *_context);

/* ----------------------------------- API ------------------------------- */

GroupManager *CreateGroupManager()
{
    GroupManager *ptrGroupManager;

    if ((ptrGroupManager = (GroupManager *)malloc(sizeof(GroupManager))) == NULL)
    {
        return NULL;
    }

    ptrGroupManager->m_hashGroups = HashMapCreate(HASH_SIZE_GROUPS, HashFunc, EqualityFunc);

    if (ptrGroupManager->m_hashGroups == NULL)
    {
        free(ptrGroupManager);
        return NULL;
    }

    ptrGroupManager->m_queueIP = QueueCreate(MAX_ACTIVE_GROUP_NUMBER);

    if (ptrGroupManager->m_queueIP == NULL)
    {
        free(ptrGroupManager->m_hashGroups);
        free(ptrGroupManager);
        return NULL;
    }

    CreateIpQueue(ptrGroupManager);

    ptrGroupManager->m_magicNumber = MAGIC_NUMBER;

    return ptrGroupManager;
}

void DestroyGroupManager(GroupManager *_groupManager)
{
    if (_groupManager != NULL && _groupManager->m_magicNumber == MAGIC_NUMBER)
    {
        QueueDestroy(&(_groupManager->m_queueIP), DestroyItemNetData);
        HashMapDestroy(&(_groupManager->m_hashGroups), KeyDestroyName, ValDestroyGroup);
        _groupManager->m_magicNumber = 0;
        free(_groupManager);
    }
}

GroupManagerErr AddGroup(GroupManager *_groupManager, char *_name, char *_ip, char *_port, char **_nameBuffer)
{
    void *item;
    NetworkData* netData; 
    Group* group;

    if (_groupManager == NULL || _name == NULL || _ip == NULL || _port == NULL || _nameBuffer == NULL)
    {
        return GROUP_MANAGER_NOT_INITIALIZED;
    }

    if (HashMapFind(_groupManager->m_hashGroups, _name, &item) == MAP_SUCCESS)
    {
        return GROUP_MANAGER_UNAVAILBLE_NAME;
    }

    if (QueueRemove(_groupManager->m_queueIP, (void **)&netData) == QUEUE_UNDERFLOW_ERROR)
    {
        return GROUP_MANAGER_OVERFLOW;
    }

    if( InsertGroupToHash(_groupManager, _name, netData, &group) == GROUP_MANAGER_ALLOCATION_ERR)
    {
        return GROUP_MANAGER_ALLOCATION_ERR;
    }

    GetGroupName(group, _nameBuffer);
    GetGroupIp(group, _ip);
    GetGroupPort(group, _port);

    IncreaseUsersCounter(group);

    free(netData);

    return GROUP_MANAGER_SUCCESS;
}



GroupManagerErr JoinGroup(GroupManager *_groupManager, char *_name, char *_ip, char *_port, char **_nameBuffer)
{
    void *item;

    if (_groupManager == NULL || _name == NULL || _ip == NULL || _port == NULL || _nameBuffer == NULL)
    {
        return GROUP_MANAGER_NOT_INITIALIZED;
    }

    if (HashMapFind(_groupManager->m_hashGroups, _name, &item) != MAP_SUCCESS)
    {
        return GROUP_MANAGER_UNAVAILBLE_NAME; /* if the group does not exist */
    }

    GetGroupIp((Group *)item, _ip);
    GetGroupPort((Group *)item, _port);
    IncreaseUsersCounter((Group *)item);

    GetGroupName((Group *)item, _nameBuffer);

    return GROUP_MANAGER_SUCCESS;
}

GroupManagerErr LeaveGroup(GroupManager *_groupManager, char *_name)
{
    void *group, *key, *value;
    char ip[MAX_IP_LEN], port[PORT_SIZE];
    GroupManagerErr status;

    if (_groupManager == NULL || _name == NULL)
    {
        return GROUP_MANAGER_NOT_INITIALIZED;
    }

    if (HashMapFind(_groupManager->m_hashGroups, _name, &group) != MAP_SUCCESS)
    {
        return GROUP_MANAGER_UNAVAILBLE_NAME; /* if the group does not exist */
    }

    DecreaseUsersCounter((Group *)group);

    if (GetGroupNumOfUsers((Group *)group) == IS_EMPTY)
    {
        if( (status = InsertIpToQueue(_groupManager, group)) != GROUP_MANAGER_SUCCESS)
        {
            return status;
        }
        HashMapRemove(_groupManager->m_hashGroups, _name, &key, &value);
        free((char *)key); /* free the group name */
        DestroyGroup((Group *)group);
    }

    return GROUP_MANAGER_SUCCESS;
}


GroupManagerErr GetGroupNames(GroupManager *_groupManager, Vector *_vector)
{
    if (_groupManager == NULL || _vector == NULL)
    {
        return GROUP_MANAGER_NOT_INITIALIZED;
    }

    HashMapForEach(_groupManager->m_hashGroups, GetNameActionFunction, _vector);

    return GROUP_MANAGER_SUCCESS;
}



size_t GetNumOfGroups(GroupManager *_groupManager)
{
    if (_groupManager == NULL)
    {
        return 0;
    }

    return HashMapSize(_groupManager->m_hashGroups);
}

/* ---------------------------- Static Function  --------------------------  */



static GroupManagerErr CreateIpQueue(GroupManager *_groupManager)
{
    char *ip;
    NetworkData* netData;
    int i;

    for (i = 1; i <= MAX_ACTIVE_GROUP_NUMBER; i++)
    {

        if ((netData = (NetworkData *)malloc(sizeof(NetworkData))) == NULL)
        {
            return GROUP_MANAGER_ALLOCATION_ERR;
        }

        CreateIpAndPort(i, netData);

        QueueInsert(_groupManager->m_queueIP, netData);

    }
    return GROUP_MANAGER_SUCCESS;
}

static void CreateIpAndPort(int _index, NetworkData* _netData)
{
    char buffer[4];
    char port[PORT_SIZE];

    strcpy(_netData->m_ip, "224.0.0.");
    sprintf(buffer, "%d", _index);
    buffer[strlen(buffer)] = '\0';
    strcat(_netData->m_ip, buffer);

    sprintf(port, "%d", (MIN_PORT_NUM + _index -1));
    strcat(_netData->m_port, port);

    return;
}

static GroupManagerErr InsertIpToQueue(GroupManager *_groupManager, Group* _group)
{
    NetworkData* netData;

    if ((netData = (NetworkData *)malloc(sizeof(NetworkData))) == NULL)
    {
        return GROUP_MANAGER_ALLOCATION_ERR;
    }

    GetGroupIp(_group, netData->m_ip);
    GetGroupPort(_group, netData->m_port);

    QueueInsert(_groupManager->m_queueIP, netData);
    
    return GROUP_MANAGER_SUCCESS;
}

static GroupManagerErr InsertGroupToHash(GroupManager *_groupManager, char *_name, NetworkData* _netData, Group** _group)
{
    size_t nameSize;
    char *keyName;

    nameSize = strlen(_name);

    if ((keyName = (char *)malloc(nameSize + 1)) == NULL)
    {
        return GROUP_MANAGER_ALLOCATION_ERR;
    }

    strcpy(keyName, _name);
    keyName[nameSize] = '\0';

    if ((*_group = CreateGroup(_netData->m_ip, keyName, _netData->m_port)) == NULL)
    {
        free(keyName);
        return GROUP_MANAGER_ALLOCATION_ERR;
    }

    if (HashMapInsert(_groupManager->m_hashGroups, keyName, *_group) != MAP_SUCCESS)
    {
        free(keyName);
        DestroyGroup(*_group);
        return GROUP_MANAGER_ALLOCATION_ERR;
    }
    return GROUP_MANAGER_SUCCESS;
}


static int GetNameActionFunction(const void *_key, void *_value, void *_context)
{
    VectorAppend(((Vector *)_context), (void *)_key);
    return 1;
}

static void DestroyItemNetData(void *_element)
{
    free((NetworkData *)_element);
}

static void ValDestroyGroup(void *_value)
{
    DestroyGroup((Group *)_value);
}

static void KeyDestroyName(void *_keyName)
{
    free((char *)_keyName);
}

static size_t HashFunc(void *_keyName)
{
    return CalcAsciiVal((char *)_keyName) * 2;
}

static size_t CalcAsciiVal(char *_elem)
{
    size_t val = 0;

    for (; *_elem != NULL_CHARACTER; ++_elem)
    {
        val += (int)*_elem;
    }

    return val;
}

static int EqualityFunc(void *_firstData, void *_secondData)
{
    if (strcmp((char *)_firstData, (char *)_secondData) == EQUAL)
    {
        return EQUAL;
    }

    return NOT_EQUAL;
}

/* ---------------------------- Get Function  --------------------------  */

Queue *GetQueue(GroupManager *_groupManager)
{
    return _groupManager->m_queueIP;
}

Group *GetGroup(GroupManager *_groupManager, char *_name)
{
    void *item;

    if (HashMapFind(_groupManager->m_hashGroups, _name, &item) == MAP_SUCCESS)
    {
        return (Group *)item;
    }

    return NULL;
}