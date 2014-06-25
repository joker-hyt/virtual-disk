/**
* VirtualDisk类，Volume类及FileNode类实现
*/

#include "FileSys.h"
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <new>

IVirtualDiskProxy *GetVirtualDiskProxy()
{
    IVirtualDiskProxy *virtual_disk = VirtualDisk::getInstance(2);
    return virtual_disk;
}

/* FindResult函数 */
FindResult *FindResult::m_find_result = NULL;
FindResult::Garbo FindResult::garbo;
const unsigned int FindResult::INIT_SIZE = 3;
FindResult *FindResult::getInstance()
{
    if(m_find_result == NULL)
        m_find_result = new FindResult;
    return m_find_result;
}
FindResult::FindResult()
{
    m_size = INIT_SIZE;
    m_count = 0;
    m_file_node = (FileNode **)malloc(m_size*sizeof(FileNode *));
    for(unsigned int i = 0; i < m_size; ++i)
    {
        m_file_node[i] = NULL;
    }
}
FindResult::~FindResult()
{
    if(m_file_node != NULL)
    {
        for(unsigned int i = 0; i < m_size; ++i)
        {
            m_file_node[i] = NULL;
        }
        free(m_file_node);
        m_file_node = NULL;
    }
    m_count = 0;
    m_size = 0;
}
void FindResult::Release()
{
    if(m_file_node != NULL)
    {
        for(unsigned int i = 0; i < m_count; ++i)
        {
            m_file_node[i] = NULL;
        }
    }
    m_count = 0;
}
IDiskObj *FindResult::GetItemByIdx(unsigned int idx)
{
    if(idx < 0 || idx >= m_count)
        return NULL;
    else
    {
        if(m_file_node[idx]->isLeaf())
            return dynamic_cast<LeafNode *>(m_file_node[idx]);
        else
            return dynamic_cast<BranchNode *>(m_file_node[idx]);
    }
}
void FindResult::resize(int size)
{
    FileNode **fileNode = m_file_node;
    m_size += size;

    m_file_node = (FileNode **)malloc(m_size*sizeof(FileNode *));
    for(unsigned int i = 0; i < m_count; ++i)
        m_file_node[i] = fileNode[i];
    for(unsigned int i = m_count; i < m_size; ++i)
        m_file_node[i] = NULL;

    for(unsigned int i = 0; i < m_count; ++i)
        fileNode[i] = NULL;
    free(fileNode);
    fileNode = NULL;
}
void FindResult::setFileNode(FileNode *fileNode)
{
    if(m_count == m_size)
        resize(INIT_SIZE);
    m_file_node[m_count++] = fileNode;
}

/* FileNode函数 */
FileNode::FileNode()
{
    m_name = NULL;
    m_time = NULL;
    m_parent = NULL;
    m_brother = NULL;
}

FileNode::FileNode(MyString &name, time_t time)
{
    m_name = name;
    m_time = time;
    m_parent = NULL;
    m_brother = NULL;
}

FileNode::~FileNode()
{
    m_name = NULL;
    m_time = NULL;
    m_parent = NULL;
    m_brother = NULL;
}

FindResult *FileNode::FindStr(MyString &findName, bool bRecursion)
{
    FindResult *findResult = FindResult::getInstance();

    if(bRecursion)
    {
        for(FileNode *p = getChild(); p != NULL; p = p->getBrother())
        {
            if(findName.wildCmp(p->getName()))
                findResult->setFileNode(p);
            if(!p->isLeaf())
            {
                p->FindStr(findName, bRecursion);
            }
        }
    }
    else
    {
        for(FileNode *p = getChild(); p != NULL; p = p->getBrother())
        {
            if(findName.wildCmp(p->getName()))
                findResult->setFileNode(p);
        }
    }

    return findResult;
}

void FileNode::getPathStr(MyString &path)
{
    FileNode *parent = getParent();
    if(parent != NULL)
        parent->getPathStr(path);
    if(path != NULL)
        path += "\\";
    path += getName();
}

void FileNode::del(FileNode *node, int &numFile, int &numDir)
{
    if(node == NULL)
        return ;

    if(getChild() == node)
    {
        if(node->getChild() == NULL)
        {
            if(node->isLeaf())
                ++numFile;
            else
                ++numDir;
            setChild(node->getBrother());
            delete node;
        }
    }
    else
    {
        FileNode *brother = getChild();
        while(brother->getBrother() != NULL && brother->getBrother() != node)
            brother = brother->getBrother();

        if(brother->getBrother() != NULL)
        {
            if(node->getChild() == NULL)
            {
                if(node->isLeaf())
                    ++numFile;
                else
                    ++numDir;
                brother->setBrother(node->getBrother());
                delete node;
            }
        }
    }
}
void FileNode::del(MyString &name, FType type, bool bRecursion, int &numFile, int &numDir)
{
    if(bRecursion)
    {
        for(FileNode *p = getChild(); p != NULL; )
        {
            // 后根遍历
            FileNode *tmp = p->getBrother();
            if(!p->isLeaf())
                p->del(name, type, bRecursion, numFile, numDir);
            if(((type == Type_Dir && !p->isLeaf())
                || (type == Type_File && p->isLeaf())
                || (type == Type_Dir_File))
                && name.wildCmp(p->getName()))
            {
                if(bRecursion && type == Type_File)
                {
                    MyString path;
                    p->getPathStr(path);
                    printf("删除文件 - %s\n", path.c_str());
                }
                del(p, numFile, numDir);
            }
            p = tmp;
        }
    }
    else
    {
        for(FileNode *p = getChild(); p != NULL; )
        {
            FileNode *tmp = p->getBrother();
            if(((type == Type_Dir && !p->isLeaf())
                || (type == Type_File && p->isLeaf())
                || (type == Type_Dir_File))
                && name.wildCmp(p->getName()))
                del(p, numFile, numDir);
            p = tmp;
        }
    }
}

void FileNode::dir(int &numFile, int &numDir)
{
    char tmp[64];
    time_t t = getTime();

    strftime(tmp, sizeof(tmp), "%Y/%m/%d", localtime(&t));
    MyString date = tmp;

    strftime(tmp, sizeof(tmp), "%H:%M ", localtime(&t)); 
    MyString time = tmp;

    if(isLeaf())
    {
        const int formatLen = 17;
        char sizeFormat[formatLen];
        sprintf(sizeFormat, "%d", getSize());
        int len = strlen(sizeFormat);

        int i, j;
        sizeFormat[formatLen - 1] = '\0';
        for(i = len - 1, j = formatLen - 2; i >= 0 && j >= 0; --i)
        {
            if((len - 1 - i)%3 == 0 && i != len - 1)
                sizeFormat[j--] = ',';
            sizeFormat[j--] = sizeFormat[i];
        }
        for(j = j; j >= 0; --j)
            sizeFormat[j] = ' ';


        printf("%s  %s%s %s\n", date.c_str(), time.c_str(),
            sizeFormat, getName().c_str());
        ++numFile;
    }
    else
    {
        printf("%s  %s   <DIR>         %s\n", date.c_str(), time.c_str(),
            getName().c_str());
        ++numDir;
    }
}
void FileNode::dir(MyString &name, FType type, bool bRecursion, int &numFile, int &numDir)
{
    if(bRecursion)
    {
        // 先根遍历
        dir(name, type, false, numFile, numDir);
        for(FileNode *p = getChild(); p != NULL; p = p->getBrother())
        {
            if(!p->isLeaf())
                p->dir(name, type, bRecursion, numFile, numDir);
        }
    }
    else
    {
        int tmpNumFile = 0;
        int tmpNumDir = 0;
        MyString curPath;
        getPathStr(curPath);
        printf("\n %s 的目录\n\n", curPath.c_str());
        for(FileNode *p = getChild(); p != NULL; p = p->getBrother())
        {
            if(((type == Type_Dir && !p->isLeaf())
                || (type == Type_File && p->isLeaf())
                || (type == Type_Dir_File))
                && name.wildCmp(p->getName()))
                p->dir(tmpNumFile, tmpNumDir);
        }
        printf("             %d 个文件\n", tmpNumFile);
        printf("             %d 个目录\n", tmpNumDir);
        numFile += tmpNumFile;
        numDir += tmpNumDir;
    }
}

FileNode *FileNode::find(MyString &name, FType type, bool isWildSupport)
{
    if(type == Type_Dir || type == Type_Dir_File)
    {
        if(name == ".")
        {
            return this;
        }
        else if(name == "..")
        {
            if(getParent() != NULL)
                return getParent();
            return this;
        }
    }
    for(FileNode *p = getChild(); p != NULL; p = p->getBrother())
    {
        if(((type == Type_Dir && !p->isLeaf()) || (type == Type_File && p->isLeaf()) || (type == Type_Dir_File))
            && ((isWildSupport && name.wildCmp(p->getName())) || (!isWildSupport && name == p->getName())))
            return p;
    }
    return NULL;
}
FileNode *FileNode::create(MyString &name, time_t t, char *data, unsigned long size)
{
    // 获取时间信息
    if(t == NULL)
        t = time(0);

    FileNode *newNode = NULL;
    if(data == NULL)
        newNode = new BranchNode(name, t);  // 创建Branch
    else
        newNode = new LeafNode(name, t, data, size);  // 创建Leaf
    newNode->setParent(this);
    if(getChild() == NULL)
        setChild(newNode);
    else
    {
        FileNode *p = getChild();
        for(; p->getBrother() != NULL; p = p->getBrother());
        p->setBrother(newNode);
    }
    return newNode;
}

/* LeafNode函数 */
LeafNode::LeafNode() : FileNode()
{
    m_file_data = NULL;
    m_size = 0;
}

LeafNode::LeafNode(MyString &name, time_t time)
    : FileNode(name, time)
{
    m_file_data = NULL;
    m_size = 0;
}

LeafNode::LeafNode(MyString &name, time_t time,
    char *file_data, int size) : FileNode(name, time)
{
    m_file_data = file_data;
    m_size = size;
}

LeafNode::~LeafNode()
{
    delete []m_file_data;
    m_file_data = NULL;
    m_size = 0;
}

void LeafNode::setFile(char *file_data, unsigned long size)
{
    if(m_file_data != NULL)
        delete []m_file_data;
    m_file_data = file_data;
    m_size = size;
}

/* BranchNode函数 */
BranchNode::BranchNode() : FileNode()
{
    m_child = NULL;
}

BranchNode::BranchNode(MyString &name, time_t time)
    :FileNode(name, time)
{
    m_child = NULL;
}

BranchNode::~BranchNode()
{
    m_child = NULL;
}

IFindResult *BranchNode::Find(const char *findstr, bool bRecursion)
{
    // 只支持文件查找
    MyString findName = findstr;
    FindResult *findResult = FindStr(findName, bRecursion);
    return findResult;
}

/* 节点链表结构 */
typedef struct fileNodeNode
{
    FileNode *node;
    struct fileNodeNode *next;
} FileNodeNode;


/* 保存节点的栈结构：mkdir时使用 */
typedef struct fileNodeStack
{
    FileNodeNode *top;
    FileNodeNode *bottom;
} FileNodeStack;
/* 初始化栈 */
void initStack(FileNodeStack *stack)
{
    stack->top = (FileNodeNode *)malloc(sizeof(FileNodeNode));
    stack->bottom = stack->top;
    stack->top->node = NULL;
    stack->top->next = NULL;
}
/* 判断栈是否为空 */
bool isEmptyStack(FileNodeStack *stack)
{
    if(stack->top == stack->bottom)
        return true;
    else
        return false;
}
/* 压栈 */
void pushStack(FileNodeStack *stack, FileNode *node)
{
    FileNodeNode *newNode = (FileNodeNode *)malloc(sizeof(FileNodeNode));
    newNode->node = node;
    newNode->next = stack->top;
    stack->top = newNode;
}
/* 弹栈 */
FileNode *popStack(FileNodeStack *stack)
{
    if(!isEmptyStack(stack))
    {
        FileNodeNode *tmp = stack->top;
        stack->top = tmp->next;
        FileNode *retNode = tmp->node;
        tmp->node = NULL;
        tmp->next = NULL;
        free(tmp);
        tmp = NULL;
        return retNode;
    }
    return NULL;
}
/* 释放栈 */
void destroyStack(FileNodeStack *stack)
{
    FileNodeNode *q = NULL;
    while(stack->top != stack->bottom)
    {
        q = stack->top;
        stack->top = q->next;
        q->node = NULL;
        q->next = NULL;
        free(q);
        q = NULL;
    }
    free(stack->top);
    stack->top = NULL;
    stack->bottom = NULL;
}

/* FileList函数 */
const int FileList::INIT_SIZE = 3;
FileList::FileList()
{
    m_count = 0;
    m_size = INIT_SIZE;
    m_file_info = new FileInfo[m_size];
    for(int i = 0; i < m_size; ++i)
    {
        m_file_info[i].m_path = NULL;
        m_file_info[i].m_name = NULL;
        m_file_info[i].m_size = 0;
        m_file_info[i].m_time = NULL;
    }
}
FileList::~FileList()
{
    if(m_file_info != NULL)
    {
        for(int i = 0; i < m_size; ++i)
        {
            m_file_info[i].m_path = NULL;
            m_file_info[i].m_name = NULL;
            m_file_info[i].m_size = 0;
            m_file_info[i].m_time = NULL;
        }
        delete [] m_file_info; m_file_info = NULL;
    }
    m_count = 0;
    m_size = 0;
}
void FileList::add(MyString &pathName, MyString &name, unsigned long size, time_t time)
{
    if(m_count == m_size)
        resize(INIT_SIZE);
    m_file_info[m_count].m_path = pathName;
    m_file_info[m_count].m_name = name;
    m_file_info[m_count].m_size = size;
    m_file_info[m_count].m_time = time;
    ++m_count;
}
void FileList::resize(int size)
{
    FileInfo *fileInfo = m_file_info;
    
    m_size += size;
    m_file_info = new FileInfo[m_size];
    for(int i = 0; i < m_count; ++i)
    {
        m_file_info[i].m_path = fileInfo[i].m_path;
        m_file_info[i].m_name = fileInfo[i].m_name;
        m_file_info[i].m_size = fileInfo[i].m_size;
        m_file_info[i].m_time = fileInfo[i].m_time;
        fileInfo[i].m_path = NULL;
        fileInfo[i].m_name = NULL;
        fileInfo[i].m_size = 0;
        fileInfo[i].m_time = NULL;
    }
    for(int i = m_count; i < m_size; ++i)
    {
        m_file_info[i].m_path = NULL;
        m_file_info[i].m_name = NULL;
        m_file_info[i].m_size = 0;
        m_file_info[i].m_time = NULL;
    }
    delete []fileInfo; fileInfo = NULL;
}

/* Volume函数 */
Volume::Volume()
{
    MyString name = "C:";
    m_root_node = new BranchNode();  // 根目录创建日期和时间为NULL
    m_root_node->setName(name);
    m_cur_node = m_root_node;
}
Volume::Volume(MyString &name)
{
    MyString volumeName = name;
    m_root_node = new BranchNode();  // 根目录创建日期和时间为NULL
    m_root_node->setName(volumeName);
    m_cur_node = m_root_node;
}

Volume::~Volume()
{
    m_cur_node = NULL;
    MyString name = "*";
    int numFile = 0;
    int numDir = 0;
    m_root_node->del(name, Type_Dir_File, true, numFile, numDir);  // 注销树
    delete m_root_node;
    m_root_node = NULL;
}

FileNode *Volume::getNodeByStr(MyString &path, FileNode *&parentNode, int &pos)
{
    FileNode *curNode = m_cur_node;
    if(pos < path.size() && path[pos] == '\\')
    {
        curNode = m_root_node;
        ++pos;
    }

    /* 遍历字符串路径 */
    MyString nodeName;
    char name[257];
    int i = pos;
    int j = 0;
    while(i < path.size())
    {
        if(path[i] == '\\' || i == path.size() - 1)
        {
            if(path[i] != '\\')
                name[j++] = path[i];
            if(j != 0)
            {
                name[j++] = '\0';
                j = 0;
                nodeName = name;

                FileNode *tmpNode = curNode->find(nodeName, Type_Dir_File, false);
                if((tmpNode != NULL) && (path[i] != '\\' || !tmpNode->isLeaf()))
                    curNode = tmpNode;
                else
                {
                    parentNode = curNode;
                    return NULL;
                }
            }
            pos = i + 1;
        }
        else
        {
            name[j++] = path[i];
        }
        ++i;
    }
    if(curNode->isLeaf())
        parentNode = curNode->getParent();
    else
        parentNode = curNode;

    return curNode;
}

int Volume::createNode(MyString &path, int startPos)
{
    FileNode *curNode = m_cur_node;
    if(startPos < path.size() && path[startPos] == '\\')
    {
        curNode = m_root_node;
        ++startPos;
    }

    // 申请栈结构
    FileNodeStack fileNodeStack;
    initStack(&fileNodeStack);

    /* 遍历字符串路径 */
    MyString nodeName;
    char name[257];
    int i = startPos;
    int j = 0;
    bool isFileToDir = false;

    while(i < path.size())
    {
        if(path[i] == '\\' || i == path.size() - 1)
        {
            if(path[i] != '\\')
                name[j++] = path[i];
            if(j != 0)
            {
                name[j++] = '\0';
                j = 0;
                nodeName = name;

                FileNode *tmpNode = curNode->find(nodeName, Type_Dir_File, false);
                if(tmpNode == NULL)
                {
                    tmpNode = curNode->create(nodeName);
                    pushStack(&fileNodeStack, tmpNode);
                    curNode = tmpNode;
                }
                else
                {
                    if(curNode != tmpNode)
                    {
                        if(isEmptyStack(&fileNodeStack))
                        {
                            if(tmpNode->isLeaf())
                                isFileToDir = true;
                            else
                                isFileToDir = false;
                        }
                        else
                        {
                            int numFile = 0;
                            int numDir = 0;
                            FileNode *parentNode = curNode->getParent();
                            if(parentNode->isLeaf())
                                delete curNode;
                            else
                                parentNode->del(curNode, numFile, numDir);
                            popStack(&fileNodeStack);
                        }
                    }
                    curNode = tmpNode;
                }
            }
        }
        else
        {
            name[j++] = path[i];
        }
        ++i;
    }
    if(isEmptyStack(&fileNodeStack))
    {
        ErrMsg::m_err_code = ErrMsg::PATH_EXIST_ERROR;
        ErrMsg::m_err_msg = path;
    }
    else if(isFileToDir)
    {
        while(!isEmptyStack(&fileNodeStack))
        {
            FileNode *tmpNode = popStack(&fileNodeStack);
            delete tmpNode;
        }
        ErrMsg::m_err_code = ErrMsg::CREATE_FILE_DIR_ERROR;
        ErrMsg::m_err_msg = path;
    }
    destroyStack(&fileNodeStack);
    return ErrMsg::m_err_code;
}

/* VirtualDisk函数 */
const int VirtualDisk::MAX_COUNT = 10;
const int VirtualDisk::COMPARE_PRINT_LEN = 16;
const int VirtualDisk::MAX_PATH_NAME_LEN = 256;
VirtualDisk::Garbo VirtualDisk::garbo;
VirtualDisk *VirtualDisk::m_virtual_disk = NULL;

VirtualDisk *VirtualDisk::getInstance(int numVolume)
{
    if(m_virtual_disk == NULL)
        m_virtual_disk = new VirtualDisk(numVolume);
    return m_virtual_disk;
}
VirtualDisk::VirtualDisk(int count)
{
    if(count <= MAX_COUNT && count >= 0)
    {
        m_count = count;
        m_volume = (Volume **)malloc(m_count * sizeof(Volume *));

        MyString name = "C:";
        for(int i = 0; i < m_count; ++i)
        {
            name[0] = 'C' + i;
            m_volume[i] = new Volume(name);
        }
        m_cur_volume = m_volume[0];
        showPath(m_cur_volume);
    }
}
VirtualDisk::~VirtualDisk()
{
    for(int i = 0; i < m_count; ++i)
    {
        delete m_volume[i];
        m_volume[i] = NULL;
    }
    free(m_volume);
    m_volume = NULL;
    m_cur_volume = NULL;
}

int VirtualDisk::checkPath(MyString &path)
{
    int numChar = 0;
    bool wildExist = false;
    for(int i = 0; i < path.size(); ++i)
    {
        ++numChar;
        if(path[i] == '/' || path[i] == '"' || path[i] == '<' || path[i] == '>' || path[i] == '|')
        {
            // path[i] == '\\' 被舍去，'\\'作为路径中的分隔符
            ErrMsg::m_err_code = ErrMsg::INVALID_PATH_NAME_ERROR;
            ErrMsg::m_err_msg = path;
            break;
        }
        else if(path[i] == ':')
        {
            if(i != 1)
            {
                ErrMsg::m_err_code = ErrMsg::FILE_DIRECTORY_SYNTACTIC_ERROR;
                ErrMsg::m_err_msg = path;
                break;
            }
            numChar = 0;
        }
        else if(path[i] == '*' || path[i] == '?')
        {
            wildExist = true;
        }
        else if(path[i] == '\\')
        {
            // 字符数 < 256
            if(numChar > MAX_PATH_NAME_LEN)
            {
                ErrMsg::m_err_code = ErrMsg::PATH_LENGTH_ERROR;
                ErrMsg::m_err_msg = path;
                break;
            }
            if(wildExist)
            {
                ErrMsg::m_err_code = ErrMsg::FILE_DIRECTORY_SYNTACTIC_ERROR;
                ErrMsg::m_err_msg = path;
                break;
            }
            numChar = 0;
        }
        else if(i == path.size() - 1)
        {
            // 字符数 < 256
            if(numChar >= MAX_PATH_NAME_LEN)
            {
                ErrMsg::m_err_code = ErrMsg::PATH_LENGTH_ERROR;
                ErrMsg::m_err_msg = path;
                break;
            }
        }
    }
    return ErrMsg::m_err_code;
}
Volume *VirtualDisk::getVolumeByStr(MyString &path, int &pos)
{
    if(path.size() >= 2 && path[1] == ':')
    {
        pos = 2;
        MyString volumeName(path, 0, 2);
        for(int i = 0; i < m_count; ++i)
        {
            if(volumeName == m_volume[i]->getRootDir()->getName())
                return m_volume[i];
        }
        return NULL;
    }
    return m_cur_volume;
}

void VirtualDisk::showPath(Volume *volume)
{
    MyString curPath;
    volume->getCurDir()->getPathStr(curPath);
    curPath += ">";
    printf("\n%s", curPath.c_str());
}

int VirtualDisk::readFile(char *fileData, MyString &path, unsigned long len)
{
    FILE *ifile = NULL;
    ifile = fopen(path.c_str(), "rb");
    if(ifile == NULL)
    {
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_FILE_ERROR;
        ErrMsg::m_err_msg = path;
    }
    else
    {
        fread(fileData, sizeof(char), len, ifile);
        fclose(ifile);
    }

    return ErrMsg::m_err_code;
}

bool VirtualDisk::isBinary(char *buf, unsigned long len)
{
    // 判断文件是否是二进制文件：如果文件中存在char(0)，则判定文件是二进制文件
    for(unsigned long i = 0; i < len; ++i)
    {
        if(buf[i] == '\0')
            return true;
    }
    return false;
}

void VirtualDisk::printNotMatch(unsigned char *buf, unsigned long pos, bool isBinary)
{
    printf("位置：%d\n", pos);
    if(isBinary)
    {
        unsigned char *p = buf;
        for(int i = 0; i < COMPARE_PRINT_LEN; ++i)
        {
            printf("%02X ", *p++);
        }
    }
    else
    {
        printf("%s", buf);
    }
    printf("\n");
}

void VirtualDisk::getDirAndFile(MyString &realPath, MyString &dir, MyString &file)
{
    int start = realPath.size() - 1;
    if(realPath[start] == '\\')
    {
        MyString tmp(realPath, 0, start);
        dir = tmp;
        file = NULL;
        realPath = tmp;
    }
    else
    {
        for(int i = start; i >= 0; --i)
        {
            if(realPath[i] == '\\')
            {
                MyString tmp1(realPath, 0, i);
                MyString tmp2(realPath, i + 1, realPath.size());
                dir = tmp1;
                file = tmp2;
                return ;
            }
            else if(realPath[i] == ':')
            {
                MyString tmp1(realPath, 0, i + 1);
                MyString tmp2(realPath, i + 1, realPath.size());
                dir = tmp1;
                file = tmp2;
                return ;
            }
        }
        dir = NULL;
        file = realPath;
    }
}

bool VirtualDisk::existDir(MyString &path, int start)
{
    bool firstIsChar = false;
    for(int i = start; i < path.size(); ++i)
    {
        if(path[i] != '\\')
            firstIsChar = true;
        else if(path[i] == '\\' && firstIsChar)
            return true;
    }
    return false;
}

MyString &VirtualDisk::genName(MyString &realName, MyString &virtualName, MyString &retName)
{
    if(virtualName == "*.*")
        virtualName = "*";
    if(!(virtualName.find('*') || virtualName.find('?')))
        retName = virtualName;
    else if(virtualName.wildCmp(realName))
        retName = realName;
    else
    {
        int i = 0;
        int pos = 0;
        while(i < virtualName.size())
        {
            if(virtualName[i] == '*')
            {
                MyString tmpName(virtualName, pos, i);
                retName += tmpName;
                retName += realName;
                pos = i + 1;
            }
            else if(virtualName[i] == '?')
            {
                MyString tmpName(virtualName, pos, i);
                retName += tmpName;
                pos = i + 1;
            }
            else if(i == virtualName.size() - 1)
            {
                MyString tmpName(virtualName, pos, virtualName.size());
                retName += tmpName;
                pos = i + 1;
            }
            ++i;
        }
    }
    return retName;
}

int VirtualDisk::findRealFile(MyString &realPath, FileList &fileList, bool bRecursion)
{
    MyString realDir;
    MyString realFile;
    getDirAndFile(realPath, realDir, realFile);
    struct _finddata_t findData;
    unsigned long handle = _findfirst(realPath.c_str(), &findData);
    unsigned long done = 0;
    if(handle == -1)
    {
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_FILE_ERROR;
        ErrMsg::m_err_msg = realPath;
    }
    else
    {
        do
        {
            MyString fName = findData.name;
            unsigned long len = findData.size;
            if((findData.attrib&_A_SUBDIR) == _A_SUBDIR)
            {
                if(bRecursion && fName != "." && fName != "..")
                {
                    MyString subPath = realPath;
                    subPath += "\\*.*";
                    findRealFile(subPath, fileList, false);
                }
            }
            else
            {
                MyString fNameAll = realDir;
                fNameAll += "\\";
                fNameAll += fName;
                fileList.add(fNameAll, fName, len, findData.time_write);
            }
            bRecursion = false;
            done = _findnext(handle, &findData);
        } while(done != -1);
    }
    _findclose(handle);
    return ErrMsg::m_err_code;
}

int VirtualDisk::copyToOne(FileNode *node, FileList &fileList, MyString &name)
{
    // 多个文件合并为一个文件
    int numCopy = 0;
    unsigned long totalSize = 0;
    const unsigned long MAX_FILE_SIZE = (const unsigned long)1024*1024*1024*2;
    for(int i = 0; i < fileList.getCount(); ++i)
    {
        totalSize += fileList.getSizeByIdx(i);
        if(totalSize > MAX_FILE_SIZE)
        {
            ErrMsg::m_err_code = ErrMsg::MEM_ALLOC_ERROR;
            ErrMsg::m_err_msg = name;
            return numCopy;
        }
    }
    char *fData = new(std::nothrow) char[totalSize];
    if(fData == NULL)
    {
        ErrMsg::m_err_code = ErrMsg::MEM_ALLOC_ERROR;
        ErrMsg::m_err_msg = name;
        return numCopy;
    }
    unsigned long offset = 0;
    for(int i = 0; i < fileList.getCount(); ++i)
    {
        if(!readFile(&fData[offset], fileList.getPathByIdx(i), fileList.getSizeByIdx(i)))
            offset += fileList.getSizeByIdx(i);
        else
        {
            delete []fData; fData = NULL;
            return numCopy;
        }
    }

    FileNode *existNode = node->find(name, Type_Dir_File, false);
    if(existNode == NULL)
    {
        time_t t = time(0);
        if(fileList.getCount() == 1)
            t = fileList.getTimeByIdx(0);

        node->create(name, t, fData, totalSize);
        numCopy += fileList.getCount();
    }
    else if(existNode->isLeaf())
    {
        char s[100];
        while(1)
        {
            printf("覆盖%s吗?(Yes/No/All):", name.c_str());
            gets(s);
            if(s[0] == 'Y' || s[0] == 'y')
            {
                if(fileList.getCount() == 1)
                    existNode->setTime(fileList.getTimeByIdx(0));
                else
                    existNode->setTime(time(0));
                existNode->setFile(fData, totalSize);
                numCopy += fileList.getCount();
                break;
            }
            else if(s[0] == 'N' || s[0] == 'n')
            {
                delete []fData; fData = NULL;
                break;
            }
            else if(s[0] == 'A' || s[0] == 'a')
            {
                if(fileList.getCount() == 1)
                    existNode->setTime(fileList.getTimeByIdx(0));
                else
                    existNode->setTime(time(0));
                existNode->setFile(fData, totalSize);
                numCopy += fileList.getCount();
                break;
            }
        }
    }
    else
    {
        delete []fData; fData = NULL;
        ErrMsg::m_err_code = ErrMsg::ACCESS_DENY_ERROR;
        ErrMsg::m_err_msg = name;
    }
    return numCopy;
}

int VirtualDisk::copyFile(FileNode *node, FileList &fileList, MyString &name)
{
    int numCopy = 0;
    bool isAll = false;
    for(int i = 0; i < fileList.getCount(); ++i)
    {
        MyString fPath = fileList.getPathByIdx(i);
        MyString fName = fileList.getNameByIdx(i);
        unsigned long fSize = fileList.getSizeByIdx(i);
        time_t fTime = fileList.getTimeByIdx(i);

        char *fData = new(std::nothrow) char[fSize];
        if(fData == NULL)
        {
            ErrMsg::m_err_code = ErrMsg::MEM_ALLOC_ERROR;
            ErrMsg::m_err_msg = fName;
            ErrMsg::printErr();
            continue;
        }
        if(!readFile(fData, fPath, fSize))
        {
            MyString newName;
            genName(fName, name, newName);
            FileNode *existNode = node->find(newName, Type_Dir_File, false);
            if(existNode == NULL)
            {
                node->create(newName, fTime, fData, fSize);
                ++numCopy;
            }
            else
            {
                if(existNode->isLeaf())
                {
                    char s[100];
                    while(1)
                    {
                        if(isAll)
                        {
                            s[0] = 'Y';
                        }
                        else
                        {
                            printf("覆盖%s吗?(Yes/No/All):", newName.c_str());
                            gets(s);
                        }
                        if(s[0] == 'Y' || s[0] == 'y')
                        {
                            existNode->setTime(fTime);
                            existNode->setFile(fData, fSize);
                            ++numCopy;
                            break;
                        }
                        else if(s[0] == 'N' || s[0] == 'n')
                        {
                            delete []fData; fData = NULL;
                            break;
                        }
                        else if(s[0] == 'A' || s[0] == 'a')
                        {
                            existNode->setTime(fTime);
                            existNode->setFile(fData, fSize);
                            ++numCopy;
                            isAll = true;
                            break;
                        }
                    }
                }
                else
                {
                    delete []fData; fData = NULL;
                    ErrMsg::m_err_code = ErrMsg::ACCESS_DENY_ERROR;
                    ErrMsg::m_err_msg = newName;
                    ErrMsg::printErr();
                }
            }
        }
        else
        {
            delete []fData; fData = NULL;
        }
    }
    return numCopy;
}

int VirtualDisk::Cd(MyString &path)
{
    int pos = 0;
    Volume *curVolume = getVolumeByStr(path, pos);
    if(curVolume == NULL)
    {
        // 找不到驱动器
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_DRIVER_ERROR;
        ErrMsg::m_err_msg = path;
        return ErrMsg::m_err_code;
    }

    FileNode *parentNode = NULL;
    FileNode *curNode = curVolume->getNodeByStr(path, parentNode, pos);
    if(curNode == NULL)
    {
        // 路径错误
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_PATH_ERROR;
        ErrMsg::m_err_msg = path;
        return ErrMsg::m_err_code;
    }
    else if(curNode->isLeaf())
    {
        ErrMsg::m_err_code = ErrMsg::INVALID_DIRECTORY_NAME_ERROR;
        ErrMsg::m_err_msg = path;
        return ErrMsg::m_err_code;
    }

    if(path == NULL || path == curVolume->getRootDir()->getName())
    {
        MyString pathStr;
        curVolume->getCurDir()->getPathStr(pathStr);
        printf("%s\n", pathStr.c_str());
    }
    else if(curVolume == m_cur_volume)
        m_cur_volume->setCurDir(curNode);
    
    return ErrMsg::m_err_code;
}

int VirtualDisk::Compare(MyString &realPath, MyString &virtualPath)
{
    int pos = 0;
    Volume *curVolume = getVolumeByStr(virtualPath, pos);
    if(curVolume == NULL)
    {
        // 找不到驱动器
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_DRIVER_ERROR;
        ErrMsg::m_err_msg = virtualPath;
        return ErrMsg::m_err_code;
    }

    FileNode *parentNode = NULL;
    FileNode *curNode = curVolume->getNodeByStr(virtualPath, parentNode, pos);
    if(curNode == NULL || !curNode->isLeaf())
    {
        // 路径错误
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_FILE_ERROR;
        ErrMsg::m_err_msg = virtualPath;
        return ErrMsg::m_err_code;
    }

    FileList fileList;
    if(findRealFile(realPath, fileList, false))
    {
        // 找不到磁盘文件
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_FILE_ERROR;
        ErrMsg::m_err_msg = realPath;
        return ErrMsg::m_err_code;
    }

    unsigned long realLen = fileList.getSizeByIdx(0);
    char *realFileData = new(std::nothrow) char[realLen];
    if(realFileData == NULL)
    {
        ErrMsg::m_err_code = ErrMsg::MEM_ALLOC_ERROR;
        ErrMsg::m_err_msg = realPath;
        return ErrMsg::m_err_code;
    }
    if(readFile(realFileData, realPath, realLen))
    {
        delete realFileData; realFileData = NULL;
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_FILE_ERROR;
        ErrMsg::m_err_msg = realPath;
        return ErrMsg::m_err_code;
    }

    char *virtualFileData = curNode->getFileData();
    unsigned long virtualLen = curNode->getSize();

    unsigned long len = realLen < virtualLen ? realLen : virtualLen;

    unsigned long readPos = 0;
    for(readPos = 0; readPos < len; ++readPos)
    {
        if(realFileData[readPos] != virtualFileData[readPos])
            break;
    }

    if(readPos == realLen && readPos == virtualLen)
    {
        printf("内容比较一致。\n");
    }
    else
    {
        unsigned char buf[COMPARE_PRINT_LEN + 1];
        unsigned long i = 0;

        for(i = 0; i < realLen - readPos && i < COMPARE_PRINT_LEN; ++i)
            buf[i] = realFileData[readPos + i];
        buf[i] = '\0';
        printNotMatch(buf, readPos, isBinary(realFileData, realLen));
        printf("\n");
        for(i = 0; i < virtualLen - readPos && i < COMPARE_PRINT_LEN; ++i)
            buf[i] = virtualFileData[readPos + i];
        buf[i] = '\0';
        printNotMatch(buf, readPos, isBinary(virtualFileData, virtualLen));
    }
    delete []realFileData; realFileData = NULL;

    return ErrMsg::m_err_code;
}

int VirtualDisk::Dir(MyString &path, FType type, bool bRecursion)
{
    int pos = 0;
    Volume *curVolume = getVolumeByStr(path, pos);
    if(curVolume == NULL)
    {
        // 找不到驱动器
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_PATH_ERROR;
        ErrMsg::m_err_msg = path;
        return ErrMsg::m_err_code;
    }
    
    FileNode *parentNode = NULL;
    FileNode *curNode = curVolume->getNodeByStr(path, parentNode, pos);
    if(existDir(path, pos))
    {
        // 路径错误
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_FILE_ERROR;
        ErrMsg::m_err_msg = path;
        return ErrMsg::m_err_code;
    }

    int numFile = 0;
    int numDir = 0;
    if(curNode == NULL)
    {
        MyString dir;
        MyString file;
        getDirAndFile(path, dir, file);
        parentNode->dir(file, type, bRecursion, numFile, numDir);
    }
    else
    {
        MyString tmpName;
        if(curNode->isLeaf())
            tmpName = curNode->getName();
        else
            tmpName = "*";

        parentNode->dir(tmpName, type, bRecursion, numFile, numDir);
    }
    if(numDir == 0 && numFile == 0)
    {
        ErrMsg::m_err_code = ErrMsg::FIND_FILE_ERROR;
        ErrMsg::m_err_msg = path;
    }
    if(ErrMsg::m_err_code == 0 && bRecursion)
    {
        printf("\n     所列总文件数:\n");
        printf("             %d 个文件\n", numFile);
        printf("             %d 个目录\n", numDir);
    }

    return ErrMsg::m_err_code;
}

int VirtualDisk::Rmdir(MyString &path, bool bRecursion)
{
    int pos = 0;
    Volume *curVolume = getVolumeByStr(path, pos);
    if(curVolume == NULL)
    {
        // 找不到驱动器
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_PATH_ERROR;
        ErrMsg::m_err_msg = path;
        return ErrMsg::m_err_code;
    }

    FileNode *parentNode = NULL;
    FileNode *curNode = curVolume->getNodeByStr(path, parentNode, pos);
    if(curNode == NULL)
    {
        // 路径错误
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_FILE_ERROR;
        ErrMsg::m_err_msg = path;
        return ErrMsg::m_err_code;
    }
    else if(curNode->isLeaf())
    {
        ErrMsg::m_err_code = ErrMsg::INVALID_DIRECTORY_NAME_ERROR;
        ErrMsg::m_err_msg = path;
        return ErrMsg::m_err_code;
    }

    if(m_cur_volume->getCurDir() != m_cur_volume->getRootDir())
    {
        for(FileNode *p = m_cur_volume->getCurDir(); p != NULL; p = p->getParent())
        {
            if(curNode == p)
            {
                ErrMsg::m_err_code = ErrMsg::RMDIR_ONESELF_ERROR;
                ErrMsg::m_err_msg = path;
                return ErrMsg::m_err_code;
            }
        }
    }
    bool isSwitchDir = false;
    if(curVolume->getCurDir() != curVolume->getRootDir())
    {
        for(FileNode *p = curVolume->getCurDir(); p != NULL; p = p->getParent())
        {
            if(curNode == p)
            {
                isSwitchDir = true;
                break;
            }
        }
    }

    int numFile = 0;
    int numDir = 0;
    if(!bRecursion)
    {
        if(curNode->getChild() == NULL)
        {
            if(curNode != curVolume->getRootDir())
                parentNode->getParent()->del(curNode, numFile, numDir);
            if(isSwitchDir)
                curVolume->setCurDir(curVolume->getRootDir());
        }
        else
        {
            ErrMsg::m_err_code = ErrMsg::DIRECTORY_NOT_EMPTY_ERROR;
            ErrMsg::m_err_msg = path;
        }
    }
    else
    {
        char s[100];
        while(1)
        {
            printf("%s，是否确认(Yes/No)?:", path);
            gets(s);
            if(s[0] == 'Y' || s[0] == 'y')
            {
                MyString tmpName = "*";
                parentNode->del(tmpName, Type_Dir_File, true, numFile, numDir);
                if(curNode != curVolume->getRootDir())
                    parentNode->getParent()->del(curNode, numFile, numDir);
                if(isSwitchDir)
                    curVolume->setCurDir(curVolume->getRootDir());
                break;
            }
            else if(s[0] == 'N' || s[0] == 'n')
            {
                break;
            }
        }
    }
    if(numDir == 0 && numFile == 0)
    {
        ErrMsg::m_err = 1;
    }

    return ErrMsg::m_err_code;
}

int VirtualDisk::Mkdir(MyString &path)
{
    int pos = 0;
    Volume *curVolume = getVolumeByStr(path, pos);
    if(curVolume == NULL)
    {
        // 找不到驱动器
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_DRIVER_ERROR;
        ErrMsg::m_err_msg = path;
        return ErrMsg::m_err_code;
    }

    curVolume->createNode(path, pos);

    return ErrMsg::m_err_code;
}

int VirtualDisk::Del(MyString &path, bool bRecursion)
{
    int pos = 0;
    Volume *curVolume = getVolumeByStr(path, pos);
    if(curVolume == NULL)
    {
        // 找不到驱动器
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_PATH_ERROR;
        ErrMsg::m_err_msg = path;
        return ErrMsg::m_err_code;
    }
    
    FileNode *parentNode = NULL;
    FileNode *curNode = curVolume->getNodeByStr(path, parentNode, pos);
    if(existDir(path, pos))
    {
        // 路径错误
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_FILE_ERROR;
        ErrMsg::m_err_msg = path;
        return ErrMsg::m_err_code;
    }

    int numFile = 0;
    int numDir = 0;
    if(curNode != NULL)
    {
        if(curNode->isLeaf())
        {
            MyString tmpName = curNode->getName();
            parentNode->del(tmpName, Type_File, bRecursion, numFile, numDir);
        }
        else
        {
            char s[100];
            MyString tmpPath;
            parentNode->getPathStr(tmpPath);
            tmpPath += "\\";
            tmpPath += "*.*";
            while(1)
            {
                printf("%s，是否确认(Y/N)?", tmpPath);
                gets(s);
                MyString tmpName = "*";
                if(s[0] == 'Y' || s[0] == 'y')
                {
                    parentNode->del(tmpName, Type_File, bRecursion, numFile, numDir);
                    break;
                }
                else if(s[0] == 'N' || s[0] == 'n')
                {
                    break;
                }
            }
        }
    }
    else
    {
        MyString dir, file;
        getDirAndFile(path, dir, file);
        MyString tmpPath;
        parentNode->getPathStr(tmpPath);
        tmpPath += "\\";
        tmpPath += file;
        if(file == "*.*" || file == "*")
        {
            char s[100];
            while(1)
            {
                printf("%s，是否确认(Y/N)?", tmpPath);
                gets(s);
                if(s[0] == 'Y' || s[0] == 'y')
                {
                    parentNode->del(file, Type_File, bRecursion, numFile, numDir);
                    break;
                }
                else if(s[0] == 'N' || s[0] == 'n')
                {
                    break;
                }
            }
        }
        else
        {
            parentNode->del(file, Type_File, bRecursion, numFile, numDir);
        }
        if(numFile + numDir == 0 && file != "*.*" && file != "*")
        {
            ErrMsg::m_err_code = ErrMsg::FIND_PATH_ERROR;
            ErrMsg::m_err_msg = tmpPath;
        }
    }
    if(numFile + numDir == 0)
    {
        ErrMsg::m_err = 1;
    }
    return ErrMsg::m_err_code;
}

int VirtualDisk::Copy(MyString &realPath, MyString &virtualPath)
{
    int pos = 0;
    Volume *curVolume = getVolumeByStr(virtualPath, pos);
    if(curVolume == NULL)
    {
        // 找不到驱动器
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_DRIVER_ERROR;
        ErrMsg::m_err_msg = virtualPath;
        return ErrMsg::m_err_code;
    }
    
    FileNode *parentNode = NULL;
    FileNode *curNode = curVolume->getNodeByStr(virtualPath, parentNode, pos);
    if(existDir(virtualPath, pos))
    {
        // 路径错误
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_PATH_ERROR;
        ErrMsg::m_err_msg = virtualPath;
        return ErrMsg::m_err_code;
    }
    
    FileList fileList;
    if(findRealFile(realPath, fileList, true))
    {
        // 找不到磁盘文件
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_FILE_ERROR;
        ErrMsg::m_err_msg = realPath;
        return ErrMsg::m_err_code;
    }

    int numCopy = 0;
    if(curNode == NULL)
    {
        MyString virtualDir;
        MyString virtualFile;
        getDirAndFile(virtualPath, virtualDir, virtualFile);
        if(!virtualFile.find('*') && !virtualFile.find('?'))
            numCopy = copyToOne(parentNode, fileList, virtualFile);
        else
            numCopy = copyFile(parentNode, fileList, virtualFile);
    }
    else if(curNode->isLeaf())
    {
        numCopy = copyToOne(parentNode, fileList, curNode->getName());
    }
    else
    {
        MyString fName = "*";
        numCopy = copyFile(parentNode, fileList, fName);
    }

    if(numCopy == 0)
        ErrMsg::m_err = 1;
    else
        ErrMsg::m_err = 0;

    printf("已复制      %d个文件。\n", numCopy);

    return ErrMsg::m_err_code;
}

int VirtualDisk::SwitchVolume(MyString &path)
{
    int pos = 0;
    Volume *curVolume = getVolumeByStr(path, pos);
    if(curVolume == NULL)
    {
        // 找不到驱动器
        ErrMsg::m_err_code = ErrMsg::FIND_SPECIFIED_DRIVER_ERROR;
        ErrMsg::m_err_msg = path;
        return ErrMsg::m_err_code;
    }

    m_cur_volume = curVolume;
    
    return ErrMsg::m_err_code;
}