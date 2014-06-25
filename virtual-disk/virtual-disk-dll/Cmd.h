/**
 * ����Cmd���༰����������
 */
#ifndef CMD_H
#define CMD_H

#include "MyString.h"
#include "FileSys.h"

/**
 * �����������
 */
class Cmd
{
public:
    Cmd(VirtualDisk *virtualDisk, MyString &cmdStr, int pos);
    virtual ~Cmd() { m_virtual_disk = NULL; }

    virtual void Execute() = 0;
    
    virtual void readParam();
    virtual void readPath(MyString &path);
    virtual void readPathWithSpace(MyString &path);

protected:
    int m_cur_pos;
    MyString m_cmd_str;
    MyString m_param;
    VirtualDisk *m_virtual_disk;
};

class Cd : public Cmd
{
public:
    Cd(VirtualDisk *virtualDisk, MyString &cmdStr, int pos) : Cmd(virtualDisk, cmdStr, pos) {}
    
    virtual void Execute();

private:
    MyString m_path;
};

class Compare : public Cmd
{
public:
    Compare(VirtualDisk *virtualDisk, MyString &cmdStr, int pos) : Cmd(virtualDisk, cmdStr, pos) {}
    
    virtual void Execute();

private:
    MyString m_real_path;
    MyString m_virtual_path;
};

class Copy : public Cmd
{
public:
    Copy(VirtualDisk *virtualDisk, MyString &cmdStr, int pos) : Cmd(virtualDisk, cmdStr, pos) {}
    
    virtual void Execute();

private:
    MyString m_real_path;
    MyString m_virtual_path;
};

class Del : public Cmd
{
public:
    Del(VirtualDisk *virtualDisk, MyString &cmdStr, int pos) : Cmd(virtualDisk, cmdStr, pos) {}
    
    virtual void Execute();

private:
    MyString m_path;
};

class Dir : public Cmd
{
public:
    Dir(VirtualDisk *virtualDisk, MyString &cmdStr, int pos) : Cmd(virtualDisk, cmdStr, pos) {}
    
    virtual void Execute();

private:
    MyString m_path;
};

class Mkdir : public Cmd
{
public:
    Mkdir(VirtualDisk *virtualDisk, MyString &cmdStr, int pos) : Cmd(virtualDisk, cmdStr, pos) {}
    
    virtual void Execute();

private:
    MyString m_path;
};

class Rmdir : public Cmd
{
public:
    Rmdir(VirtualDisk *virtualDisk, MyString &cmdStr, int pos) : Cmd(virtualDisk, cmdStr, pos) {}
    
    virtual void Execute();

private:
    MyString m_path;
};

class SwitchVolume : public Cmd
{
public:
    SwitchVolume(VirtualDisk *virtualDisk, MyString &cmdStr, int pos) : Cmd(virtualDisk, cmdStr, pos) {}
    
    virtual void Execute();
};

class Factory 
{
public:
    Factory() {}
    ~Factory() {}
    
    Cmd *createCmd(VirtualDisk *virtualDisk, MyString &str);  // �����������
    int checkCmd(MyString &str, int &pos);  // �������

    enum
    {
        CD,
        COMPARE,
        COPY,
        DEL,
        DIR,
        MKDIR,
        RMDIR,
        EXIT,
        SWITCH_VOLUME
    };
private:
    static const int MAX_CMD_LEN;  // cmd��󳤶�
    static const int NUM_CMD;  // ֧��cmd����
    static const MyString c_cmd_str[];  // ֧�ֵ������ַ���
};

#endif