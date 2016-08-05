#ifndef __CONFIG_H_
#define __CONFIG_H_

#include <string>
using namespace std;

#define MSG_USER_QUITTHREAD		   WM_USER+1	
#define MSG_USER_MOBILE_STATEUS    WM_USER+2
#define MSG_USER_MOBILE_TIP        WM_USER+3


#define WM_BN_CLICK						WM_USER+801

typedef struct tagMSG_NOTIFY {
	DWORD	nThreadId	;
	HWND	hSubWnd		;
	UINT	msg			;
	void	*relMsgMem	;	//回接内存地址
} MSG_NOTIFY ;

typedef struct tagsThrd{
	HANDLE hThrd		 ;
	unsigned int hThrdId ;
} sThrd ;

//// 固定三张图片名字
//string button_out_pic ini.bmp;      // 初始按钮图片名字
//string button_in_pic Install.bmp;     // 未安装应用鼠标放上去的图片
//string Installbutton_in_pic  UnInstall.bmp;     // 安装应用鼠标放上去的图片
//string Installbutton_in_pic  click.bmp;     // 未安装应用鼠标点击图片
/// 小图 大图三张
typedef struct ADD_APP{
	string   appname;             /// 可执行文件名字
	bool     isLagerPic;          // true 是大图 false 是小图
	bool     isInstall;           //true 已安装 false 未安装
	int      type;                // 应用程序类型 0 表示大图片 1 普通程序 2 游戏类游戏
	int      version;             /// 10000
	ADD_APP(){
		appname ="";   //address
		isLagerPic =false  ;				//金额
		isInstall = false;
		type = 0;  
		version =1000;
	}
}ADD_APP_DATA;


typedef void (*BLOCKDATA)(char *pRecBuffer, int nRecLen , WPARAM wParam); 
#endif