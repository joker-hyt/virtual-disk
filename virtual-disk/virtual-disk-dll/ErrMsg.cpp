/**
 * ������Ϣ��ӡʵ��
 */

#include "ErrMsg.h"
#include <stdio.h>

/* ���������Ϣ��ʼ�� */
const MyString ErrMsg::M_ERR_MSG[22] = {
    "������ȷ",                                           // ��ȷ
    " - �����ڲ����ⲿ���Ҳ���ǿ����еĳ���\n���������ļ���",  // �������
    "ϵͳ�Ҳ���ָ����·����",                               // "cd sss"
    "ϵͳ�Ҳ���ָ�����ļ���",                               //                             "copy sss"                                       "rmdir sss"      "compare sss aaa"
    "�Ҳ����ļ�",                                         //               "dir sss"
    "��Ч���� - ",                                        //               "dir /z"                     "del /ab"                         "rmdir /a"
    "�����﷨����ȷ��",                                    //                             "copy"          "del"          "mkdir",          "rmdir"          "compare"
    " - ������·��̫����",                                 //                                                            "mkdir a..."
    "�ļ�������չ��̫����",                                 // "cd a..."
    "�ļ���̫����",
    "�Ҳ��� - ",                                         //                                              "del sss"
    "�ļ���·�������Ϸ���",                                 //                                                            "mkdir a:a"
    "Ŀ¼������Ч��",                                      // "cd sss"                                                                     "rmdir a.txt"
    " - ��Ŀ¼���ļ��Ѿ����ڡ�",                            //                                                            "mkdir aaa"
    "Ŀ¼���ǿյġ�",                                      //                                                                              "rmdir sss"
    "�ļ�����Ŀ¼�������﷨����ȷ��",                       // ȫ��
    "��һ����������ʹ�ô��ļ��������޷����ʡ�",                 //                                                                               "rmdir oneself"
    " - �ļ�̫���޷������ڴ�",                            // ȫ��
    " - �ܾ����ʡ�",                                      //                             "copy aa a?(a?��Ŀ¼)"
    "ϵͳ�Ҳ���ָ������������",                              //                             "copy aa z:\"
    "�޷�����Ŀ¼���ļ�"
};

int ErrMsg::m_err = 0;
int ErrMsg::m_err_code = 0;
MyString ErrMsg::m_err_msg = M_ERR_MSG[0];

/* ��ӡ������Ϣ */
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