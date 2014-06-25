/**
 * Cmd基类及其命令子类实现
 */

#include "Cmd.h"
#include <stdio.h>

/* Cmd类函数 */
Cmd::Cmd(VirtualDisk *virtualDisk, MyString &cmdStr, int pos)
{
    m_virtual_disk = virtualDisk;
    m_cmd_str = cmdStr;
    m_param = NULL;
    m_cur_pos = pos;
}

void Cmd::readParam()
{
    int i = 0;
    int j = 0;
    int flag = 0; // 读取状态
    char strParam[100];
    while(i < m_cmd_str.size())
    {
        if(m_cmd_str[i] == ' ' || m_cmd_str[i] == '\t')
        {
            flag = 0;
        }
        else if(flag == 1 || m_cmd_str[i] == '/')
        {
            strParam[j++] = m_cmd_str[i];
            flag = 1;
        }
        ++i;
    }
    strParam[j] = '\0';
    m_param = strParam;
}

void Cmd::readPath(MyString &path)
{
    int i = m_cur_pos;
    int j = 0;
    int flag = 0; // 读取状态
    char strPath[10000];
    while(i < m_cmd_str.size())
    {        
        if(m_cmd_str[i] == ' ' || m_cmd_str[i] == '\t')
        {
            flag = 0;
            if(j != 0)
                break;
        }
        else if(flag == 1 || m_cmd_str[i] == '/')
        {
            flag = 1;
            if(j != 0)
                break;
        }
        else
        {
            strPath[j++] = m_cmd_str[i];
        }
        ++i;
    }
    strPath[j] = '\0';
    path = strPath;
    m_cur_pos = i;
}

void Cmd::readPathWithSpace(MyString &path)
{
    int i = m_cur_pos;
    int j = 0;
    int flag = 0; // 读取状态
    char strPath[10000];
    while(i < m_cmd_str.size())
    {
        if(m_cmd_str[i] == ' ' && j == 0)
        {
            flag = 0;
        }
        else if(m_cmd_str[i] == '\t')
        {
            flag = 0;
            if(j != 0)
                break;
        }
        else if(flag == 1 || m_cmd_str[i] == '/')
        {
            flag = 1;
            if(j != 0)
                break;
        }
        else
        {
            strPath[j++] = m_cmd_str[i];
        }
        ++i;
    }
    strPath[j] = '\0';
    for(int k = j - 1; k >= 0; --k)
    {
        if(strPath[k] == ' ')
            strPath[k] = '\0';
        else
            break;
    }
    path = strPath;
    m_cur_pos = i;
}

/* Cd类的Execute函数 */
void Cd::Execute()
{
    readParam();
    if(!(m_param == NULL))
    {
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_PATH_ERROR;
        ErrMsg::m_err_msg = m_param;
    }
    else
    {
        MyString tmpPath;
        readPath(m_path);
        readPath(tmpPath);

        if(tmpPath == NULL)
        {
            if(!m_virtual_disk->checkPath(m_path))
            {
                m_virtual_disk->Cd(m_path);
            }
            else if(ErrMsg::m_err_code == ErrMsg::PATH_LENGTH_ERROR)
            {
                ErrMsg::m_err_code = ErrMsg::FILE_EXTENSION_LENGTH_ERROR;
                ErrMsg::m_err_msg = m_path;
            }
        }
        else
        {
            ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_PATH_ERROR;
            ErrMsg::m_err_msg = m_param;
        }
    }
}

/* Compare类的Execute函数 */
void Compare::Execute()
{
    readParam();
    if(!(m_param == NULL))
    {
        ErrMsg::m_err_code = ErrMsg::CMD_SYNTACTIC_ERROR;
        ErrMsg::m_err_msg = m_param;
    }
    else
    {
        MyString path;
        readPath(m_real_path);
        readPath(m_virtual_path);
        readPath(path);
        if(m_virtual_path == NULL || m_real_path == NULL || (!(path == NULL)))
        {
            ErrMsg::m_err_code = ErrMsg::CMD_SYNTACTIC_ERROR;
            ErrMsg::m_err_msg = m_param;
        }
        else
        {
            if(!m_virtual_disk->checkPath(m_virtual_path))
            {
                m_virtual_disk->Compare(m_real_path, m_virtual_path);
            }
            else if(ErrMsg::m_err_code == ErrMsg::PATH_LENGTH_ERROR)
            {
                ErrMsg::m_err_code = ErrMsg::FILE_LENGTH_ERROR;
                ErrMsg::m_err_msg = m_virtual_path;
            }
        }
    }
}

/* Copy类的Execute函数 */
void Copy::Execute()
{
    readParam();
    if(!(m_param == NULL))
    {
        ErrMsg::m_err_code = ErrMsg::CMD_SYNTACTIC_ERROR;
        ErrMsg::m_err_msg = m_param;
    }
    else
    {
        MyString path;
        readPath(m_real_path);
        readPath(m_virtual_path);
        readPath(path);
        if(m_real_path == NULL || (!(path == NULL)))
        {
            ErrMsg::m_err_code = ErrMsg::CMD_SYNTACTIC_ERROR;
            ErrMsg::m_err_msg = m_param;
        }
        else
        {
            if(!m_virtual_disk->checkPath(m_virtual_path))
            {
                m_virtual_disk->Copy(m_real_path, m_virtual_path);
            }
            else if(ErrMsg::m_err_code == ErrMsg::PATH_LENGTH_ERROR)
            {
                ErrMsg::m_err_code = ErrMsg::FILE_LENGTH_ERROR;
                ErrMsg::m_err_msg = m_virtual_path;
            }
        }
    }
}

/* Del类的Execute函数 */
void Del::Execute()
{
    readParam();
    int flag = 0;

    int i = 0;
    while(i < m_param.size())
    {
        if(m_param[i] == '/')
        {
            if(m_param[i + 1] == 's')
            {
                flag = 1;
                i += 2;
            }
            else
            {
                ErrMsg::m_err_code = ErrMsg::INVALID_SWITCH_ERROR;
                ErrMsg::m_err_msg = m_param;
                break;
            }
        }
        else
        {
            ErrMsg::m_err_code = ErrMsg::INVALID_SWITCH_ERROR;
            ErrMsg::m_err_msg = m_param;
            break;
        }
    }

    if(!ErrMsg::m_err_code)
    {
        int numPath = 0;
        do
        {
            readPath(m_path);
            if(!(m_path == NULL))
            {
                if(!m_virtual_disk->checkPath(m_path))
                {
                    if(flag == 0)
                        m_virtual_disk->Del(m_path, false);
                    else if(flag == 1)
                        m_virtual_disk->Del(m_path, true);
                }
                else if(ErrMsg::m_err_code == ErrMsg::PATH_LENGTH_ERROR)
                {
                    ErrMsg::m_err_code = ErrMsg::FILE_EXTENSION_LENGTH_ERROR;
                    ErrMsg::m_err_msg = m_path;
                }
            }
            else if(numPath == 0)
            {
                ErrMsg::m_err_code = ErrMsg::CMD_SYNTACTIC_ERROR;
                ErrMsg::m_err_msg = m_param;
            }
            ++numPath;
            ErrMsg::printErr();
        } while(m_cur_pos < m_cmd_str.size());
    }
}

/* Dir类的Execute函数 */
void Dir::Execute()
{
    readParam();
    int flag = 0;

    int i = 0;
    while(i < m_param.size())
    {
        if(m_param[i] == '/')
        {
            if(m_param[i + 1] == 's')
            {
                if(flag != 1 && flag != 3)
                    flag += 1;
                i += 2;
            }
            else if(m_param[i + 1] == 'a' && m_param[i + 2] == 'd')
            {
                if(flag != 2 && flag != 3)
                    flag += 2;
                i += 3;
            }
            else
            {                
                ErrMsg::m_err_code = ErrMsg::INVALID_SWITCH_ERROR;
                ErrMsg::m_err_msg = m_param;
                break;
            }
        }
        else
        {
            ErrMsg::m_err_code = ErrMsg::INVALID_SWITCH_ERROR;
            ErrMsg::m_err_msg = m_param;
            break;
        }
    }

    if(!ErrMsg::m_err_code)
    {
        int numPath = 0;
        do
        {
            readPath(m_path);
            if(!m_virtual_disk->checkPath(m_path))
            {
                if(numPath == 0 || (m_path != NULL))
                {
                    if(flag == 0)
                        m_virtual_disk->Dir(m_path, Type_Dir_File, false);
                    else if(flag == 1)
                        m_virtual_disk->Dir(m_path, Type_Dir_File, true);
                    else if(flag == 2)
                        m_virtual_disk->Dir(m_path, Type_Dir, false);
                    else
                        m_virtual_disk->Dir(m_path, Type_Dir, true);
                }
            }
            else if(ErrMsg::m_err_code == ErrMsg::PATH_LENGTH_ERROR)
            {
                ErrMsg::m_err_code = ErrMsg::FILE_EXTENSION_LENGTH_ERROR;
                ErrMsg::m_err_msg = m_path;
            }
            ++numPath;
            ErrMsg::printErr();
        } while(m_cur_pos < m_cmd_str.size());
    }
}

/* Mkdir类的Execute函数 */
void Mkdir::Execute()
{
    readParam();
    if(!(m_param == NULL))
    {
        ErrMsg::m_err_code = ErrMsg::CMD_SYNTACTIC_ERROR;
        ErrMsg::m_err_msg = m_param;
    }
    else
    {
        int numPath = 0;
        do
        {
            readPath(m_path);
            if(!(m_path == NULL))
            {
                if(!m_virtual_disk->checkPath(m_path))
                {
                    if(m_path.find('*') || m_path.find('?'))
                    {
                        ErrMsg::m_err_code = ErrMsg::INVALID_PATH_NAME_ERROR;
                        ErrMsg::m_err_msg = m_path;
                    }
                    else
                        m_virtual_disk->Mkdir(m_path);
                }
            }
            else if(numPath == 0)
            {
                ErrMsg::m_err_code = ErrMsg::CMD_SYNTACTIC_ERROR;
                ErrMsg::m_err_msg = m_param;
            }
            ++numPath;
            ErrMsg::printErr();
        } while(m_cur_pos < m_cmd_str.size());
    }
}

/* Rmdir类的Execute函数 */
void Rmdir::Execute()
{
    readParam();
    int flag = 0;

    int i = 0;
    while(i < m_param.size())
    {
        if(m_param[i] == '/')
        {
            if(m_param[i + 1] == 's')
            {
                flag = 1;
                i += 2;
            }
            else
            {
                ErrMsg::m_err_code = ErrMsg::INVALID_SWITCH_ERROR;
                ErrMsg::m_err_msg = m_param;
                break;
            }
        }
        else
        {
            ErrMsg::m_err_code = ErrMsg::INVALID_SWITCH_ERROR;
            ErrMsg::m_err_msg = m_param;
            break;
        }
    }

    if(!ErrMsg::m_err_code)
    {
        int numPath = 0;
        do
        {
            readPath(m_path);
            if(!(m_path == NULL))
            {
                if(!m_virtual_disk->checkPath(m_path))
                {
                    if(flag == 0)
                        m_virtual_disk->Rmdir(m_path, false);
                    else if(flag == 1)
                        m_virtual_disk->Rmdir(m_path, true);
                }
                else if(ErrMsg::m_err_code == ErrMsg::PATH_LENGTH_ERROR)
                {
                    ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_PATH_ERROR;
                    ErrMsg::m_err_msg = m_path;
                }
            }
            else if(numPath == 0)
            {
                ErrMsg::m_err_code = ErrMsg::CMD_SYNTACTIC_ERROR;
                ErrMsg::m_err_msg = m_param;
            }
            ++numPath;
            ErrMsg::printErr();
        } while(m_cur_pos < m_cmd_str.size());
    }
}

void SwitchVolume::Execute()
{
    MyString cmdStr(m_cmd_str, 0, 2);
    m_virtual_disk->SwitchVolume(cmdStr);
    ErrMsg::printErr();
}

/* Factory类函数 */
const int Factory::MAX_CMD_LEN = 10;
const int Factory::NUM_CMD = 9;
const MyString Factory::c_cmd_str[] = { "cd", "compare", "copy", "del", "dir", "mkdir", "rmdir", "exit" };

Cmd *Factory::createCmd(VirtualDisk *virtualDisk, MyString &str)
{
    int pos = 0;
    int cmdCode = checkCmd(str, pos);

    switch(cmdCode)
    {
    case CD:
        return new Cd(virtualDisk, str, pos);
    case COMPARE:
        return new Compare(virtualDisk, str, pos);
    case COPY:
        return new Copy(virtualDisk, str, pos);
    case DEL:
        return new Del(virtualDisk, str, pos);
    case DIR:
        return new Dir(virtualDisk, str, pos);
    case MKDIR:
        return new Mkdir(virtualDisk, str, pos);
    case RMDIR:
        return new Rmdir(virtualDisk, str, pos);
    case EXIT:
        return NULL;
    case SWITCH_VOLUME:
        return new SwitchVolume(virtualDisk, str, pos);
    default:
        ErrMsg::m_err_code = ErrMsg::CMD_ERROR;
        ErrMsg::m_err_msg = str;
        return NULL;
    }
}

int Factory::checkCmd(MyString &str, int &pos)
{
    char cmd_char[MAX_CMD_LEN + 1];
    
    int i = 0;
    int j = 0;
    while(i < str.size() && i < MAX_CMD_LEN)
    {
        if(str[i] == '.' || str[i] == '\\' || str[i] == '/' || ((str[i] == '\t' || str[i] == ' ') && j != 0))
        {
            break;
        }
        else if(str[i] != ' ' && str[i] != '\t')
        {
            cmd_char[j++] = str[i];
        }
        ++i;
    }
    cmd_char[j++] = '\0';
    pos = i;

    MyString cmdStr = cmd_char;
    int retCode = -1;
    for(int i = 0; i < NUM_CMD - 1; ++i)
    {
        if(cmdStr == c_cmd_str[i])
        {
            retCode = i;
            break;
        }
    }
    if((str.size() == 2 && str[1] == ':') || (str.size() > 2 && str[1] == ':' && str[2] == ' '))
        retCode = NUM_CMD - 1;
    return retCode;
}

int VirtualDisk::ExecCommand(const char *command)
{
    static Factory factory;
    MyString cmdStr = command;
    Cmd *cmd = factory.createCmd(this, cmdStr);
    if(cmd == NULL)
    {
        if(!ErrMsg::m_err_code)
        {
            delete cmd;
            cmd = NULL;
            return -1;
        }
    }
    else
    {
        cmd->Execute();
    }
    delete cmd;
    cmd = NULL;
    ErrMsg::printErr();
    if(ErrMsg::m_err != 0)
    {
        ErrMsg::m_err = 0;
        showPath(m_cur_volume);
        return 0;
    }
    else
    {
        showPath(m_cur_volume);
        return 1;
    }
}
