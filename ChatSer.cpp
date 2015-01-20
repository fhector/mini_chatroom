//ChatSer.cpp : Defines the entry point for the application. use VC 6.0
//=========================================================================
//�����Э�飭�������Э��TCP
//˵����  Chat Server�������˳���
//=========================================================================
//VC 6.0   add  WSOCK32.LIB in Project -> Settings... ->  Link
#include <winsock.h>
#include <stdlib.h>
//--------------------------------------------------------------
// Global Variables:
HINSTANCE hInst;				// ʵ�������������������
HWND hWnd;						// �����ھ����������������
char Title[]="Chat-Server";		// ���ڱ��� The title bar text
int WinX0=500,WinY0=100,WinW=360,WinH=200;	// ����λ�á���С

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
//--------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hI,HINSTANCE hP,LPSTR lp,int nC)
{
	MSG msg;
	hInst = hI; // Store instance handle in our global variable
	WNDCLASS wc;

	memset(&wc,0,sizeof(WNDCLASS));

	wc.lpfnWndProc	= (WNDPROC)WndProc;
	wc.hInstance		= hI;
	wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wc.hbrBackground	= (HBRUSH)(COLOR_BTNFACE+1);
	wc.lpszClassName	= "Win32API";
	RegisterClass(&wc);
//���ⴰ��1��ʼ���ڶ��㣬��������ʾ��
	hWnd=CreateWindowEx(WS_EX_TOPMOST,"Win32API",Title,
		WS_DLGFRAME|WS_SYSMENU,
		WinX0,WinY0,WinW,WinH,			// ����λ�á���С
		NULL, NULL, hI, NULL);
   	if(!hWnd)   return FALSE;

   	ShowWindow(hWnd, nC);
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
//--------------------------------------------------------------
//��ť
HWND CreateButton(char *Titel,int x0,int y0,int w,int h,int ID,HWND hW,HINSTANCE hInst)
{
	return CreateWindowEx(WS_EX_PALETTEWINDOW,"BUTTON",Titel,
		WS_VISIBLE | WS_CHILD ,
		x0,y0,w,h, hW,(HMENU)ID,hInst,NULL);
}
//---------------------------------------------------------------------------
//���б༭��
HWND CreateEdit(char *Titel,int x0,int y0,int w,int h,int ID,HWND hW,HINSTANCE hInst)
{
	return CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT",Titel,
		WS_VISIBLE | WS_CHILD |
		    ES_LEFT | ES_MULTILINE | WS_HSCROLL ,
		x0,y0,w,h, hW,
		(HMENU)ID,hInst,NULL);
}
//---------------------------------------------------------------------------
//�����ı���
HWND CreateMemo(char *Titel,int x0,int y0,int w,int h,int ID,HWND hW,HINSTANCE hInst)
{
	return CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT",Titel,
		WS_VISIBLE | WS_CHILD |
		    ES_LEFT | ES_MULTILINE |ES_READONLY|
		    WS_HSCROLL | WS_VSCROLL,
		x0,y0,w,h,
		hW,(HMENU)ID,hInst,NULL);
}
//---------------------------------------------------------------------------
//������ı���������ı�
void Puts(HWND hW,int ID_EDIT,char *str)
{
	int nLen=GetWindowTextLength(GetDlgItem(hW,ID_EDIT));
	SendMessage(GetDlgItem(hW,ID_EDIT),EM_SETSEL,nLen,nLen);
	SendMessage(GetDlgItem(hW,ID_EDIT),EM_REPLACESEL,TRUE,(long)(LPCTSTR)str);
	nLen=GetWindowTextLength(GetDlgItem(hW,ID_EDIT));
	SendMessage(GetDlgItem(hW,ID_EDIT),EM_SETSEL,nLen,nLen);
	SendMessage(GetDlgItem(hW,ID_EDIT),EM_REPLACESEL,TRUE,(long)(LPCTSTR)"\r\n");
}
//---------------------------------------------------------------------------
//��ť
#define BUTTON1 501
#define BUTTON2 502
//---------------------------------------------------------------------------
//���б༭��
#define EDIT1 601
#define EDIT2 602
#define EDIT3 603
//---------------------------------------------------------------------------
//���б༭��
#define MEMO1 701
//---------------------------------------------------------------------------
#define UN 100		//100���û���¼��Ϣ
WSADATA ws;
SOCKET Ss,Cs,Csi[UN];
struct sockaddr_in SA,CA,CAi[11];
char aa[200];
char bb[200];
char User[UN][20];
int d;
int GetUserBySocket(SOCKET s)
{
	int i;
	for(i=0;i<UN;i++)
		if(Csi[i]==s)
			break;
	return i;
}
void Broadcast(SOCKET s,char *str)
{
	char a1[200];
	int i;
	int ui;
	ui = GetUserBySocket(s);
	for(i=0;i<UN;i++)
	{
		if(Csi[i]==-1) continue;
		wsprintf(a1,"%s:%s",User[ui],str);
		send(Csi[i],a1,strlen(a1),0);
	}
}
int NameCmp(char *str)
{
int i;
	for(i=0;i<UN;i++)
 	   if(!strcmp(User[i],str)) return 1;
	return 0;
}
void BroadcastToClients(HWND hW,SOCKET s,char *str)	//�������ͻ�ת����Ϣ���û���¼��ע��
{
	int i,j=1;
	for(i=0;i<UN;i++)
		if((Csi[i]==s) && (User[i][0]==0))
		{
			strcpy(aa,str);
			while(NameCmp(aa))				//�����û���¼��
				wsprintf(aa,"%s-%d",str,j++);
			if(j>1)
			{
				wsprintf(bb,"Your Name=%s!",aa);
				send(s,bb,strlen(bb),0);
			}
			strcpy(User[i],aa);
			wsprintf(aa,"Come in!");
			Broadcast(s,aa);				//�㲥�����û� Come in!��
			wsprintf(aa,"%s:Come in![%s:%d]",User[i],
			inet_ntoa(CAi[i].sin_addr),ntohs(CAi[i].sin_port));
			Puts(hW,MEMO1,aa);
			return;
		}
	Broadcast(s,str);			//�㲥�ͻ�������Ϣ
}
void DelUserName(HWND hW,SOCKET s,char *str)	//ɾ���ͷ����ӵ��û���¼��Ϣ
{
	int i;
	int ui;
	ui = GetUserBySocket(s);
	for(i=0;i<UN;i++)
		if(Csi[i]==s)
		{
			Broadcast(s,str);
			wsprintf(aa,"%s:%s[%s:%d]",User[ui],str,
			inet_ntoa(CAi[i].sin_addr),ntohs(CAi[i].sin_port));
			Puts(hW,MEMO1,aa);
			Csi[i]=-1;
			User[i][0]=0;
			return;
		}
}
void AddUser()	//��ӽ������ӵ��û���¼��Ϣ
{
	int i;
	for(i=0;i<UN;i++)
		if(Csi[i]==-1)
		{
			Csi[i]=Cs;
			CAi[i]=CA;
			User[i][0]=0;
			break;
		}
}
//---------------------------------------------------------------------------
//��Ϣ����
LRESULT CALLBACK WndProc(HWND hW, UINT msg, WPARAM wP, LPARAM lP)
{
int i;
	switch (msg)
	{
	case WM_DESTROY:	//�رճ��򴥷��¼�
		WSAAsyncSelect(Ss, hW, 0, 0);	//��Windowsע��Socket�����¼�
		closesocket(Ss);				//�ͷ�����
		WSACleanup( );					//ж�ض�̬���ӿ�WinSock DLL
		PostQuitMessage(0);
		break;
	case WM_CREATE:		//�����ʼ�������¼�
		WSAStartup(0x0101,&ws);								//װ�ض�̬���ӿ�WinSock DLL
		CreateButton("Send",10,22,50,20,BUTTON1,hW,hInst);	//Chat Server��������Ϣ��ť
		CreateButton("Start",10,2,50,20,BUTTON2,hW,hInst);	//��������ť
		CreateEdit("192.168.40.106",70,2,180,20,EDIT2,hW,hInst);	//Chat Server��IP��ַ�����
		CreateEdit("7777",260,2,90,20,EDIT3,hW,hInst);		//Chat Server��Port�˿������
		CreateEdit("Hello client!",70,22,280,20,EDIT1,hW,hInst);	//Chat Server��������Ϣ�����
		CreateMemo("Info.",	2,45,350,120,MEMO1,hW,hInst);	//�ͻ���¼��Ϣ��ʾ����
		SetFocus(GetDlgItem(hW,BUTTON1));
		EnableWindow(GetDlgItem(hW,BUTTON1),FALSE);
		EnableWindow(GetDlgItem(hW,BUTTON2),TRUE);
		for(i=0;i<UN;i++)	//��ʼ���ͻ���¼��Ϣ����
		{
			Csi[i]=-1;
			User[i][0]=0;
		}
		break;
	case WM_USER+1:				//Socket�����¼�
		switch(LOWORD(lP))
		{
		case FD_ACCEPT:			//Chat Client�Ľ�����������
			d=sizeof(CA);
			Cs=accept(Ss,(struct sockaddr *) &CA,&d);	//����Chat Client����������
			AddUser();									//��ӽ������ӵ��û���¼��Ϣ
			break;
		case FD_READ:			//�յ�Chat Client��Ϣ
			d=recv(wP,bb,sizeof(bb),0);//wP=Cs			//�����ջ�����

			bb[d]=0;
			BroadcastToClients(hW,wP,bb);				//�������ͻ�ת����Ϣ���û���¼��ע��
			break;
		case FD_CLOSE:			//Chat Client���ͷ���������
			wsprintf(bb,"Leave!");
			DelUserName(hW,wP,bb);						//ɾ���ͷ����ӵ��û���¼��Ϣ
			break;
		}
		break;
	case WM_COMMAND:			//�ؼ������¼�
		switch(wP)
		{
		case BUTTON1:			//����������Ϣ��ť
			GetDlgItemText(hW,EDIT1,bb,sizeof(bb));
			for(i=0;i<UN;i++)	//���¼�Ŀͻ�������Ϣ
			{
				if(Csi[i]==-1) continue;
				wsprintf(aa,"Server:%s",bb);
				send(Csi[i],aa,strlen(aa),0);
			}
			break;
		case BUTTON2:			//������������ť
			EnableWindow(GetDlgItem(hW,BUTTON2),FALSE);
			EnableWindow(GetDlgItem(hW,BUTTON1),TRUE);
			EnableWindow(GetDlgItem(hW,EDIT2),FALSE);
			EnableWindow(GetDlgItem(hW,EDIT3),FALSE);
			GetDlgItemText(hW,EDIT2,aa,sizeof(aa));
			GetDlgItemText(hW,EDIT3,bb,sizeof(bb));
			Ss=socket(AF_INET, SOCK_STREAM,0);	//����TCP�׽���Socket
			SA.sin_family=AF_INET;				//Chat Server�����ԣ���ַ���� AF_INET for Internet
			SA.sin_port = htons(atoi(bb));		//Chat Server�����ԣ��˿�
			SA.sin_addr.s_addr =inet_addr(aa);	//Chat Server�����ԣ�IP��ַ

			bind(Ss,(struct sockaddr *) &SA,sizeof(SA));				//����socket����
			WSAAsyncSelect(Ss,hW,WM_USER+1,FD_ACCEPT|FD_READ|FD_CLOSE);	//��Windowsע��Socket�����¼�
			listen(Ss,100);												//�������ӻ�������100������
			Beep(2000,100);
			break;
		}
		break;
	}
	return DefWindowProc(hW,msg,wP,lP);
}
//--------------------------------------------------------------

