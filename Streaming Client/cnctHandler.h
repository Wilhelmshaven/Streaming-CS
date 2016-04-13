/*--Author������--*/

#include "CommonHeaders.h"

/*
	cnctHandler���ͻ�����������ģ�飬ʹ��Winsock��������������ӣ�������������Ϣ
*/

/*
	��������Ϣ�ṹ��srvInfo

	������
	����Ϊ���ļ��ж�ȡ�������б������ڴ����������ģ�顣
	���˼·����ͨ�������ļ��������������Ϣ������������Ϣ�и��£�ֱ�Ӵ���վ����������ʽ�滻�������ļ����ɡ�
	ʹ��˫�����������ļ��ж�ȡ���Ĵ洢��������Ϣ��
	�����ļ�����Ӧ�ı�׼��������ҵ����ġ�

	Todo�������ļ���׼�鵽����
*/
typedef struct srvInfo
{
	//����������
	string srvName;

	//�����ĵ�ַ
	string address;

	//Э��
	string protocol;

	//����
	string hostName;

	sockaddr_in srv;
};

typedef struct threadParam
{
	SOCKET socket;
};
/*
	cnctHandler�ࣺ�ṩ�ͻ����������ӹ���

	������
	����Winsockʵ��
	�����ǳ�ʼ�����ڹ���ʱ�趨Ĭ�ϲ���Ϊ���ط���������������ȡ���������ļ���������������Ϣ�ṹ����˫��������ʽ�洢
	֮���ɵ���connnectServer�������ӷ�����

	ʹ�ã�

	int connectServer()�����ӷ��������ɹ�����0������Ϊ-1�������޿��÷�������������ֵ���������ϵ�������ʱ����connect�������صĴ����룬�μ�MSDN��

	string getDisplayAddr()����ȡ�������ŵ�ַ������������ý�����������

	void sendMessage(string msg)�����͸���������

	bool getRTPMessage(string &msg)��ȡ���յ���ͼ������

	bool getRTSPMessage(string &msg)��ȡ���յ�����������
*/
class cnctHandler
{
public:

	WSADATA wsaData;

	static cnctHandler *getInstance();

	//���ӷ��������ɹ�����0������Ϊ-1�������޿��÷�������������ֵ���������ϵ�������ʱ����connect�������صĴ����룬�μ�MSDN��
	int connectServer();    

	//��ȡ�������ŵ�ַ������������ý�����������
	string getDisplayAddr();

	//���͸���������
	static void sendMessage(string msg);

	//ȡ���յ���ͼ������
	static bool getRTPMessage(string &msg);

	//ȡ���յ�����������
	static bool getRTSPMessage(string &msg);

	~cnctHandler();

private:

	//��Ϣ����
	static queue<string> sendMsgQueue;

	static queue<string> recvRTPQueue;

	static queue<string> recvRTSPQueue;

	static DWORD WINAPI sendThread(LPVOID lparam);

	static DWORD WINAPI recvThread(LPVOID lparam);

	/*
		���������ñ�����
	*/

	//�洢��������Ϣ��˫������
	list<srvInfo> myServerList;

	srvInfo myServer;

	//��ȡ�����ļ�
	bool readFile(string fileName);

	void fillInfo(string label, string buf, srvInfo &serverInfo);

	/*
		��ǰ����ʹ�õķ�������Ϣ
	*/

	SOCKET srvSocket;

	//��ַ�ṹ��ipv4/6��IP���˿ڣ�
	sockaddr_in srvAddr;   

	//�����Ĳ��ŵ�ַ��eg. http://www.rayion.com/desktop
	string displayAddr;    

	/*
		����
	*/

	//���ӳɹ�����ã����������뷢�͵��߳�
	void startThread();

	/*
		����Ϊ����ģʽ���
	*/

	//���캯��������Ϊ�洢�з�������Ϣ��XML��ʽ�����ļ�
	cnctHandler(string file = srvSettingFile);

	static cnctHandler *instance; 

	//��ֹ���������Լ���ֵ
	cnctHandler(const cnctHandler &);
	cnctHandler & operator = (const cnctHandler &);

	//��������
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (instance)delete instance;
		}
	};
	static CGarbo Garbo;
};