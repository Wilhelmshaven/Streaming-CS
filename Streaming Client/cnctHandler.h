/*--Author������--*/

#include "CommonHeaders.h"

/*
	cnctHandler���ͻ�����������ģ�飬ʹ��Winsock���������������
*/

/*
	�������б�ṹ��serverList

	������
	����Ϊ���ļ��ж�ȡ�������б������ڴ����������ģ�顣
	���˼·����ͨ�������ļ��������������Ϣ������������Ϣ�и��£�ֱ�Ӵ���վ����������ʽ�滻�������ļ����ɡ�
	ʹ��˫�����������ļ��ж�ȡ���Ĵ洢��������Ϣ��
	�����ļ�����Ӧ�ı�׼��������ҵ����ġ�

	Todo�������ļ���׼�鵽����
*/
typedef struct serverList
{
	//ÿһ�д洢һ������
	//ServerArgs���������������ϸ�Ĳ���������ͷ�еĳ�����ServerInfoDefine
	string srvArgs[ServerArgc];      

	serverList *prev;
	serverList *next;		
};

/*
	cnctHandler�ࣺ�ṩ�ͻ����������ӹ���

	������
	����Winsockʵ��
	�����ǳ�ʼ�����ڹ���ʱ�趨Ĭ�ϲ���Ϊ���ط���������������ȡ���������ļ���������������Ϣ�ṹ����˫��������ʽ�洢
	֮���ɵ���connnectServer�������ӷ�����

	ʹ�ã�
	static cnctHandler *getInstance()����ȡ����

	int connectServer()�����ӷ��������ɹ�����0������Ϊ-1�������޿��÷�������������ֵ���������ϵ�������ʱ����connect�������صĴ����룬�μ�MSDN��

	SOCKET getSocket()����ȡSOCKETʵ��
	int getStreamPort()����ȡ��ʹ�õĶ˿ں�
	string getDisplayAddr()����ȡ���������ӵ�ַ��eg. http://www.rayion.com/desktop��
	serverList* getSrvStruct()�����سɹ����ӵķ�������Ϣ�ṹ��

	void showSrvInfo():�ڿ���̨��ʾ��ǰʹ�õķ�������Ϣ
*/
class cnctHandler
{
public:
	WSADATA wsaData;

	static cnctHandler *getInstance();

	int connectServer();             
	
	SOCKET getSocket(); 	
	string getDisplayAddr();
	int getStreamPort();
	serverList* getSrvStruct();

	void showSrvInfo();                 

	~cnctHandler();

private:
	/*
		���������ñ�����
	*/
	serverList *mySrvList;  

	//�����ļ����ļ���
	string fileName;    

	//��ȡ��Ӧ�����������±꣨������������ͷ�������ؽṹ���ַ��������ж�Ӧ�±���ַ���ֵ
	string getCfgByIndex(int index);   

	//��д����
	void getLabelMsg(string name, string buf);    

	//��ȡ�����ļ�
	bool readFile();  

	/*
		��ǰ����ʹ�õķ�������Ϣ
	*/
	SOCKET srvSocket;

	//��ַ�ṹ��ipv4/6��IP���˿ڣ�
	sockaddr_in srvAddr;   

	//�����Ĳ��ŵ�ַ��eg. http://www.rayion.com/desktop
	string displayAddr;        

	/*
		����Ϊ����ģʽ���
	*/

	//���캯��������Ϊ�洢�з�������Ϣ��XML��ʽ�����ļ�
	cnctHandler(string file);
	cnctHandler();

	//���ڹ��캯�����ã���ɹ��칤��
	void defaultSettings();

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