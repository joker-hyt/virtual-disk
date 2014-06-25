/**
 * 定义错误代码及错误信息
 * 错误信息打印
 */

#ifndef ERRMSG_H
#define ERRMSG_H

#include "MyString.h"

/**
 * ErrMsg类：提供错误信息
 */

class ErrMsg
{
public:
    static void printErr();

    enum
    {
        CMD_OK,
        CMD_ERROR,
        FIND_SPECIFIED_PATH_ERROR,
        FIND_SPECIFIED_FILE_ERROR,
        FIND_FILE_ERROR,
        INVALID_SWITCH_ERROR,
        CMD_SYNTACTIC_ERROR,
        PATH_LENGTH_ERROR,
        FILE_EXTENSION_LENGTH_ERROR,
        FILE_LENGTH_ERROR,
        FIND_PATH_ERROR,
        INVALID_PATH_NAME_ERROR,
        INVALID_DIRECTORY_NAME_ERROR,
        PATH_EXIST_ERROR,
        DIRECTORY_NOT_EMPTY_ERROR,
        FILE_DIRECTORY_SYNTACTIC_ERROR,
        RMDIR_ONESELF_ERROR,
        MEM_ALLOC_ERROR,
        ACCESS_DENY_ERROR,
        FIND_SPECIFIED_DRIVER_ERROR,
        CREATE_FILE_DIR_ERROR
    };
    const static MyString M_ERR_MSG[22];

    static int m_err_code;
    static int m_err;
    static MyString m_err_msg;
private:
    ErrMsg() {}
    ~ErrMsg() {}
};

#endif