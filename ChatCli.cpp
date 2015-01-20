//ChatCli.cpp : Defines the entry point for the application. use VC 6.0
//=========================================================================
//�������Э��TCP
//˵����  Chat Client�ͻ��˳���
//=========================================================================
//VC 6.0   add  WSOCK32.LIB in Project -> Settings... ->  Link
#include <winsock.h>
#include <stdlib.h>
//--------------------------------------------------------------
// Global Variables:
HINSTANCE hInst;				// ʵ�������������������
HWND hWnd;						// �����ھ����������������
char Title[]="Chat-Client";		// ���ڱ��� The title bar text
int WinX0=250,WinY0=50,WinW=360,WinH=200;	// ����λ�á���С

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
//		WinX0,WinY0,WinW,WinH,			// ����λ�á���С
		CW_USEDEFAULT,CW_USEDEFAULT,WinW,WinH,	// ����λ�� Windows selects the default position ����С
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
#define EDIT4 604
//---------------------------------------------------------------------------
//���б༭��
#define MEMO1 701
//---------------------------------------------------------------------------
WSADATA ws;
SOCKET Ss,Cs;
struct sockaddr_in SA,CA;
char aa[200];
char bb[200];
int d;
//---------------------------------------------------------------------------
//��Ϣ����
LRESULT CALLBACK WndProc(HWND hW, UINT msg, WPARAM wP, LPARAM lP)
{
int i;
	switch (msg)
	{
	case WM_DESTROY:	//�رճ��򴥷��¼�
		WSAAsyncSelect(Ss, hW, 0, 0);	//��Windowsע��Socket�����¼�
		closesocket(Cs);				//�ͷ�����
		WSACleanup( );					//ж�ض�̬���ӿ�WinSock DLL
		PostQuitMessage(0);
		break;
	case WM_CREATE:		//�����ʼ�������¼�
		WSAStartup(0x0101,&ws);								//װ�ض�̬���ӿ�WinSock DLL
		CreateButton("Send",10,22,50,20,BUTTON1,hW,hInst);	//������Ϣ��ť
		CreateButton("Conn",10,2,50,20,BUTTON2,hW,hInst);	//����Chat Server��ť
		CreateEdit("127.0.0.1",70,2,120,20,EDIT2,hW,hInst);	//Chat Server��IP��ַ�����
		CreateEdit("7777",200,2,60,20,EDIT3,hW,hInst);		//Chat Server��Port��ַ�����
		CreateEdit("Name",270,2,80,20,EDIT4,hW,hInst);		//�û���¼�������
		CreateEdit("Hello!",70,22,280,20,EDIT1,hW,hInst);	//������Ϣ�����
		CreateMemo("Info.",	2,45,350,120,MEMO1,hW,hInst);	//������Ϣ��ʾ����
		SetFocus(GetDlgItem(hW,BUTTON2));
		EnableWindow(GetDlgItem(hW,BUTTON1),FALSE);
		break;
	case WM_USER+1:				//Socket�����¼�
		switch(LOWORD(lP))
		{
		case FD_CONNECT:		//Chat Server�Ľ�������Ӧ��
			//			Puts(hW,MEMO1,"FD_CONNECT->Ok!");
			break;
		case FD_READ:			//�յ�Chat Server��Ϣ
			d=recv(wP,bb,sizeof(bb),0);//wP=Cs	//�����ջ�����
			bb[d]=0;
			wsprintf(aa,"FD_READ->:%s",bb);
			Puts(hW,MEMO1,bb);
			break;
		case FD_CLOSE:			//Chat Server���ͷ���������
			Puts(hW,MEMO1,"Server Stop!");
			break;
		}
		break;
	case WM_COMMAND:		//�ؼ������¼�
		switch(wP)
		{
		case BUTTON1:		//���·�����Ϣ��ť
			GetDlgItemText(hW,EDIT1,bb,sizeof(bb));
			send(Cs,bb,strlen(bb),0);				//��Chat Server������Ϣ
			break;
		case BUTTON2:		//��������Chat Server��ť
			GetDlgItemText(hW,EDIT2,aa,sizeof(aa));
			GetDlgItemText(hW,EDIT3,bb,sizeof(bb));
			Cs = socket(PF_INET, SOCK_STREAM, 0);	//����TCP�׽���Socket
			SA.sin_family = PF_INET;				//Զ��Chat Server�����ԣ���ַ���� AF_INET for Internet
			SA.sin_port = htons(atoi(bb));			//Զ��Chat Server�����ԣ��˿�
			SA.sin_addr.s_addr =inet_addr(aa);		//Զ��Chat Server�����ԣ�IP��ַ
			d=sizeof(SA);
			i=connect(Cs,(struct sockaddr *)&SA,d);	//����Chat Server
			if(!i)
			{									//����Chat Server�ɹ�
				EnableWindow(GetDlgItem(hW,BUTTON2),FALSE);
				EnableWindow(GetDlgItem(hW,BUTTON1),TRUE);
				EnableWindow(GetDlgItem(hW,EDIT2),FALSE);
				EnableWindow(GetDlgItem(hW,EDIT3),FALSE);
				EnableWindow(GetDlgItem(hW,EDIT4),FALSE);
				WSAAsyncSelect(Cs,hW,WM_USER+1,FD_CONNECT|FD_READ|FD_CLOSE);//��Windowsע��Socket�����¼�
			   GetDlgItemText(hW,EDIT4,bb,sizeof(bb));
			   send(Cs,bb,strlen(bb),0);		//���ӳɹ�����Chat Server�����û���¼��
			}
			else
				Puts(hW,MEMO1,"connect fail!");	//����Chat Serverʧ��
			Beep(1000,100);
			break;
		}
		break;
	}
	return DefWindowProc(hW,msg,wP,lP);
}
//--------------------------------------------------------------


