/**
 * 错误信息打印实现
 */

#include "ErrMsg.h"
#include <stdio.h>

/* 命令错误信息初始化 */
const MyString ErrMsg::M_ERR_MSG[22] = {
    "命令正确",                                           // 正确
    " - 不是内部或外部命令，也不是可运行的程序\n或批处理文件。",  // 命令错误
    "系统找不到指定的路径。",                               // "cd sss"
    "系统找不到指定的文件。",                               //                             "copy sss"                                       "rmdir sss"      "compare sss aaa"
    "找不到文件",                                         //               "dir sss"
    "无效开关 - ",                                        //               "dir /z"                     "del /ab"                         "rmdir /a"
    "命令语法不正确。",                                    //                             "copy"          "del"          "mkdir",          "rmdir"          "compare"
    " - 的完整路径太长。",                                 //                                                            "mkdir a..."
    "文件名或扩展名太长。",                                 // "cd a..."
    "文件名太长。",
    "找不到 - ",                                         //                                              "del sss"
    "文件或路径名不合法。",                                 //                                                            "mkdir a:a"
    "目录名称无效。",                                      // "cd sss"                                                                     "rmdir a.txt"
    " - 子目录或文件已经存在。",                            //                                                            "mkdir aaa"
    "目录不是空的。",                                      //                                                                              "rmdir sss"
    "文件名、目录名或卷标语法不正确。",                       // 全部
    "另一个程序正在使用此文件，进程无法访问。",                 //                                                                               "rmdir oneself"
    " - 文件太大，无法分配内存",                            // 全部
    " - 拒绝访问。",                                      //                             "copy aa a?(a?是目录)"
    "系统找不到指定的驱动器。",                              //                             "copy aa z:\"
    "无法创建目录或文件"
};

int ErrMsg::m_err = 0;
int ErrMsg::m_err_code = 0;
MyString ErrMsg::m_err_msg = M_ERR_MSG[0];

/* 打印错误信息 */
void ErrMsg::printErr()
{
    switch(m_err_code)
    {
    case CMD_OK:
        break;

    case FIND_SPECIFIED_PATH_ERROR:
    case FIND_SPECIFIED_FILE_ERROR:
    case FIND_FILE_ERROR:
    case CMD_SYNTACTIC_ERROR:
    case INVALID_PATH_NAME_ERROR:
    case INVALID_DIRECTORY_NAME_ERROR:
    case DIRECTORY_NOT_EMPTY_ERROR:
    case FILE_DIRECTORY_SYNTACTIC_ERROR:
    case RMDIR_ONESELF_ERROR:
    case FIND_SPECIFIED_DRIVER_ERROR:
    case FILE_EXTENSION_LENGTH_ERROR:
    case FILE_LENGTH_ERROR:
    case CREATE_FILE_DIR_ERROR:
        m_err = 1;
        printf("%s\n", M_ERR_MSG[m_err_code].c_str());
        break;

    case INVALID_SWITCH_ERROR:
    case FIND_PATH_ERROR:
        m_err = 1;
        printf("%s%s\n", M_ERR_MSG[m_err_code].c_str(), m_err_msg.c_str());
        break;
        
    case CMD_ERROR:
    case PATH_LENGTH_ERROR:
    case PATH_EXIST_ERROR:
    case MEM_ALLOC_ERROR:
    case ACCESS_DENY_ERROR:
        m_err = 1;
        printf("%s%s\n", m_err_msg.c_str(), M_ERR_MSG[m_err_code].c_str());
        break;

    default:
        break;
    }
    m_err_code = 0;
    m_err_msg = M_ERR_MSG[0];
}