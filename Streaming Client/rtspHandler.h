/*--Author������--*/

#include "CommonHeaders.h"

/*
	rtspHandler�ࣺRTSP��Ϣ�Ĵ���
	
	������
	����������ı���벻��Ҫ���ʵ����1���͹��á�
	���к���Ψһ�ģ�ÿ��һ������Ҫ����һ�Ρ�
	�������������ˣ����ҲҪ���������仰˵�������ʵ�������ͷ�����һ��
	���������Է�����ʽ���֡�ע�⣺ֻ�ܱ���룬���ܷ���

	ʹ�ã�

	�����������ã�
	bool setHandler(string URI, WORD rtspVer, int port = 8554, bool enableUDP = false)�����ô�������URI��ַ��RTSP�汾����ý��˿ںš��Ƿ�ʹ��UDP

	string encodeMsg(int method)����ý��������룬������ý�巽����������������ͷ���������ַ�����ʽ������

	int decodeMsg(string msg)����ý��������룬����Ϊ��������ý����Ϣ������ֵΪ��ý������루200ΪOK������Ϊ����
*/
class rtspHandler
{
public:

	static rtspHandler *getInstance();

	//���ô�������URI��ַ��RTSP�汾��Ĭ��1.0�����˿ںţ�Ĭ��8554�������䷽����Ĭ��TCP��
	bool setHandler(string URI, string rtspVersion = "1.0", int port = 8554, bool enableUDP = false);

	//�����������ֵΪ����õ���Ϣ
	string encodeMsg(int method);   

	/*
		�����������ֵΪ������룬200ΪOK
		���棬�ͻ�����ʵֻ��Ҫ֪���Ƿ�OK����
	*/
	int decodeMsg(string msg);     

	//��ȡ�Ự��
	string getSession();

	/*
		��ȡrtsp�������������Ϣ
		����ֵʾ����"RTSP�汾��1.0\r\n�Ự�ţ�session����FFFFFFFF\r\n�˿ںţ�8554\r\n���䷽ʽ��UDP\r\n"
	*/
	string getHandlerInfo();

private:

	//�������к�
	unsigned int seqNum;   

	//�ͻ��˶˿ںţ�������ż��
	unsigned short streamingPort;

	//�Ƿ�ʹ��UDP������UDP������TCP
	bool enableUDP;

	//��ý���ַ
	string URI;          

	//�Ự��
	string session;      

	//RTSP�汾
	string rtspVersion;                        

	//�����Ϣ��Ŀǰ���ã�
	string trackNum;                           
	                           
	/*
		����ģʽ���
	*/

	rtspHandler();

	static rtspHandler *instance;

	//��ֹ���������Լ���ֵ
	rtspHandler(const rtspHandler &);
	rtspHandler & operator = (const rtspHandler &);

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