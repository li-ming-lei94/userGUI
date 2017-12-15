#include "settings.h"  
#include "spb.h"
#include "24cxx.h"
#include "calendar.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/7/20
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
_system_setings systemset;	  
#define SYS_MENU_SIZE 			19		//��Ŀ����	
	 
//ϵͳ������Ŀ¼��
u8*const sysset_mmenu_tbl[GUI_LANGUAGE_NUM][SYS_MENU_SIZE]=//ϵͳһ��Ŀ¼�ĸ���
{
	{
	"1.ʱ������",
	"2.��������",
	"3.����ʱ������",
	"4.���ӿ�������",
	"5.������������",
	"6.��������",
	"7.�����������",
	"8.���ֲ�����ģʽ����",
	"9.�������ȿ�������",
	"10.VS1053����&3D����",
	"11.VS1053��������",
	"12.VS1053��������", 
	"13.��������",
 	"14.��ĻУ׼",
	"15.ϵͳ�ļ�����",
	"16.�ָ�Ĭ������",
	"17.ϵͳ��Ϣ",
	"18.ϵͳ״̬", 
	"19.����"
	},
	{			   								
	"1.�r�g�O��",
	"2.�����O��",
	"3.�[犕r�g�O��",
	"4.�[��_�P�O��",
	"5.�[���O��",
	"6.�Z���O��",
	"7.���a����O��",
	"8.����������ģʽ�O��",
	"9.���d�����_�P�O��", 
	"10.VS1053����&3D�O��",
	"11.VS1053�����O��",
	"12.VS1053�����O��", 
	"13.�����O��",
 	"14.��ĻУ��",
	"15.ϵ�y�ļ�����",
	"16.�֏�Ĭ�J�O��",
	"17.ϵ�y��Ϣ",
	"18.ϵ�y��B", 
	"19.�P�"
	}, 	 
	{
	"1.TIME SET",
	"2.DATE SET",
	"3.ALARM TIME SET",
	"4.ALARM ON/OFF SET",
	"5.ALARM RING SET",
	"6.LANGUAGE SET",
	"7.DIGITAL PHOTO FRAME SET",
	"8.AUDIO PLAYER MODE SET",
	"9.ONBOARD SPEAKER SET",
	"10.VS1053 VOL&3D SET",
	"11.VS1053 TREBLE SET",
	"12.VS1053 BASS SET", 
	"13.BACKLIGHT SET",
 	"14.TOUCH SCREEN ADJUST", 
	"15.SYSTEM FILE UPDATE",
	"16.RESTORE DEFAULT SET",
	"17.SYSTEM INFORMATION",
	"18.SYSTEM STATUS", 
	"19.ABOUT"
	},
};	    
//���������б�
u8*const sysset_language_tbl[GUI_LANGUAGE_NUM]=
{
	"��������","���w����","English",
};  	    
//���������б�
u8*const sysset_ring_tbl[GUI_LANGUAGE_NUM][4]=
{
{"����1","����2","����3","����4",},
{"�1","�2","�3","�4",},
{"RING 1","RING 2","RING 3","RING 4",},	 
};
//����/ͼƬ����ģʽ�����б�
u8*const sysset_avplaymode_tbl[GUI_LANGUAGE_NUM][3]=
{
{"ȫ��ѭ��","�������","����ѭ��"},
{"ȫ��ѭ�h","�S�C����","����ѭ�h"},
{"LOOP PLAY","SHUFFLE PLAY","SINGLE PLAY"},
};
//�������ȿ��������б�
u8*const sysset_speakermode_tbl[GUI_LANGUAGE_NUM][2]=
{
{"�ر�����","��������"},
{"�P�]����","�_������"},
{"Speaker OFF","Speaker ON"},
};
//VS1053����&3D�O��
u8*const sysset_vs1053vol3d_tbl[2][GUI_LANGUAGE_NUM]=
{
{"������:","������:","Volume:",}, 
{"3DЧ��:","3DЧ��:","3D Effect:"},
};
//VS1053 EQ����
u8*const sysset_vs1053eq_tbl[2][GUI_LANGUAGE_NUM]=
{ 
{"����Ƶ��:","�����l��:","Center Freq:",}, 
{"����:","����:","Gain:"}, 
}; 
//ϵͳ���������ʾ��Ϣ 
u8*const sysset_system_remaindmsg_tbl[3][GUI_LANGUAGE_NUM]=
{
{"��ȷ�ϸ���ϵͳ�ļ�?","���_�J����ϵ�y�ļ�?","Are you sure to update?"},
{"��ȷ�ϻָ�Ĭ������?","���_�J�֏�Ĭ�J�O��?","Are you sure to restore?"},
{"�ָ�Ĭ��������...","�֏�Ĭ�J�O����...","Default set restoring...",},
};
//ϵͳ���¸�����ʾ��Ϣ
u8*const sysset_system_update_cpymsg_tbl[2][GUI_LANGUAGE_NUM]=
{
{"���ڸ���:","�����}�u:"," Copying:",},
{"��ǰ�ļ���:","��ǰ�ļ��A:","Cur Folder:",},
}; 
//ϵͳ������ʾ
u8*const sysset_system_update_msg_tbl[GUI_LANGUAGE_NUM]=
{"ϵͳ���ڸ���...","ϵ�y���ڸ���...","SYSTEM Updating..."};
//ϵͳ���½����ʾ
u8*const sysset_system_update_err_tbl[3][GUI_LANGUAGE_NUM]=
{
{"ϵͳ�ļ����³ɹ�!","ϵ�y�ļ����³ɹ�!","SYSTEM file lost!",},
{"ϵͳ�ļ���ʧ!","ϵ�y�ļ��Gʧ!","SYSTEM file lost!",},
{"�û���ֹ����!","�Ñ��Kֹ����!","User stop update!",},
};
//ϵͳ��Ϣ��ע��
u8*const sysset_system_info_caption_tbl[9][GUI_LANGUAGE_NUM]=
{
{"������:","̎����:","CPU:"},
{"�ڴ�:","�ȴ�:","RAM:"},
{"SD��:","SD��:","SD Card:"},
{"FLASH��:","FLASH�P:","FLASH Disk:"}, 
{"����ϵͳ:","����ϵ�y:","OS:"},
{"ͼ�ν���:","�D�ν���:","GUI:"},
{"Ӳ��ƽ̨:","Ӳ��ƽ�_:","Hardware:"},
{"��Ȩ��Ϣ:","�����Ϣ:","Copyright:"},
{"����֧��:","���g֧��:","Tech Support:"},
};
//ϵͳ��ʾ��Ϣ��
u8*const sysset_system_info_msg_tbl[9]=
{
"STM32F103ZET6 @ 72Mhz",
"64KB+1024KB",
"MB",
"KB", 
"uCOS II ",
"ALIENTEK GUI ",
"ս��STM32F103 ",
"������������",
"www.openedv.com",
}; 
//ϵͳ״��ʾ��Ϣ
u8*const sysset_sysstatus_tbl[4][GUI_LANGUAGE_NUM]=
{
{"CPUʹ����:","CPUʹ����:","CPU USAGE:",},
{"�ڲ��ڴ�ʹ����:","�ڲ��ڴ�ʹ����:","IN MEMORY USAGE:",},
{"�ⲿ�ڴ�ʹ����:","�ⲿ�ڴ�ʹ����:","EX MEMORY USAGE:"},
{"�¶�:","�ض�:","TEMP:"},
};

//ϵͳ������ʾ��Ϣ
u8*const sysset_system_about_caption_tbl[2][GUI_LANGUAGE_NUM]=
{
{"ALIENTEKս��V3","ALIENTEK��ŞV3","ALIENTEK WarShip V3",},
{"��Ʒ���к�","�aƷ����̖","Serial Number",},
};
//�������ú����ʾ
u8*const sysset_remindmsg_tbl[2][GUI_LANGUAGE_NUM]=
{
{"����������","����������","Updating",},
{"ϵͳ���ڸ���������,���Ժ�...","ϵ�y���ڸ���������,Ո�Ժ�...","System updating,Please wait...",}, 
}; 
////////////////////////////////////////////////////////////////////////////////////////////

#define SYSSET_BTN1_WIDTH			50			//һ�ఴ������(�Ӽ���ť)
#define SYSSET_BTN1_HEIGHT			40			//һ�ఴ���߶�(�Ӽ���ť)
#define SYSSET_BTN2_WIDTH			60			//���ఴ������(ȷ��/ȡ����ť)
#define SYSSET_BTN2_HEIGHT			30			//���ఴ���߶�(ȷ��/ȡ����ť)
#define SYSSET_BTN3_WIDTH			100			//���ఴ������(������ť�Ŀ���)
#define SYSSET_EDIT_WIDTH			50			//���ֱ༭���Ŀ���
#define SYSSET_EDIT_HEIGHT			25			//���ֱ༭���ĸ߶�
 
#define SYSSET_INWIN_FONT_COLOR		0X736C		//0XAD53		//������Ƕ����ɫ
#define SYSSET_EDIT_BACK_COLOR		0XE79E		//��ʾ����������ɫ
 
/////////////////////////////////////////////////////////////////////////////////////////////

//ʱ��/��������
//x,y:��������(���ڳߴ��Ѿ��̶��˵�)	 
//hour,min:ʱ��
//caption:��������				  
//����ֵ:0,ok;����,ȡ�����ߴ���.
u8 sysset_time_set(u16 x,u16 y,u8 *hour,u8 *min,u8*caption) 
{
	u8 rval=0,res;
	u8 i;
	_window_obj* twin=0;	//����
 	_btn_obj * tbtn[6];		//�ܹ�������ť:0,ʱ�ӼӰ�ť;1,ʱ�Ӽ���ť;2,���ӼӰ�ť;3,���Ӽ���ť;4,ȷ�ϰ�ť;5,ȡ����ť.		  
 	twin=window_creat(x,y,150,199,0,1|1<<5,16);//��������
 	tbtn[0]=btn_creat(x+20,y+42,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);			//������ť
	tbtn[1]=btn_creat(x+20,y+42+67,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//������ť
	tbtn[2]=btn_creat(x+20+60,y+42,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//������ť
	tbtn[3]=btn_creat(x+20+60,y+42+67,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//������ť
	tbtn[4]=btn_creat(x+10,y+42+117,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);		//������ť
	tbtn[5]=btn_creat(x+10+70,y+42+117,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);	//������ť
	for(i=0;i<6;i++)
	{
		if(tbtn[i]==NULL)
		{
			rval=1;
			break;
		}
		if(i<4)//�Ӽ�����
		{
			tbtn[i]->bcfucolor=BLACK;//�ɿ�ʱΪ��ɫ
			tbtn[i]->bcfdcolor=WHITE;//����ʱΪ��ɫ			
			tbtn[i]->bkctbl[0]=0X453A;//�߿���ɫ
			tbtn[i]->bkctbl[1]=0X5DDC;//��һ�е���ɫ				
			tbtn[i]->bkctbl[2]=0X5DDC;//�ϰ벿����ɫ
			tbtn[i]->bkctbl[3]=0X453A;//�°벿����ɫ
		}else//ȷ�Ϻ�ȡ������
		{
			tbtn[i]->bkctbl[0]=0X8452;//�߿���ɫ
			tbtn[i]->bkctbl[1]=0XAD97;//��һ�е���ɫ				
			tbtn[i]->bkctbl[2]=0XAD97;//�ϰ벿����ɫ
			tbtn[i]->bkctbl[3]=0X8452;//�°벿����ɫ
		}
		if(i==0||i==2)tbtn[i]->caption="��";
		if(i==1||i==3)tbtn[i]->caption="��";
		if(i==4)tbtn[i]->caption=(u8*)GUI_OK_CAPTION_TBL[gui_phy.language];
		if(i==5)tbtn[i]->caption=(u8*)GUI_CANCEL_CAPTION_TBL[gui_phy.language];				
	}
    if(twin==NULL)rval=1;
	else
	{
		twin->caption=caption;
		twin->windowbkc=APP_WIN_BACK_COLOR;	     
	}
	if(rval==0)//�޴���
	{
		window_draw(twin);					//��������
		for(i=0;i<6;i++)btn_draw(tbtn[i]);	//����ť
		gui_fill_rectangle(x+20+1,y+42+41,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);  		//���ʱ�ӱ���
		gui_fill_rectangle(x+20+60+1,y+42+41,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);	//�����ӱ���
 		app_show_nummid(x+20,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*hour,2,16,BLACK,SYSSET_EDIT_BACK_COLOR);
		app_show_nummid(x+20+60,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*min,2,16,BLACK,SYSSET_EDIT_BACK_COLOR);  
		while(rval==0)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
//			delay_ms(1000/OS_TICKS_PER_SEC);		//��ʱһ��ʱ�ӽ���
// 			if(system_task_return)
//			{
//				rval=1;			//ȡ������
//				break;			//TPAD����	
//			}
			for(i=0;i<6;i++)
			{
				res=btn_check(tbtn[i],&in_obj);//ȷ�ϰ�ť���
				if(res)
				{
					if((tbtn[i]->sta&0X80)==0)//����Ч����
					{
						switch(i)
						{
							case 0://ʱ�����Ӱ�ť������
								(*hour)++;
								if(*hour>23)*hour=0;
								break;
							case 1://ʱ�Ӽ��ٰ�ť������	  
								if(*hour)(*hour)--;
								else *hour=23;
								break;
							case 2://�������Ӱ�ť������
								(*min)++;
								if(*min>59)(*min)=0;
								break;
							case 3://���Ӽ��ٰ�ť������	  
								if(*min)(*min)--;
								else *min=59;
								break;
							case 4://ȷ�ϰ�ť����  
								rval=0XFF;
								break;	  
							case 5://ȡ����ť����	  
								rval=1;
								break;
 						}
					}
					app_show_nummid(x+20,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*hour,2,16,BLACK,SYSSET_EDIT_BACK_COLOR);
					app_show_nummid(x+20+60,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*min,2,16,BLACK,SYSSET_EDIT_BACK_COLOR);  

				}
			}
			
		}
 	}
	window_delete(twin);				//ɾ������
	for(i=0;i<6;i++)btn_delete(tbtn[i]);//ɾ����ť					   
//	system_task_return=0;				//����˳���־
	if(rval==0XFF)return 0;
	return rval;
}
//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//year:���
//����ֵ:������ǲ�������.1,��.0,����
u8 sysset_is_leap_year(u16 year)
{			  
	if(year%4==0) //�����ܱ�4����
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400���� 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	
//��������
//x,y:��������(���ڳߴ��Ѿ��̶��˵�)
//year,month,date:������
//caption:��������
//����ֵ:0,ok;����,ȡ�����ߴ���.
u8 sysset_date_set(u16 x,u16 y,u16 *year,u8 *month,u8 *date,u8*caption) 
{
	u8 rval=0,res;
	u8 i;
	u8 maxdate=31;			//2�·���������
	_window_obj* twin=0;	//����
 	_btn_obj * tbtn[8];		//�ܹ��˸���ť:0,��ݼӰ�ť;1,��ݼ���ť;2,�·ݼӰ�ť;3�·ݼ���ť;4,���ڼӰ�ť;5,���ڼ���ť;6,ȷ�ϰ�ť;7,ȡ����ť		  
 	twin=window_creat(x,y,190,199,0,1|1<<5,16);//��������
 	tbtn[0]=btn_creat(x+10,y+42,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);			//������ť
	tbtn[1]=btn_creat(x+10,y+42+67,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//������ť
	tbtn[2]=btn_creat(x+10+60,y+42,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//������ť
	tbtn[3]=btn_creat(x+10+60,y+42+67,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//������ť
	tbtn[4]=btn_creat(x+10+120,y+42,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//������ť
	tbtn[5]=btn_creat(x+10+120,y+42+67,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);	//������ť

	tbtn[6]=btn_creat(x+20,y+42+117,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);		//������ť
	tbtn[7]=btn_creat(x+20+90,y+42+117,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);	//������ť
 	for(i=0;i<8;i++)
	{
		if(tbtn[i]==NULL)
		{
			rval=1;
			break;
		}
		if(i<6)//�Ӽ�����
		{
			tbtn[i]->bcfucolor=BLACK;//�ɿ�ʱΪ��ɫ
			tbtn[i]->bcfdcolor=WHITE;//����ʱΪ��ɫ			
			tbtn[i]->bkctbl[0]=0X453A;//�߿���ɫ
			tbtn[i]->bkctbl[1]=0X5DDC;//��һ�е���ɫ				
			tbtn[i]->bkctbl[2]=0X5DDC;//�ϰ벿����ɫ
			tbtn[i]->bkctbl[3]=0X453A;//�°벿����ɫ
		}else//ȷ�Ϻ�ȡ������
		{
			tbtn[i]->bkctbl[0]=0X8452;//�߿���ɫ
			tbtn[i]->bkctbl[1]=0XAD97;//��һ�е���ɫ				
			tbtn[i]->bkctbl[2]=0XAD97;//�ϰ벿����ɫ
			tbtn[i]->bkctbl[3]=0X8452;//�°벿����ɫ
		}
		if(i==0||i==2||i==4)tbtn[i]->caption="��";
		if(i==1||i==3||i==5)tbtn[i]->caption="��";
		if(i==6)tbtn[i]->caption=(u8*)GUI_OK_CAPTION_TBL[gui_phy.language];;
		if(i==7)tbtn[i]->caption=(u8*)GUI_CANCEL_CAPTION_TBL[gui_phy.language];;				
	}
    if(twin==NULL)rval=1;
	else
	{
		twin->caption=caption;
		twin->windowbkc=APP_WIN_BACK_COLOR;	     
	}
	if(rval==0)//�޴���
	{
		window_draw(twin);					//��������
		for(i=0;i<8;i++)btn_draw(tbtn[i]);	//����ť
		gui_fill_rectangle(x+10+1,y+42+41,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);  		//�����ݱ���
		gui_fill_rectangle(x+10+60+1,y+42+41,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);	//����·ݱ���
 		gui_fill_rectangle(x+10+120+1,y+42+41,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);	//������ڱ���

 		app_show_nummid(x+10,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*year,4,16,BLACK,SYSSET_EDIT_BACK_COLOR);
		app_show_nummid(x+10+60,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*month,2,16,BLACK,SYSSET_EDIT_BACK_COLOR);  
		app_show_nummid(x+10+120,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*date,2,16,BLACK,SYSSET_EDIT_BACK_COLOR);  
		while(rval==0)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
//			delay_ms(1000/OS_TICKS_PER_SEC);		//��ʱһ��ʱ�ӽ���
// 			if(system_task_return)
//			{
//				rval=1;			//ȡ������
//				break;			//TPAD����	
//			}
			for(i=0;i<8;i++)
			{
				res=btn_check(tbtn[i],&in_obj);//ȷ�ϰ�ť���
				if(res)
				{
					if((tbtn[i]->sta&0X80)==0)//����Ч����
					{
						if(*month==2)
						{
							if(sysset_is_leap_year(*year))maxdate=29;//�������2�·�
	 						else maxdate=28;	  
						}else maxdate=31;  
						switch(i)
						{
							case 0://������Ӱ�ť������
								(*year)++;
								if(*year>2100)*year=2000;
								break;
							case 1://��ݼ��ٰ�ť������	  
								if(*year>2000)(*year)--;
								else *year=2100;
								break;
							case 2://�·����Ӱ�ť������
								(*month)++;
								if(*month>12)(*month)=1;
								break;
							case 3://�·ݼ��ٰ�ť������	  
								if(*month>1)(*month)--;
								else *month=12;
								break;
							case 4://�������Ӱ�ť������
								(*date)++;
								if(*date>maxdate)(*date)=1;
								break;
							case 5://���ڼ��ٰ�ť������	  
								if(*date>1)(*date)--;
								else *date=maxdate;
								break;
							case 6://ȷ�ϰ�ť����  
								rval=0XFF;
								break;	  
							case 7://ȡ����ť����  
								rval=1;
								break;
 						}
					}   
			 		app_show_nummid(x+10,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*year,4,16,BLACK,SYSSET_EDIT_BACK_COLOR);
					app_show_nummid(x+10+60,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*month,2,16,BLACK,SYSSET_EDIT_BACK_COLOR);  
					app_show_nummid(x+10+120,y+42+41,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*date,2,16,BLACK,SYSSET_EDIT_BACK_COLOR);  
				}
			}
			
		}
 	}
	window_delete(twin);				//ɾ������
	for(i=0;i<8;i++)btn_delete(tbtn[i]);//ɾ����ť					   
//	system_task_return=0;				//����˳���־
	if(rval==0XFF)return 0;
	return rval;
}	  
//VS1053������3D��ʾ
//x,y:sysset_wm8978_vol3d_set����������
//mask:
//[0]:����;0~100% 
//[1]:3DЧ��ֵ:0~3
//[7]:���µ�λ
//��ʾ������
void sysset_vs1053_vol3d_show(u16 x,u16 y,u8 mask,u8 val)
{ 
   	if(mask&(1<<7))
	{
		gui_show_string("%",x+200-10-6,y+32+12,20,12,12,SYSSET_INWIN_FONT_COLOR);	//��ʾ�ٷֱ�
 	}
	gui_phy.back_color=APP_WIN_BACK_COLOR;//���ñ���ɫ
	if(mask&(1<<0))//��ʾ����
	{						 
 		gui_fill_rectangle(x+200-10-24,y+32+12,18,12,APP_WIN_BACK_COLOR);//���֮ǰ����ʾ
		if(val>=100)
		{
			val=100;
			gui_show_num(x+200-10-24,y+32+12,3,SYSSET_INWIN_FONT_COLOR,12,val,0);//��ʾ�����ٷֱ� 
		}else 
		{
			gui_show_num(x+200-10-18,y+32+12,2,SYSSET_INWIN_FONT_COLOR,12,val,0);//��ʾ�����ٷֱ�   
 	  	}
 	}
	if(mask&(1<<1))//��ʾ3d����ֵ
	{ 
		gui_fill_rectangle(x+200-10-12,y+32+12+50,12,12,APP_WIN_BACK_COLOR);//���֮ǰ����ʾ
		gui_show_num(x+200-10-12,y+32+12+50,2,SYSSET_INWIN_FONT_COLOR,12,val,0);//��ʾ��ֵ 
 	}  
}
//VS1053 EQ������ʾ
//x,y:����
//eqx:��ǰ���õ�eq���,0/1
//mask:
//[0]:����Ƶ�ʱ�־;0~14 
//[1]:Ƶ������:0~15
//[7]:���µ�λ
//��ʾ������
void sysset_vs1053_eq_show(u16 x,u16 y,u8 mask,u8 eqx,u8 val)
{ 
	signed char tdbval; 
   	if(mask&(1<<7))
	{
		if(eqx==0)gui_show_string("Hz",x+200-10-12,y+32+12,20,12,12,SYSSET_INWIN_FONT_COLOR);	//��ʾHz
 		else gui_show_string("Khz",x+200-10-18,y+32+12,24,12,12,SYSSET_INWIN_FONT_COLOR);		//��ʾKhz
		gui_show_string("dB",x+200-10-12,y+32+12+50,20,12,12,SYSSET_INWIN_FONT_COLOR);			//��ʾdB 
 	}
	gui_phy.back_color=APP_WIN_BACK_COLOR;//���ñ���ɫ
	if(mask&(1<<0))//��ʾ��ǰ���õ�����Ƶ��
	{ 
		gui_fill_rectangle(x+200-10-6*(2+eqx)-6*2,y+32+12,6*2,12,APP_WIN_BACK_COLOR);			//���֮ǰ����ʾ 
 		if(eqx==0)val=(val+2)*10;
		else val=val+1;  
		app_show_float(x+200-10-6*(2+eqx),y+32+12,val,0,3,12,SYSSET_INWIN_FONT_COLOR,APP_WIN_BACK_COLOR);//��ʾ��ֵ  
	}
	if(mask&(1<<1))//��ʾƵ������
	{  
		if(eqx==0)
		{
			app_show_float(x+200-10-12,y+32+12+50,val,0,3,12,SYSSET_INWIN_FONT_COLOR,APP_WIN_BACK_COLOR);//��ʾ��ֵ 
		}else 
		{
			tdbval=((signed char)val-8)*15;	 
			app_show_float(x+200-10-12,y+32+12+50,tdbval,1,5,12,SYSSET_INWIN_FONT_COLOR,APP_WIN_BACK_COLOR);//��ʾ��ֵ 
		}
	}		
}
//������������
//x,y:��ʾ������
//����:180,�߶�:180
//caption:����
//vsset:vs1003������
//����ֵ:0,ok;����,ȡ�����ߴ���.
u8 sysset_bklight_set(u16 x,u16 y,u8* caption,u16 *bkval)
{
 	u8 rval=0,res;
  	_window_obj* twin=0;			//����
 	_btn_obj * rbtn=0;				//ȡ����ť		  
 	_btn_obj * okbtn=0;				//ȷ����ť		  
	_progressbar_obj *bkprgb;		//�������ù�����
	u8 *buf;
	
 	twin=window_creat(x,y,180,150,0,1|1<<5,16);				//��������
	bkprgb=progressbar_creat(x+15,y+32+35,150,20,0X20);		//����������,����ʾ����
	buf=gui_memin_malloc(100);
	if(bkprgb==NULL||buf==NULL)rval=1;
 													 
  	okbtn=btn_creat(x+20,y+110,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);		//������ť
 	rbtn=btn_creat(x+20+60+20,y+110,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);	//������ť
	if(twin==NULL||rbtn==NULL||okbtn==NULL||rval)rval=1;
	else
	{
		//���ڵ����ֺͱ���ɫ
		twin->caption=caption;
		twin->windowbkc=APP_WIN_BACK_COLOR;	
		//���ذ�ť����ɫ    
		rbtn->bkctbl[0]=0X8452;		//�߿���ɫ
		rbtn->bkctbl[1]=0XAD97;		//��һ�е���ɫ				
		rbtn->bkctbl[2]=0XAD97;		//�ϰ벿����ɫ
		rbtn->bkctbl[3]=0X8452;		//�°벿����ɫ
		okbtn->bkctbl[0]=0X8452;	//�߿���ɫ
		okbtn->bkctbl[1]=0XAD97;	//��һ�е���ɫ				
		okbtn->bkctbl[2]=0XAD97;	//�ϰ벿����ɫ
		okbtn->bkctbl[3]=0X8452;	//�°벿����ɫ

		rbtn->caption=(u8*)GUI_CANCEL_CAPTION_TBL[gui_phy.language];	//����Ϊȡ��
 		okbtn->caption=(u8*)GUI_OK_CAPTION_TBL[gui_phy.language];		//����Ϊ����
		bkprgb->curpos=*bkval;					//ȡ�õ�ǰ�����ðٷֱ�
   		bkprgb->totallen=100;					//�ܳ���Ϊ100
 											 
 		window_draw(twin);						//��������
		btn_draw(rbtn);							//����ť
		btn_draw(okbtn);						//����ť			    
		progressbar_draw_progressbar(bkprgb);	//��ʾ������
		gui_phy.back_color=APP_WIN_BACK_COLOR;	//���ñ���ɫ  
		if(bkprgb->curpos)sprintf((char*)buf,"%d%%",bkprgb->curpos);//�ٷֱ�
		else sprintf((char*)buf,"auto");	 						//auto
		gui_show_string(buf,x+15,y+32+22,24,12,12,SYSSET_INWIN_FONT_COLOR);//��ʾ�ٷֱ�/auto
 	}
 	while(rval==0)
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
//		delay_ms(1000/OS_TICKS_PER_SEC);		//��ʱһ��ʱ�ӽ���
//		if(system_task_return)
//		{
//			rval=1;			//ȡ������
//			break;			//TPAD����	
//		}
		res=btn_check(rbtn,&in_obj);			//ȡ����ť���
		if(res&&((rbtn->sta&0X80)==0))			//����Ч����
		{
			rval=1;
			break;//�˳�
		}
 		res=btn_check(okbtn,&in_obj);			//ȷ����ť���
 		if(res&&((okbtn->sta&0X80)==0))			//����Ч����
		{		 
			rval=0XFF;		   
			break;//�˳�
		}    
		res=progressbar_check(bkprgb,&in_obj);
		if(res&&(*bkval!=bkprgb->curpos))		//�������Ķ���
		{
 			*bkval=bkprgb->curpos;				//�������µĽ�� 
 			gui_fill_rectangle(x+15,y+32+22,24,12,APP_WIN_BACK_COLOR);	//����ɫ 
			if(bkprgb->curpos)
			{
				sprintf((char*)buf,"%d%%",bkprgb->curpos);//�ٷֱ�
//				if(lcddev.id==0X1963)LCD_SSD_BackLightSet(*bkval);
//				else LCD_BLPWM_VAL=*bkval;		//���ñ������� 
			}else 
			{
				sprintf((char*)buf,"auto");	//auto
				app_lcd_auto_bklight();		//�Զ��������
			} 
			gui_show_string(buf,x+15,y+32+22,24,12,12,SYSSET_INWIN_FONT_COLOR);//��ʾ�ٷֱ�/auto
		}  		 
	}
	gui_memin_free(buf);			//�ͷ��ڴ�
	window_delete(twin);			//ɾ������
	btn_delete(rbtn);				//ɾ����ť	
	progressbar_delete(bkprgb);		//ɾ��������  
//	system_task_return=0;			//����˳���־
	if(rval==0XFF)return 0;
	return rval;
}
//�����ⲿ��������
u16 cpymsg_x;
u16 cpymsg_y;
 //ϵͳ������ʱ��,������ʾ���½���
//*pname:�����ļ�����
//pct:�ٷֱ�
//mode:ģʽ
//[0]:�����ļ���
//[1]:���°ٷֱ�pct
//[2]:�����ļ���
//[3~7]:����
//����ֵ:0,����;
//       1,��������			  
u8 sysset_system_update_cpymsg(u8*pname,u8 pct,u8 mode)
{		
	u16 filled; 
//	if(system_task_return)return 1;//TPAD����	
	if(mode&0X01)//��ʾ�ļ���
	{  
 		gui_phy.back_color=APP_WIN_BACK_COLOR;
		gui_fill_rectangle(cpymsg_x+10+9*6,cpymsg_y+32+15,126,12,APP_WIN_BACK_COLOR);		//����ɫ  0XE73C 
 		gui_show_string(pname,cpymsg_x+10+9*6,cpymsg_y+32+15,126,12,12,BLACK);				//��ʾ�µ��ļ���	
   		gui_show_string((u8*)sysset_system_update_cpymsg_tbl[1][gui_phy.language],cpymsg_x+10,cpymsg_y+32+15+21,66,12,12,BLACK);	
 	}
	if(mode&0X04)//��ʾ�ļ�����
	{
 		gui_phy.back_color=APP_WIN_BACK_COLOR;
		gui_fill_rectangle(cpymsg_x+10+11*6,cpymsg_y+32+15+21,114,12,APP_WIN_BACK_COLOR);	//����ɫ  0XE73C 
 		gui_show_string(pname,cpymsg_x+10+11*6,cpymsg_y+32+15+21,114,12,12,BLACK);			//��ʾ�µ��ļ�����	
	}
	if(mode&0X02)//���°ٷֱ�
	{
		if(pct>100)pct=100;
	    filled=pct;
		filled=(filled*9)/5;
		gui_fill_rectangle(cpymsg_x+10,cpymsg_y+32+15+21*2,filled,16,0X071F);				//���ռ���˵İٷֱȲ���     
		gui_fill_rectangle(cpymsg_x+10+filled,cpymsg_y+32+15+21*2,180-filled,16,WHITE);		//���δռ�õİٷֱȲ���  
		gui_show_num(cpymsg_x+10+72,cpymsg_y+32+15+21*2+2,3,BLACK,12,pct,1);				//��ʾ���½���
		gui_show_string("%",cpymsg_x+10+72+18,cpymsg_y+32+15+21*2+2,6,12,12,BLACK);			//��ʾ�ٷֺ�
 	}
//	delay_ms(1000/OS_TICKS_PER_SEC);//��ʱһ��ʱ�ӽ���
	return 0;	
}
//����ϵͳ�ļ���
//caption:����
void sysset_system_update(u8*caption,u16 sx,u16 sy)
{
  	_window_obj* twin=0;			//����
	u8 res;
	u8 *p;  
	res=app_system_file_check("0");
  	if(res)
	{
		p=(u8*)sysset_system_update_err_tbl[1][gui_phy.language];	//ϵͳ�ļ���ʧ
		window_msg_box(sx,sy+20,200,80,p,APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		delay_ms(1500);
//		system_task_return=0;//ȡ��TPAD
		return;
	}
	twin=window_creat(sx,sy,200,120,0,1,16);//��������
  	if(twin!=NULL)
	{
		//���ڵ����ֺͱ���ɫ
		twin->caption=caption;
		twin->windowbkc=APP_WIN_BACK_COLOR;	
 		window_draw(twin);						//��������
		gui_phy.back_color=APP_WIN_BACK_COLOR;
  		gui_show_string((u8*)sysset_system_update_cpymsg_tbl[0][gui_phy.language],sx+10,sy+32+15,54,12,12,BLACK);	//���ڸ���:
  		gui_show_string((u8*)sysset_system_update_cpymsg_tbl[1][gui_phy.language],sx+10,sy+32+15+21,66,12,12,BLACK);//��ǰ�ļ���:
 		gui_draw_rectangle(sx+10-1,sy+32+15+21*2-1,182,18,0X4A49);		//���߿�
		gui_fill_rectangle(sx+10,sy+32+15+21*2,180,16,WHITE);			//����ɫ  0XE73C 
		gui_show_num(sx+10+72,sy+32+15+21*2+2,3,BLACK,12,0,1);			//��ʾ���½���
		gui_show_string("%",sx+10+72+18,sy+32+15+21*2+2,6,12,12,BLACK);	//��ʾ�ٷֺ�
		cpymsg_x=sx;
		cpymsg_y=sy;
		res=app_system_update(sysset_system_update_cpymsg,"0:");		//��SD������
	   	p=(u8*)sysset_system_update_err_tbl[0][gui_phy.language];		//���³ɹ���ʾ
 		if(res==0XFF)p=(u8*)sysset_system_update_err_tbl[2][gui_phy.language];	//ǿ���˳�
		else if(res)p=(u8*)sysset_system_update_err_tbl[1][gui_phy.language];	//ϵͳ�ļ���ʧ
  		window_msg_box(sx,sy+20,200,80,p,APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		delay_ms(1500); 
	}
	window_delete(twin);
//	system_task_return=0;//ȡ��TPAD
}  
//��ʾϵͳ��Ϣ
//x,y:��ʼ����
//����:200,�߶�:272.
//caption:����
void sysset_system_info(u16 x,u16 y,u8*caption)
{
  	_window_obj* twin=0;			//����
 	_btn_obj * rbtn=0;				//���ذ�ť		  
	u8 rval=0;
	u8 *msgbuf;
	u8 numstrbuf[6];
 	u8 i;
	u32 dtsize,dfsize; 
	msgbuf=mymalloc(SRAMIN,31);		//����31���ֽ�
  	twin=window_creat(x,y,200,272,0,1|1<<5,16);//��������
 	rbtn=btn_creat(x+50,y+32+10+20*9+10,SYSSET_BTN3_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);//������ť
  	if(twin!=NULL&&rbtn!=NULL)
	{
		//���ڵ����ֺͱ���ɫ
		twin->caption=caption;
		twin->windowbkc=APP_WIN_BACK_COLOR;	
 		//���ذ�ť����ɫ    
		rbtn->bkctbl[0]=0X8452;//�߿���ɫ
		rbtn->bkctbl[1]=0XAD97;//��һ�е���ɫ				
		rbtn->bkctbl[2]=0XAD97;//�ϰ벿����ɫ
		rbtn->bkctbl[3]=0X8452;//�°벿����ɫ
 		rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//����Ϊ����

 		window_draw(twin);	//��������	
		btn_draw(rbtn);	  	//������
 		gui_draw_rectangle(x+10-4,y+32+14-4,188,20*9,0X4A49);		//���߿�
 		gui_phy.back_color=APP_WIN_BACK_COLOR;
 		rval=strlen((const char*)sysset_system_info_caption_tbl[0][gui_phy.language]);
		for(i=0;i<9;i++)
		{
	 		strcpy((char*)msgbuf,(const char*)sysset_system_info_caption_tbl[i][gui_phy.language]);
	 		if(i==2||i==3)//SD��/FLASH��  ����
			{
				dtsize=0;
				dfsize=0;
				if(i==2)
				{
					if(i==2)exf_getfree("0:",&dtsize,&dfsize);	//�õ�SD��ʣ��������������
					else exf_getfree("2:",&dtsize,&dfsize);		//�õ�SD��ʣ�������������� 
					dtsize>>=10;
					dfsize>>=10;
				}else exf_getfree("1:",&dtsize,&dfsize);//�õ�FLASH��ʣ��������������  
  				gui_num2str(numstrbuf,dfsize);
				strcat((char*)msgbuf,(const char*)numstrbuf);//���ӿ�������
				if(i==2)strcat((char*)msgbuf,(const char*)"MB/");
				else strcat((char*)msgbuf,(const char*)"KB/"); 
				gui_num2str(numstrbuf,dtsize);
				strcat((char*)msgbuf,(const char*)numstrbuf);//����������	  
			}
			strcat((char*)msgbuf,(const char*)sysset_system_info_msg_tbl[i]);
			if(i==4||i==5||i==6)
			{									   
				if(i==4)app_get_version(numstrbuf,OS_VERSION,3);	  	//OS�汾
				else if(i==5)app_get_version(numstrbuf,GUI_VERSION,3);	//GUI�汾
				else app_get_version(numstrbuf,HARDWARE_VERSION,2);		//Ӳ���汾
				strcat((char*)msgbuf,(const char*)numstrbuf);		 
 			}     
	   		gui_show_string(msgbuf,x+10,y+32+14+20*i,180,12,12,BLACK);//��ʾ��Ϣ
		} 
 		while(1)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
			delay_ms(1000/OS_OPT_TIME_PERIODIC);		//��ʱһ��ʱ�ӽ���
	 		if(system_task_return)break;			//TPAD����	
			rval=btn_check(rbtn,&in_obj);			//���ذ�ť���
			if(rval&&(rbtn->sta&0X80)==0)break;	 	//����Ч����
	 
		}
 	}
	myfree(SRAMIN,msgbuf);
	window_delete(twin);
	btn_delete(rbtn);
	system_task_return=0;//ȡ��TPAD
}	  

//��ʾϵͳ״̬
//x,y:��ʾ������
//����:200,�߶�:206.
//caption:����
void sysset_system_status(u16 x,u16 y,u8* caption)
{
 	u8 rval=0,res;
	u8 i=0;
	u16 temperate=0;
 	_window_obj* twin=0;			//����
 	_btn_obj * rbtn=0;				//���ذ�ť		  
	_progressbar_obj * cpuprgb=0;	//CPUʹ���ʽ�����
	_progressbar_obj * meminprgb=0;	//�ڲ��ڴ�ʹ�ý�����  
	_progressbar_obj * memexprgb=0;	//�ⲿ�ڴ�ʹ�ý�����

 	twin=window_creat(x,y,200,206,0,1|1<<5,16);					//��������
	cpuprgb=progressbar_creat(x+10,y+32+19,180,18,0X61);		//����������
	meminprgb=progressbar_creat(x+10,y+32+19+35,180,18,0X61);	//���������� 
	memexprgb=progressbar_creat(x+10,y+32+19+35*2,180,18,0X61);	//����������
	rbtn=btn_creat(x+50,y+32+19+35*2+36,SYSSET_BTN3_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);//������ť
 	if(twin==NULL||cpuprgb==NULL||meminprgb==NULL||memexprgb==NULL||rbtn==NULL)rval=1;
	else
	{
		//���ڵ����ֺͱ���ɫ
		twin->caption=caption;
		twin->windowbkc=APP_WIN_BACK_COLOR;	
		//���ذ�ť����ɫ    
		rbtn->bkctbl[0]=0X8452;//�߿���ɫ
		rbtn->bkctbl[1]=0XAD97;//��һ�е���ɫ				
		rbtn->bkctbl[2]=0XAD97;//�ϰ벿����ɫ
		rbtn->bkctbl[3]=0X8452;//�°벿����ɫ
		rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//����Ϊ����
		cpuprgb->totallen=100; 			//�����100
		meminprgb->totallen=100;    	//�����100
		memexprgb->totallen=100;		//�����100	 
 		window_draw(twin);				//��������
		btn_draw(rbtn);					//����ť
		gui_show_string((u8*)sysset_sysstatus_tbl[0][gui_phy.language],x+10,y+32+5,190,12,12,SYSSET_INWIN_FONT_COLOR);		//��ʾCPUʹ����
 		gui_show_string((u8*)sysset_sysstatus_tbl[1][gui_phy.language],x+10,y+32+5+35,190,12,12,SYSSET_INWIN_FONT_COLOR);	//��ʾ�ڲ��ڴ�ʹ����
		gui_show_string((u8*)sysset_sysstatus_tbl[2][gui_phy.language],x+10,y+32+5+35*2,190,12,12,SYSSET_INWIN_FONT_COLOR);	//��ʾ�ⲿ�ڴ�ʹ����
 		gui_show_string((u8*)sysset_sysstatus_tbl[3][gui_phy.language],x+200-10-36-36,y+32+5,190,12,12,SYSSET_INWIN_FONT_COLOR);//��ʾ�¶�
 		gui_show_string("��",x+200-10-12,y+32+5,190,12,12,SYSSET_INWIN_FONT_COLOR);	//��ʾCPU��ǰ�¶�
 	}
 	while(rval==0)
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
		delay_ms(1000/OS_OPT_TIME_PERIODIC);		//��ʱһ��ʱ�ӽ���
 		if(system_task_return)break;			//TPAD����	
		res=btn_check(rbtn,&in_obj);			//���ذ�ť���
		if(res)
		{
			if((rbtn->sta&0X80)==0)//����Ч����
			{
				break;//�˳�
			}
		}	
		if(i==0)//ʱ�䵽��,�����ڴ�,CPU�ȵ�״̬
		{
			cpuprgb->curpos=OSStatTaskCPUUsage;
			meminprgb->curpos=my_mem_perused(SRAMIN);	//�ڲ��ڴ�ʹ���� 
			memexprgb->curpos=my_mem_perused(SRAMEX);	//�ⲿ�ڴ�ʹ���� 
			progressbar_draw_progressbar(cpuprgb);		//��ʾ������
			progressbar_draw_progressbar(meminprgb);	//��ʾ������ 
			progressbar_draw_progressbar(memexprgb);	//��ʾ������
			//��ʾ�¶�ֵ
			//temperate=Get_Temprate()/10;//�õ��ڲ��¶�
			app_show_float(x+200-10-12,y+32+5,temperate,1,5,12,SYSSET_INWIN_FONT_COLOR,twin->windowbkc);//��ʾ�¶�
  			i=100;
		}
		i--;
	}
	window_delete(twin);			//ɾ������
	btn_delete(rbtn);				//ɾ����ť	
	progressbar_delete(cpuprgb);	//ɾ��������
	progressbar_delete(meminprgb);	//ɾ�������� 
	progressbar_delete(memexprgb);	//ɾ��������
	system_task_return=0;		 	//����˳���־
}
//����ϵͳ
//caption:����
void sysset_system_about(u16 x,u16 y,u8*caption)
{
  	_window_obj* twin=0;			//����
 	_btn_obj * rbtn=0;				//���ذ�ť		  
	u8 rval=0;
 	u32 sn0,sn1,sn2;
	u8 *tbuf;		  
	u8 verbuf[6];

 	tbuf=mymalloc(SRAMIN,31);					//����31���ֽ�
  	twin=window_creat(x,y,200,262,0,1|1<<5,16);	//��������
 	rbtn=btn_creat(x+50,y+32+180+10,SYSSET_BTN3_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);//������ť
  	if(twin!=NULL&&rbtn!=NULL)
	{
		//���ڵ����ֺͱ���ɫ
		twin->caption=caption;
		twin->windowbkc=APP_WIN_BACK_COLOR;	
 		//���ذ�ť����ɫ    
		rbtn->bkctbl[0]=0X8452;	//�߿���ɫ
		rbtn->bkctbl[1]=0XAD97;	//��һ�е���ɫ				
		rbtn->bkctbl[2]=0XAD97;	//�ϰ벿����ɫ
		rbtn->bkctbl[3]=0X8452;	//�°벿����ɫ
 		rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//����Ϊ����

 		window_draw(twin);		//��������	
		btn_draw(rbtn);	  		//������
 		gui_draw_rectangle(x+10-4,y+32+10+8,188,102,0X4A49);		//���߿�
 		gui_draw_rectangle(x+10-4,y+32+120+18,188,42,0X4A49);		//���߿�
 
  		gui_phy.back_color=APP_WIN_BACK_COLOR;
		gui_show_ptstr(x+10,y+32+10,x+10+180,y+32+10+16,0,BLACK,16,(u8*)sysset_system_about_caption_tbl[0][gui_phy.language],0);//ALIENTEK ս��
 	 	strcpy((char*)tbuf,"HARDWARE:");
		app_get_version(verbuf,HARDWARE_VERSION,2);
		strcat((char*)tbuf,(const char*)verbuf);
		strcat((char*)tbuf,", SOFTWARE:");						   
		app_get_version(verbuf,SOFTWARE_VERSION,3);
		strcat((char*)tbuf,(const char*)verbuf);
  		gui_show_string(tbuf,x+10,y+32+34,180,12,12,BLACK);//��ʾ��Ϣ
 		gui_show_string("Copyright (C) 2015~2025",x+10,y+32+34+20*1,180,12,12,BLACK);//��ʾ��Ϣ
 		gui_show_string("�����������ӿƼ����޹�˾",x+10,y+32+34+20*2,180,12,12,BLACK);//��ʾ��Ϣ
 		gui_show_string("www.openedv.com",x+10,y+32+34+20*3,180,12,12,BLACK);//��ʾ��Ϣ
 
  		gui_show_ptstr(x+10,y+32+120+10,x+10+180,y+32+120+10+16,0,BLACK,16,(u8*)sysset_system_about_caption_tbl[1][gui_phy.language],0);//��Ʒ���к�
		app_getstm32_sn(&sn0,&sn1,&sn2);//�õ����к�
		sprintf((char*)tbuf,"SN:%X%X%X",sn0,sn1,sn2);
		gui_show_string(tbuf,x+10,y+32+120+10+24,180,12,12,BLACK);//��ʾ��Ϣ
  		while(1)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
			delay_ms(1000/OS_OPT_TIME_PERIODIC);		//��ʱһ��ʱ�ӽ���
	 		if(system_task_return)break;			//TPAD����	
			rval=btn_check(rbtn,&in_obj);			//���ذ�ť���
			if(rval&&(rbtn->sta&0X80)==0)break;	 	//����Ч����
	 
		}
 	}
	myfree(SRAMIN,tbuf);
	window_delete(twin);
	btn_delete(rbtn);
	system_task_return=0;//ȡ��TPAD
} 

//������Ŀ����
//mcaption:��Ŀ¼�µ�Ŀ¼��(һ��Ҫ��'.'�ַ�������)
u8 * set_search_caption(const u8 *mcaption)
{
	while(*mcaption!='.')mcaption++;
	return (u8*)(++mcaption);
}
//ϵͳ�������ݱ�����:SYSTEM_PARA_SAVE_BASE
//��ȡϵͳ������Ϣ
//sysset:ϵͳ��Ϣ 
void sysset_read_para(_system_setings * sysset)
{
	AT24CXX_Read(SYSTEM_PARA_SAVE_BASE,(u8*)sysset,sizeof(_system_setings));
}
//д��ϵͳ������Ϣ
//sysset:ϵͳ��Ϣ 
void sysset_save_para(_system_setings * sysset)
{
  CPU_SR_ALLOC();
	OS_CRITICAL_ENTER();	//�����ٽ���
	AT24CXX_Write(SYSTEM_PARA_SAVE_BASE,(u8*)sysset,sizeof(_system_setings));
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
}

//ϵͳ���� 
u8 sysset_play(void)
{	
	u8 tlanguage;			//����ʱ���Ե�����					  	   
	u8 res;		 
	u8 selitem=0;
	u8 topitem=0;  	 
	u8 **items;	     					   	   
 	u8 *caption;			//�����б� 
	u8 *scaption;			//�ӱ���
	u16 temp1;
	u8 temp2,temp3;			//ʱ��/�����ݴ�
	u8 savemask=0;		    //0,����Ҫ�����κ�����;
							//[0]:systemset���ݸĶ����
							//[1]:vsset���ݸĶ����
							//[2]:alarm���ݸĶ����	 
//	_vs10xx_obj tvsset;
 
  caption=(u8*)APP_MFUNS_CAPTION_TBL[5][gui_phy.language];	 
	items=(u8**)sysset_mmenu_tbl[gui_phy.language];
	tlanguage=gui_phy.language;							//����ս��������Ե�����
	while(1)
	{   						   
   		res=app_listbox_select(&selitem,&topitem,caption,items,SYS_MENU_SIZE);
 		if(system_task_return){system_task_return=0;break;}					//TPAD����
		if(res&0X80)
		{	  
			scaption=set_search_caption(items[selitem]); 	 						   
			switch(selitem)//�ж�ѡ�����Ŀ
			{
				case 0://ʱ������ 			    
					RTC_Get();//���µ����µ�ʱ�� 
					temp1=calendar.hour;
					temp2=calendar.min;
					res=sysset_time_set((lcddev.width-150)/2,(lcddev.height-200)/2,(u8*)&temp1,(u8*)&temp2,scaption);
					if(res==0)//�ɹ�
					{
						RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,temp1,temp2,0);//����ʱ��
					} 
  					break;	  
				case 1://��������  	    
					RTC_Get();//���µ����µ�ʱ�� 
					temp1=calendar.w_year;
					temp2=calendar.w_month;	 
					temp3=calendar.w_date;
					res=sysset_date_set((lcddev.width-190)/2,(lcddev.height-200)/2,&temp1,(u8*)&temp2,(u8*)&temp3,scaption);
					if(res==0)//�ɹ�
					{
						RTC_Set(temp1,temp2,temp3,calendar.hour,calendar.min,calendar.sec);//����ʱ��
					} 																	  
					break;
				case 2://����ʱ������
					temp1=alarm.hour;
					temp2=alarm.min;
					res=sysset_time_set((lcddev.width-150)/2,(lcddev.height-200)/2,(u8*)&temp1,(u8*)&temp2,scaption);
					if(res==0)//�ɹ�,�����ȷ�ϰ�ť
					{
						alarm.hour=temp1;
						alarm.min=temp2;
						calendar_alarm_init(&alarm,&calendar);//���³�ʼ������
						savemask|=1<<2;//����������ݸĶ���
					} 
  					break;
				case 3://���ӿ�������
					temp1=alarm.weekmask;
			 		res=app_items_sel((lcddev.width-200)/2,(lcddev.height-302)/2,200,302,(u8**)calendar_week_table[gui_phy.language],7,(u8*)&temp1,0XB0,scaption);//��ѡ
 					if(res==0)//���óɹ�
					{
						alarm.weekmask=temp1;
						calendar_alarm_init(&alarm,&calendar);//���³�ʼ������	
						savemask|=1<<2;//����������ݸĶ���
					}   
					break;
				case 4://����������������
					temp1=alarm.ringsta&0X03;
			 		res=app_items_sel((lcddev.width-180)/2,(lcddev.height-232)/2,180,72+40*4,(u8**)sysset_ring_tbl[gui_phy.language],4,(u8*)&temp1,0X90,scaption);//��ѡ
					if(res==0)//���óɹ�
					{
						alarm.ringsta&=0XFC;//ȡ�������λԭ��������
						alarm.ringsta|=temp1;//������������						    
						savemask|=1<<2;//����������ݸĶ���
					}   
					break;
				case 5://��������
					temp1=gui_phy.language;//�õ�֮ǰ������
			 		res=app_items_sel((lcddev.width-180)/2,(lcddev.height-192)/2,180,72+40*3,(u8**)sysset_language_tbl,3,(u8*)&temp1,0X90,scaption);//��ѡ
					if(res==0)//���óɹ�
					{												   
						gui_phy.language=temp1;		//������������
						systemset.syslanguage=temp1;//��������Ĭ������
					    caption=(u8*)APP_MFUNS_CAPTION_TBL[5][gui_phy.language];	 
						items=(u8**)sysset_mmenu_tbl[gui_phy.language];			 
						savemask|=1<<0;//���ϵͳ�������ݸĶ���
					} 
					break; 
				case 6://ͼƬ���ģʽ����				 
					temp1=systemset.picmode;//�õ�֮ǰ������
			 		res=app_items_sel((lcddev.width-180)/2,(lcddev.height-152)/2,180,72+40*2,(u8**)sysset_avplaymode_tbl[gui_phy.language],2,(u8*)&temp1,0X90,scaption);//2��ģʽ
					if(res==0)//���óɹ�
					{												   
						systemset.picmode=temp1;//����ͼƬ���ģʽ			   		 
						savemask|=1<<0;//���ϵͳ�������ݸĶ���
					} 
					break; 				
				case 7://���ֲ���ģʽ����				 
					temp1=systemset.audiomode;//�õ�֮ǰ������
			 		res=app_items_sel((lcddev.width-180)/2,(lcddev.height-192)/2,180,72+40*3,(u8**)sysset_avplaymode_tbl[gui_phy.language],3,(u8*)&temp1,0X90,scaption);//3��ģʽ
					if(res==0)//���óɹ�
					{												   
						systemset.audiomode=temp1;//�������ֲ���ģʽ			   		 
						savemask|=1<<0;//���ϵͳ�������ݸĶ���
					} 
					break;			
				case 8://�������ȿ�������			 
//					temp1=vsset.speakersw;//�õ�֮ǰ������
			 		res=app_items_sel((lcddev.width-180)/2,(lcddev.height-152)/2,180,72+40*2,(u8**)sysset_speakermode_tbl[gui_phy.language],2,(u8*)&temp1,0X90,scaption);//2��ģʽ
					if(res==0)//���óɹ�
					{												   
//						vsset.speakersw=temp1;			//��������ģʽ	
//						VS_SPK_Set(vsset.speakersw);	//��������״̬  
						savemask|=1<<0;					//���ϵͳ�������ݸĶ���
					} 
					break;
				case 9://VS1053����&3D����   
//					tvsset=vsset;
//					res=sysset_vs1053_vol3d_set((lcddev.width-200)/2,(lcddev.height-212)/2,&tvsset,scaption);
 					if(res==0)//���óɹ�
					{			
//						vsset=tvsset;	//���浽vsset 		   		 
						savemask|=1<<1;	//���vsset���ݸĶ���
					}//else VS_Set_All();	//�ָ�֮ǰ������ 
					break; 
				case 10://EQ���� 
				case 11:
//					tvsset=vsset;
//					res=sysset_vs1053_eq_set((lcddev.width-200)/2,(lcddev.height-212)/2,&tvsset,selitem-10,scaption);
 					if(res==0)//���óɹ�
					{			
//						vsset=tvsset;	//���浽wsset 		   		 
						savemask|=1<<1;	//���vsset���ݸĶ���
					}//else VS_Set_All();	//�ָ�֮ǰ������ 
					break; 
				case 12://��������
 					temp1=systemset.lcdbklight;//�õ�֮ǰ�ı���
 			 		res=sysset_bklight_set((lcddev.width-180)/2,(lcddev.height-150)/2,scaption,(u16*)&systemset.lcdbklight);//��������
					if(res==0)//���óɹ�
					{												           		   		 
						savemask|=1<<0;//���ϵͳ�������ݸĶ���
					}else 	//ȡ��������
					{
						systemset.lcdbklight=temp1;//�ָ�ԭ��������
						if(systemset.lcdbklight)//�ָ�֮ǰ������.	
						{ 	
//							if(lcddev.id==0X1963)LCD_SSD_BackLightSet(systemset.lcdbklight);
//							else LCD_BLPWM_VAL=systemset.lcdbklight; 
						}else app_lcd_auto_bklight();//�Զ��������
					}
					break;
				case 13://��ĻУ׼  
					if((tp_dev.touchtype&0X80)==0)TP_Adjust(); //����������ҪУ׼
					system_task_return=0;	//����˳���־
					break; 
				case 14://ϵͳ����
					res=window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,"",(u8*)sysset_system_remaindmsg_tbl[0][gui_phy.language],12,0,0X03,0);
					if(res==1)//��Ҫ����
					{
				    	sysset_system_update((u8*)sysset_system_update_msg_tbl[gui_phy.language],(lcddev.width-200)/2,(lcddev.height-120)/2-10);
					}
					system_task_return=0;//ȡ��TPAD 
					break;
				case 15://�ָ�Ĭ������
					res=window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,"",(u8*)sysset_system_remaindmsg_tbl[1][gui_phy.language],12,0,0X03,0);
					if(res==1)//��Ҫ�ָ�Ĭ������
					{
						window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)sysset_system_remaindmsg_tbl[2][gui_phy.language],scaption,12,0,0,0);
				    app_set_default();
						delay_ms(1000);
					}
					break;
				case 16://ϵͳ��Ϣ	
					sysset_system_info((lcddev.width-200)/2,(lcddev.height-292)/2,scaption);
					break;
				case 17://ϵͳ״̬ ��ʾCPUʹ����/�ڴ�״̬��
					sysset_system_status((lcddev.width-200)/2,(lcddev.height-206)/2,scaption);
					break;
				case 18://����
					sysset_system_about((lcddev.width-200)/2,(lcddev.height-262)/2,scaption);
					break;	  				 				
			}
  		}else break;//�д�������	 			   
	}
	if(savemask&1<<0)//ϵͳ�������ݸĶ���
	{
		printf("sysset save!\r\n");
		sysset_save_para(&systemset);	//����ϵͳ������Ϣ 
	}
	if(savemask&1<<1)//WM8978���ݸĶ���
	{
		printf("VS1053 save!\r\n");
//		vs10xx_save_para(&vsset); 		//����VS1053������Ϣ 
	}
	if(savemask&1<<2)//�������ݸĶ���
	{
		printf("alarm save!\r\n");
//		calendar_save_para(&alarm);		//��������������Ϣ 
	}   
	if(tlanguage!=gui_phy.language)		//���Է����˸ı�
	{
		LCD_Clear(BLACK);
    window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)sysset_remindmsg_tbl[1][gui_phy.language],(u8*)sysset_remindmsg_tbl[0][gui_phy.language],12,0,0,0);
 		spb_delete();	//ɾ��֮ǰ��SPB����
		spb_init();		//����������.
	}
	return res;			   							  
}





















