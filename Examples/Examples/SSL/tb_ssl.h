#ifndef TB_SSL_H
#define TB_SSL_H

#include <TPTypedef.h>
#include <TPNet.h>

#define SSL_ERR_MALLOC_FAILED -2000
#define SSL_ERR_BADF          -2001
#define SSL_ERR_MFILE         -2002
#define SSL_ERR_INIT          -2003
#define SSL_ERR_HANDSHAKE     -2004
#define SSL_ERR_WRITE         -2005
#define SSL_ERR_READ          -2006

// typedef enum
// {
//     HTTP_OK              =   0,     //���ӳɹ�
//         HTTP_ERROR_PPP       =  -1,		//GRPS PPP ����ʧ��
//         HTTP_ERROR_CONNECT   =  -2,		//���ӷ�������ʱ
//         HTTP_PARAM_ERROR     =  -3,		//��������
//         HTTP_SEND_FAIL       =  -4,		//����ʧ�ܻ�������ķ��ͳ�������0
//         HTTP_RECV_FAIL       =  -5,		//����ʧ�ܻ�������Ľ��ճ�������0
//         HTTP_ERROR_NOT200    =  -6,		//���ز���200 OK
//         HTTP_RECV_EMPTY      =  -7,		//��ȥHttpЭ��ͷ�������Ϊ��
//         HTTP_ALLOCMEMORY_FAIL = -8,		//�ڴ����ʧ��
//         HTTP_SSL_HANDSHAKE_ERROR = -9,  //ssl����ʧ��
//         
// } HTTP_RESULT;

// prueba asd asd 
//HTTPͷ�Ľ�β�ַ���
#define HTTP_HEAD_END_STR  "\r\n\r\n"
//HTTPͷ�ؼ��ֳ���
#define HTTP_KEYWORD_CONTENT_LENGTH "Content-Length:"

//HTTPͷ�ļ���Χ
#define HTTP_CONTENT_RANGE  "Content-Range:"

#define HTTP_KEYWORD_CHUNKED  "Transfer-Encoding: chunked"

/**
 * <b>Create a SSL link on a connected socket</b><br>
 *
 * <p>
 *
 *
 * @param socket_fd_32 <br>A descriptor of socket has been connected to the server
 * @param timeout_ms_32 <br>Timeout value to open a SSL socket channel
 *
 * @return >=0, descriptor of SSL socket channel<br>
 *          <0, see error codes
 *
 * @since TB_SSL V1.0
 */
extern int32
tb_ssl_open(int32 socket_fd_32,
            int32 timeout_ms_32);

/**
 * <b>Write datas to server through specified SSL link</b><br>
 *
 * <p>
 *
 *
 * @param fd_32 <br> Descriptor of a SSL socket as return value of function <code>tb_ssl_socket()</code>
 * @param p_data_u8 <br>Pointer to datas to send
 * @param send_bytes_u16 <br>Bytes number to send
 * @param timeout_ms_32 <br>Timeout value in milliseconds
 *
 * @return >=0, number of actually sended bytes<br><0, see error codes
 *
 * @since TB_SSL V1.0
 */
extern int32
tb_ssl_write(int32        fd_32,
             const uint8* p_data_u8,
             uint16       send_bytes_u16,
             int32        timeout_ms_32);

/**
 * <b>Read datas from server through a specified SSL link</b><br>
 *
 * <p>
 *
 *
 * @param fd_32 <br>
 * @param p_buf_u8 <br>
 * @param read_bytes_u16 <br>
 * @param timeout_ms_32 <br>
 *
 * @return
 *
 * @since TB_SSL V1.0
 */
extern int32
tb_ssl_read(int32  fd_32,
            uint8* p_buf_u8,
            uint16 read_bytes_u16,
            int32  timeout_ms_32);

/**
 * <b>Destroy a specified SSL link</b><br>
 *
 * This function only destroy the SSL link which specified by <code>fd_32</code>, you should then call the function <code>TP_NetCloseSocket()</code> to close the socket that used by this SSL link
 *
 * @param fd_32 <br>Descriptor of SSL link that allocated by function <code>tb_ssl_open()</code>
 *
 * @return 0, Operation successful<br> <0, see error codes
 *
 * @since TB_SSL V1.0
 */
extern int32
tb_ssl_close(int32 fd_32);

#endif

