/**
 * VirtualDisk类、Volume类、FindResult及FileNode类定义
 * 完成文件系统结构的定义
 */

#ifndef FILESYS_H
#define FILESYS_H

#define DLL_IMPLEMENT

#include <stdio.h>
#include "virtual-disk.h"
#include "MyString.h"
#include "ErrMsg.h"
#include <string.h>
#include <time.h>

enum FType
{
    Type_Dir,
    Type_File,
    Type_Dir_File
};

/**
 * 定义路径节点基类
 */
class FindResult;

class FileNode
{
public:
    FileNode();
    FileNode(MyString &name, time_t time);
    virtual ~FileNode();

    virtual bool isLeaf() const = 0;

    virtual void setParent(FileNode *parent) { m_parent = parent; }
    virtual void setBrother(FileNode *brother) { m_brother = brother; }
    virtual void setChild(FileNode *child) {}

    virtual void setName(MyString &name) { m_name = name; }
    virtual void setTime(time_t time) { m_time = time; }

    virtual FileNode *getParent() const { return m_parent; }
    virtual FileNode *getBrother() const { return m_brother; }
    virtual FileNode *getChild() const { return NULL; };

    virtual MyString getName() const { return m_name; }
    virtual time_t getTime() const { return m_time; }

    virtual void setFile(char *file_data, unsigned long size) {}
    virtual char *getFileData() const { return NULL; }
    virtual unsigned long getSize() const { return 0; }


    virtual void getPathStr(MyString &path);

    virtual void del(FileNode *node, int &numFile, int &numDir);  // 删除当前节点的子节点node
    virtual void del(MyString &name, FType type, bool bRecursion, int &numFile, int &numDir);  // 删除节点
    
    virtual void dir(int &numFile, int &numDir);  // 打印当前节点
    virtual void dir(MyString &name, FType type, bool bRecursion, int &numFile, int &numDir);  // 打印节点

    virtual FileNode *find(MyString &name, FType type, bool isWildSupport);  // 查找当前节点下是否存在名为name的节点

    virtual FileNode *create(MyString &name, time_t t = NULL, char *data = NULL, unsigned long size = 0);  // 创建节点，并返回创建的节点

    virtual FindResult *FindStr(MyString &findName, bool bRecursion);  // 查找字符串函数

protected:

    FileNode *m_parent;
    FileNode *m_brother;
    
    MyString m_name;
    time_t m_time;
};

/**
 * FindResult类定义：单例模式
 */
class FindResult : public IFindResult
{
public:
    static FindResult *getInstance();

    void setFileNode(FileNode *fileNode);
    void resize(int size);

    // 导出接口
    virtual unsigned int GetCount() { return m_count; }
    virtual void Release();
    virtual IDiskObj *GetItemByIdx(unsigned int idx);

private:
	FindResult();
    ~FindResult();
    static const unsigned int INIT_SIZE;
    unsigned int m_count;  // 存储多少个节点
    unsigned int m_size;  // 存储空间大小
    FileNode **m_file_node;

    static FindResult *m_find_result;
    class Garbo
    {
    public:
        ~Garbo()
        {
            if(FindResult::m_find_result != NULL)
            {
                delete FindResult::m_find_result;
                FindResult::m_find_result = NULL;
            }
        }
    };
    static Garbo garbo;
};

/**
 * 定义文件节点，继承自FileNode
 */
class LeafNode : public FileNode, public IFileProxy
{
public:
    LeafNode();
    LeafNode(MyString &name, time_t time);
    LeafNode(MyString &name, time_t time, char *file_data, int size);
    ~LeafNode();

    virtual bool isLeaf() const { return true; }
    
    virtual void setFile(char *file_data, unsigned long size);
    virtual char *getFileData() const { return m_file_data; }
    virtual unsigned long getSize() const { return m_size; }

    // 导出接口
    virtual const char *GetName() { return m_name.c_str(); }
    virtual ETYPE GetType() { return IFile; }

private:
    char *m_file_data;
    unsigned long m_size;
};

/**
 * 定义目录节点，继承自FileNode
 */
class BranchNode : public FileNode, public IDirProxy
{
public:
    BranchNode();
    BranchNode(MyString &name, time_t time);
    ~BranchNode();

    virtual bool isLeaf() const { return false; }
    
    virtual void setChild(FileNode *child) { m_child = child; }
    virtual FileNode *getChild() const { return m_child; }

    // 导出接口
    virtual const char *GetName() { return m_name.c_str(); }
    virtual ETYPE GetType() { return IDir; }
    virtual IFindResult *Find(const char *findstr, bool bRecursion);

private:
    FileNode *m_child;
};

/**
 * 定义真正磁盘文件信息
 * 供copy命令使用
 */
struct FileInfo
{
    MyString m_path;
    MyString m_name;
    unsigned long m_size;
    time_t m_time;
};

/**
 * 定义真正磁盘文件列表
 * 供copy命令使用
 */
class FileList
{
public:
    FileList();
    ~FileList();

    void add(MyString &path, MyString &name, unsigned long size, time_t time);
    MyString &getPathByIdx(int idx) const { return m_file_info[idx].m_path; }
    MyString &getNameByIdx(int idx) const { return m_file_info[idx].m_name; }
    unsigned long getSizeByIdx(int idx) const { return m_file_info[idx].m_size; }
    time_t getTimeByIdx(int idx) { return m_file_info[idx].m_time; }
    int getCount() const { return m_count; }

private:
    void resize(int size);

    static const int INIT_SIZE;
    int m_size;
    int m_count;
    FileInfo *m_file_info;
};

/**
 * 定义标卷
 */
class Volume : public IVolumnProxy
{
public:
    Volume();
    Volume(MyString &name);
    ~Volume();

    FileNode *getNodeByStr(MyString &path, FileNode *&parentNode, int &pos);  // 通过路径查节点
    int createNode(MyString &path, int startPos);  // 根据路径新建节点及其子节点

    FileNode *getCurDir() { return m_cur_node; }
    FileNode *getRootDir() { return m_root_node; }
    void setCurDir(FileNode *cur_dir) { m_cur_node = cur_dir; }

    // 导出接口
    virtual IDirProxy *GetRootDir() { return dynamic_cast<BranchNode *>(m_root_node); }

private:
    FileNode *m_root_node;
    FileNode *m_cur_node;
};

/**
 * 定义虚拟磁盘
 */
class VirtualDisk : public IVirtualDiskProxy
{
public:
    static VirtualDisk *getInstance(int numVolume);

    int Cd(MyString &path);  // 转换路径
    int Compare(MyString &realPath, MyString &virtualPath);  // 比较两个文件是否相等
    int Dir(MyString &path, FType type, bool bRecursion);  // dir命令
    int Rmdir(MyString &path, bool bRecursion);  // 删除路径
    int Mkdir(MyString &path);  // 创建路径
    int Del(MyString &path, bool bRecursion);  // 删除文件
    int Copy(MyString &realPath, MyString &virtualPath);  // 拷贝文件
    int SwitchVolume(MyString &path);  // 标卷转换

    int checkPath(MyString &path);  // 检查输入路径是否合法

    int copyToOne(FileNode *node, FileList &fileList, MyString &name);  // 拷贝多个文件为一个文件
    int copyFile(FileNode *node, FileList &fileList, MyString &name);  // 拷贝多个文件为多个文件

    void showPath(Volume *volume);  // 打印当前路径
    int readFile(char *fileData, MyString &path, unsigned long len);  // 从实际磁盘读取文件
    bool isBinary(char *buf, unsigned long len);  // 判断文件是否是二进制文件
    void printNotMatch(unsigned char *buf, unsigned long pos, bool isBinary);  // Compare函数发生不匹配时，打印信息
    void getDirAndFile(MyString &realPath, MyString &dir, MyString &file);  // 获取倒数第二层路径（即目录）
    bool existDir(MyString &path, int start);  // 判断path[start]后面是否存在目录
    int findRealFile(MyString &realPath, FileList &fileList, bool bRecursion);  // 在真是磁盘中查找文件，保存在fileList中
    MyString &genName(MyString &realName, MyString &virtualName, MyString &retName);  // copy时声称虚拟磁盘下文件的名字

    Volume *getVolumeByStr(MyString &path, int &pos);  // 通过路径查找标卷
    Volume *getCurVolume() { return m_cur_volume; }
    Volume *getVolumeByIdx(int idx) { if(idx < 0 || idx >= m_count) return NULL; else return m_volume[idx]; }

    // 导出接口
    virtual int ExecCommand(const char *command); //返回1表示成功，0表示失败
    virtual int GetVolumnCount() { return m_count; }
    virtual IVolumnProxy *GetVolumnByIdx(int idx) { if(idx < 0 || idx >= m_count) return NULL; else return m_volume[idx]; }

private:
    static const int MAX_COUNT;
    static const int COMPARE_PRINT_LEN;
    static const int MAX_PATH_NAME_LEN;
    int m_count;
    Volume *m_cur_volume;
    Volume **m_volume;

private:
    VirtualDisk(int count);
    ~VirtualDisk();

    static VirtualDisk *m_virtual_disk;
    class Garbo
    {
    public:
        ~Garbo()
        {
            if(VirtualDisk::m_virtual_disk != NULL)
            {
                delete VirtualDisk::m_virtual_disk;
                VirtualDisk::m_virtual_disk = NULL;
            }
        }
    };
    static Garbo garbo;
};

#endif