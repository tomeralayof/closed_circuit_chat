#include <stdio.h>
#include <string.h>

size_t PackRegistration(char _buffer[], char _userName[], char _pass[]);
size_t PackResponse(char _buffer[], char _msg[]);
size_t UnPackRegistration(char _buffer[], size_t _messageLen, char _userName[], char _pass[]);
int UnPackResponse(char _buffer[], size_t _messageLen, char _msg[]);

int main(void)
{
    /*size_t packlen, unpackLen, msgLen = 50;
    int tag;
    char buffer[4096];
    char userName[50] = "ork", password[50] = "59000000000";
    char newuserName[50], newpassword[50];
    char msg[40] = "User registered successfuly";

    packlen = PackRegistration(buffer, userName, password);

    unpackLen = UnPackRegistration(buffer, msgLen, newuserName, newpassword);

    printf("name - %s\n", newuserName);

    printf("password - %s\n", newpassword);

    tag = (int)buffer[0];

    printf("tag - %d - Should be 1\n", tag);

     printf("size of tag - %ld\n", sizeof(buffer[0]));

    printf("total len - %d - Should be 16\n", buffer[1]);

    printf("username len - %d - Should be 3\n", buffer[2]);

    strncpy(newuserName, &buffer[3], (size_t)buffer[2]);

    printf("name - %s\n", newuserName);

    printf("passowrd len - %d - Should be 11\n", buffer[6]);

    strncpy(newpassword, &buffer[7], (size_t)buffer[6]);

    printf("password - %s\n", newpassword); */

    char buffer[4096];
    char msg[50] = "Registered successfuly", newMsg[50];
    size_t buffSize;
    int tag;

    buffSize = PackResponse(buffer, msg);

    UnPackResponse(buffer, buffSize, newMsg);

    printf("msg - %s\n", newMsg);


/*     tag = (int)buffer[0];
    printf("tag - %d - Should be 2\n", tag);

    printf("total len - %d - Should be 23\n", buffer[1]);

    strncpy(newMsg, &buffer[2], (size_t)buffer[1]);

    printf("msg - %s\n", newMsg); */

    return 0;

}

int UnPackResponse(char _buffer[], size_t _messageLen, char _msg[])
{
    if(_buffer == NULL)
    {
        return -1;
    }

    if (_buffer[0] != 2)
    {
        return -1;
    }

    strncpy(_msg, &_buffer[2], (size_t)_buffer[1]);

    return 1;
}




size_t PackResponse(char _buffer[], char _msg[])
{
    size_t counter = 0;
    int lenMsg;
    int i;

    if (_buffer == NULL || _msg == NULL)
    {
        return 0;
    }

    lenMsg = strlen(_msg);

    _buffer[counter++] = 2;
  
    _buffer[counter++] = lenMsg + 1;

    while (*_msg != '\0')
    {
        _buffer[counter++] = *_msg;
        _msg++;
    }
    
    return counter;
}



size_t UnPackRegistration(char _buffer[], size_t _messageLen, char _userName[], char _pass[])
{

    size_t passLenIndex;

    if(_buffer == NULL)
    {
        return 2;
    }

    strncpy(_userName, &_buffer[3], (size_t)_buffer[2]);

    printf("pass len - %ld\n", (size_t)_buffer[6]);

    passLenIndex = (size_t)(3+(size_t)_buffer[2]);

    printf("pass len - %ld\n", passLenIndex);

    strncpy(_pass, &_buffer[passLenIndex + 1], (size_t)_buffer[passLenIndex]);


    return 1;
}


size_t PackRegistration(char _buffer[], char _userName[], char _pass[])
{
    size_t counter = 0;
    int lenUsrName,lenPass;
    int i;

    if (_buffer == NULL || _userName == NULL|| _pass== NULL)
    {
        return 0;
    }

    lenUsrName = strlen(_userName);
    lenPass = strlen(_pass);
    _buffer[counter++] = 1;
  
    _buffer[counter++] = lenUsrName + lenPass + 2;
    _buffer[counter++] = lenUsrName;

    while (*_userName != '\0')
    {
        _buffer[counter++] = *_userName;
        _userName++;
    }
    
    _buffer[counter++] = lenPass;

    while (*_pass != '\0')
    {
        _buffer[counter++] = *_pass;
        _pass++;
    }

    printf("%ld\n",counter );
    return counter;
}


