#include "led.h"
#include "beep.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "ILI93xx.h"
#include "w25qxx.h"
#include "usart.h"	 
#include "24cxx.h"
#include "flash.h"
#include "touch.h"
#include "sram.h"
#include "timer.h"
#include "sdio_sdcard.h"
#include "malloc.h"
#include "ff.h"
#include "exfuns.h"
#include "w25qxx.h"
#include "includes.h"
#include "spblcd.h"
#include "spb.h"
#include "guix.h"
#include "piclib.h"
#include "ebook.h"
#include "rtc.h"
#include "calendar.h"
 #include "picviewer.h"
#include "piclib.h"
#include "settings.h"
#include "fontupd.h"
#include "usbplay.h"
#include "nesplay.h"
#include "calculator.h"
#include "notepad.h"
#include "exeplay.h"
#include "appplay.h"



#include "mass_mal.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "memory.h"	    
#include "usb_bot.h"  
/************************************************
 UCOSIII���������ȼ��û�������ʹ�ã�ALIENTEK
 ����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
 ���ȼ�0���жϷ������������� OS_IntQTask()
 ���ȼ�1��ʱ�ӽ������� OS_TickTask()
 ���ȼ�2����ʱ���� OS_TmrTask()
 ���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
 ���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()
************************************************/

#define START_TASK_PRIO				3
#define START_STK_SIZE 				128
OS_TCB StartTaskTCB;
CPU_STK START_TASK_STK[START_STK_SIZE];
void start_task(void *p_arg);

#define TOUCH_TASK_PRIO				4
#define TOUCH_STK_SIZE				128
OS_TCB TouchTaskTCB;
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
void touch_task(void *p_arg);

#define LED0_TASK_PRIO 				5
#define LED0_STK_SIZE				128
OS_TCB Led0TaskTCB;
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
void led0_task(void *p_arg);

#define EMWINDEMO_TASK_PRIO			6
#define EMWINDEMO_STK_SIZE			1024
OS_TCB EmwindemoTaskTCB;
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
void emwindemo_task(void *p_arg);


//�ⲿ�ڴ����(���֧��1M�ֽ��ڴ����)
//x,y:����
//fsize:�����С
//����ֵ:0,�ɹ�;1,ʧ��.
u8 system_exsram_test(u16 x,u16 y,u8 fsize)
{  
	u32 i=0;  	  
	u16 temp=0;	   
	u16 sval=0;	//�ڵ�ַ0����������	  				   
  LCD_ShowString(x,y,lcddev.width,y+fsize,fsize,"Ex Memory Test:   0KB"); 
	//ÿ��1K�ֽ�,д��һ������,�ܹ�д��1024������,�պ���1M�ֽ�
	for(i=0;i<512*1024;i+=1024)
	{
		FSMC_SRAM_WriteBuffer((u8*)&temp,i,2);
		//printf("FSMC_SRAM_WriteBuffer[%d] =%d\r\n",i/1024,temp);
		temp++;
	}
	//���ζ���֮ǰд�������,����У��		
	temp=0;  
 	for(i=0;i<512*1024;i+=1024) 
	{
  	FSMC_SRAM_ReadBuffer((u8*)&temp,i,2);
		//printf("FSMC_SRAM_ReadBuffer[%d] =%d\r\n",i/1024,temp);
//		if(i==0)sval=temp;
// 		else if(temp<=sval)break;//�������������һ��Ҫ�ȵ�һ�ζ��������ݴ�.	   		   
		LCD_ShowxNum(x+15*(fsize/2),y,(u16)(temp-sval+1),4,fsize,0);//��ʾ�ڴ�����  
 	}
	if(i>=512*1024)
	{
		LCD_ShowxNum(x+15*(fsize/2),y,i/1024,4,fsize,0);//��ʾ�ڴ�ֵ  		
		return 0;//�ڴ�����,�ɹ�
	}
	return 1;//ʧ��
}
//��ʾ������Ϣ
//x,y:����
//fsize:�����С
//x,y:����.err:������Ϣ
void system_error_show(u16 x,u16 y,u8*err,u8 fsize)
{
	POINT_COLOR=RED;
 	while(1)
	{
		LCD_ShowString(x,y,lcddev.width,lcddev.height,fsize,err);
		delay_ms(400);
		LCD_Fill(x,y,lcddev.width,y+fsize,BLACK);
		delay_ms(100);
		LED0=!LED0;
	} 
}
//��������SPI FLASH(��������Դ��ɾ��),�Կ��ٸ���ϵͳ.
//x,y:����
//fsize:�����С
//x,y:����.err:������Ϣ
//����ֵ:0,û�в���;1,������
u8 system_files_erase(u16 x,u16 y,u8 fsize)
{
	u8 key;
	u8 t=0;
	POINT_COLOR=RED;
	LCD_ShowString(x,y,lcddev.width,lcddev.height,fsize,"Erase all system files?");
	while(1)
	{
		t++;
		if(t==20)LCD_ShowString(x,y+fsize,lcddev.width,lcddev.height,fsize,"KEY0:NO / KEY2:YES");
		if(t==40)
		{
			gui_fill_rectangle(x,y+fsize,lcddev.width,fsize,BLACK);//�����ʾ
			t=0;
			LED0=!LED0;
		}
		key=KEY_Scan(0);
		if(key==KEY0_PRES)//������,�û�ȡ����
		{ 
			gui_fill_rectangle(x,y,lcddev.width,fsize*2,BLACK);//�����ʾ
			POINT_COLOR=WHITE;
			LED0=1;
			return 0;
		}
		if(key==KEY2_PRES)//Ҫ����,Ҫ��������
		{
			LED0=1;
			LCD_ShowString(x,y+fsize,lcddev.width,lcddev.height,fsize,"Erasing SPI FLASH...");
			W25QXX_Erase_Chip();
			LCD_ShowString(x,y+fsize,lcddev.width,lcddev.height,fsize,"Erasing SPI FLASH OK");
			delay_ms(600);
			return 1;
		}
		delay_ms(10);
	}
}
//�ֿ����ȷ����ʾ.
//x,y:����
//fsize:�����С 
//����ֵ:0,����Ҫ����;1,ȷ��Ҫ����
u8 system_font_update_confirm(u16 x,u16 y,u8 fsize)
{
	u8 key;
	u8 t=0;
	u8 res=0;
	POINT_COLOR=RED;
	LCD_ShowString(x,y,lcddev.width,lcddev.height,fsize,"Update font?");
	while(1)
	{
		t++;
		if(t==20)LCD_ShowString(x,y+fsize,lcddev.width,lcddev.height,fsize,"KEY0:NO / KEY2:YES");
		if(t==40)
		{
			gui_fill_rectangle(x,y+fsize,lcddev.width,fsize,BLACK);//�����ʾ
			t=0;
			LED0=!LED0;
		}
		key=KEY_Scan(0);
		if(key==KEY0_PRES)break;//������ 
		if(key==KEY2_PRES){res=1;break;}//Ҫ���� 
		delay_ms(10);
	}
	LED0=1;
	gui_fill_rectangle(x,y,lcddev.width,fsize*2,BLACK);//�����ʾ
	POINT_COLOR=WHITE;
	return res;
}
extern u8 Max_Lun;	//֧�ֵĴ��̸���,0��ʾ1��,1��ʾ2��.
//USB����SPI FLASHȷ��.
//x,y:����
//fsize:�����С 
//����ֵ:0,����Ҫ����;1,ȷ�ϸ���
u8 system_usb_update_confirm(u16 x,u16 y,u8 fsize)
{
	u8 key;
	u8 t=0;
	u8 res=0;
 	u8 offline_cnt=0;
	u8 Divece_STA=0;
 	u8 USB_STA=0;
	POINT_COLOR=RED;
	LCD_ShowString(x,y,lcddev.width,lcddev.height,fsize,"Use USB Update SPI FLASH Files?");
	while(1)
	{
		t++;
		if(t==20)LCD_ShowString(x,y+fsize,lcddev.width,lcddev.height,fsize,"KEY0:NO / KEY1:YES");
		if(t==40)
		{
			gui_fill_rectangle(x,y+fsize,lcddev.width,fsize,BLACK);//�����ʾ
			t=0;
			LED0=!LED0;
		}
		delay_ms(10);
		key=KEY_Scan(0);
		if(key==KEY0_PRES)break;//������ 
		if(key==KEY1_PRES){res=1;break;}//ȷ������
	}
	gui_fill_rectangle(x,y,lcddev.width,fsize*2,BLACK);//�����ʾ
	if(res)
	{
//		//�Ȳ���SPI FLASH
//		LCD_ShowString(x,y,lcddev.width,lcddev.height,fsize,"Erasing SPI FLASH...");
//		W25QXX_Erase_Chip();
//		//�ٸ�ʽ��SPI FLASH
//		LCD_ShowString(x,y,lcddev.width,lcddev.height,fsize,"Formating SPI FLASH...");
//		res=f_mkfs("1:",1,4096);//��ʽ��FLASH,1,�̷�;1,����Ҫ������,8������Ϊ1����
//		if(res==0)
//		{
//			f_setlabel((const TCHAR *)"1:ALIENTEK");				//����Flash���̵�����Ϊ��ALIENTEK
//		}
		LCD_ShowString(x,y,lcddev.width,lcddev.height,fsize,"Please push USB cable to USB_SLAVE");
		Data_Buffer=(u32*)gui_memin_malloc(BULK_MAX_PACKET_SIZE*2*4);	//ΪUSBͨѶ�����ڴ�
		Bulk_Data_Buff=gui_memin_malloc(BULK_MAX_PACKET_SIZE);			//ΪUSBͨѶ�����ڴ�
		//USB����
		USB_Port_Set(0); 	//USB�ȶϿ�
		delay_ms(700);  
		USB_Port_Set(1);	//USB�ٴ�����  
		Max_Lun=0;			//��һ������. 	  
		Mass_Memory_Size[0]=1024*1024*12;	//ǰ12M�ֽ�
		Mass_Block_Size[0] =512;			//����SPI FLASH�Ĳ���������СΪ512
		Mass_Block_Count[0]=Mass_Memory_Size[0]/Mass_Block_Size[0];  
		USB_Interrupts_Config();    
		Set_USBClock();   
		USB_Init();
		t=0;
		while(1)
		{
			t++;
			delay_ms(5);
			if(t==40)//200msʱ�䵽�� 
			{
				t=0;
				if(USB_STATUS_REG&0x10)
				{
					offline_cnt=0;//USB������,�����offline������
					bDeviceState=CONFIGURED;
				}else//û�еõ���ѯ 
				{
					offline_cnt++;  
					if(offline_cnt>10)bDeviceState=UNCONNECTED;//2s��û�յ����߱��,����USB���γ���
				}
				USB_STATUS_REG=0;
				LED0=!LED0;
			}			
			if(USB_STA!=USB_STATUS_REG)//״̬�ı��� 
			{	 						   
				gui_fill_rectangle(x,y+fsize*4,lcddev.width,fsize,BLACK);//�����ʾ 
				if(USB_STATUS_REG&0x01)//����д		  
				{
					LCD_ShowString(x,y+fsize*4,lcddev.width,lcddev.height,fsize,"USB Writing..."); 
					USB_STATUS_REG&=0xFD;//��������
				}
				USB_STA=USB_STATUS_REG;//��¼����״̬
			}
			if(Divece_STA!=bDeviceState)   
			{                                                                                        
				if(bDeviceState==CONFIGURED)
				{
					LCD_ShowString(x,y+fsize,lcddev.width,lcddev.height,fsize,"USB Connected   ");
					LCD_ShowString(x,y+fsize*2,lcddev.width,lcddev.height,fsize,"Copy SYSTEM folder to ALIENTEK Disk");
					LCD_ShowString(x,y+fsize*3,lcddev.width,lcddev.height,fsize,"When File Copy finished Press KEY0!");
				}else
				{
					LCD_ShowString(x,y+fsize,lcddev.width,lcddev.height,fsize,"USB DisConnected ");//��ʾUSB���γ���
					gui_fill_rectangle(x,y+fsize*2,lcddev.width,fsize*3,BLACK);//�����ʾ 
				}
				Divece_STA=bDeviceState;
			} 
			if(bDeviceState==CONFIGURED)//����״̬��,�ſ����˳�
			{
				key=KEY_Scan(0);
				if(key==KEY0_PRES){res=1;break;}
			}
		}
		USB_Port_Set(0); 				//USB�ȶϿ�
		gui_memin_free(Data_Buffer);	//�ͷ����뵽���ڴ�
		gui_memin_free(Bulk_Data_Buff);	//�ͷ����뵽���ڴ�
	} 
	LED0=1;
	gui_fill_rectangle(x,y,lcddev.width,fsize*2,BLACK);//�����ʾ
	POINT_COLOR=WHITE;
	return res;
}

void hw_check(void)
{
 	u16 okoffset=162;
 	u16 ypos=0;
	u16 j=0;
	u16 temp=0;
	u8 res;
	u32 dtsize,dfsize;
	u8 *stastr=0;
	u8 *version=0; 
	u8 verbuf[12];
	u8 fsize;
	u8 icowidth;

	uart_init(115200);			//��ʼ�����ڲ�����Ϊ115200
	//exeplay_app_check();		//����Ƿ���Ҫ����APP����.  
	delay_init();					//��ʱ��ʼ��  
 	LED_Init();							//��ʼ��LED 
 	TFTLCD_Init();							//LCD��ʼ��    
 	BEEP_Init();						//��������ʼ��
 	KEY_Init();							//������ʼ�� 
 	FSMC_SRAM_Init();				//��ʼ���ⲿSRAM
 	AT24CXX_Init();    			//EEPROM��ʼ��
	W25QXX_Init();					//��ʼ��W25Q128
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);		//��ʼ���ⲿSRAM�ڴ�� 
	tp_dev.init(); 					//��������ʼ��
	gui_init();	  					//gui��ʼ��
	piclib_init();					//piclib��ʼ��	  
	slcd_dma_init(); 
	exfuns_init();					//FATFS �����ڴ�
	
	version=mymalloc(SRAMIN,31);//����31���ֽ��ڴ�
REINIT:
	LCD_Clear(BLACK);			//����
	POINT_COLOR=WHITE;
	BACK_COLOR=BLACK;
	ypos=2;j=0;  
	if(lcddev.width==240)
	{
		fsize=12;
		icowidth=18;
		okoffset=190;
		app_show_mono_icos(5,ypos,icowidth,24,(u8*)APP_ALIENTEK_ICO1824,YELLOW,BLACK);
	}else if(lcddev.width==320)
	{
		fsize=16;
		icowidth=24;
		okoffset=250;
		app_show_mono_icos(5,ypos,icowidth,32,(u8*)APP_ALIENTEK_ICO2432,YELLOW,BLACK);		
	}else if(lcddev.width==480)
	{
		fsize=24;
		icowidth=36;
		okoffset=370;
		app_show_mono_icos(5,ypos,icowidth,48,(u8*)APP_ALIENTEK_ICO3648,YELLOW,BLACK);		
	}
	LCD_ShowString(icowidth+5*2,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "ALIENTEK STM32F1 WarShip V3");
	LCD_ShowString(icowidth+5*2,ypos+fsize*j++,lcddev.width,lcddev.height,fsize,"Copyright (C) 2015-2025");    
	app_get_version(verbuf,HARDWARE_VERSION,2);
	strcpy((char*)version,"HARDWARE:");
	strcat((char*)version,(const char*)verbuf);
	strcat((char*)version,", SOFTWARE:");
	app_get_version(verbuf,SOFTWARE_VERSION,3);
	strcat((char*)version,(const char*)verbuf);
	LCD_ShowString(5,ypos+fsize*j++,lcddev.width,lcddev.height,fsize,version);
	sprintf((char*)verbuf,"LCD ID:%04X",lcddev.id);		//LCD ID��ӡ��verbuf����
	LCD_ShowString(5,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, verbuf);	//��ʾLCD ID 
	//��ʼӲ������ʼ�� 
	LED0=0;LED1=0;				//ͬʱ��������LED
	LCD_ShowString(5,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "CPU:STM32F103ZET6 72Mhz");
	LCD_ShowString(5,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "FLASH:512KB   SRAM:64KB");	
	if(system_exsram_test(5,ypos+fsize*j,fsize))system_error_show(5,ypos+fsize*j++,"EX Memory Error!",fsize);
	LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize,"OK");			 
	my_mem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��,��������ڴ���֮��
	LED0=1;LED1=1;				//ͬʱ�ر�����LED 
	if(W25QXX_ReadID()!=W25Q128)//��ⲻ��W25Q128
	{	 
		system_error_show(5,ypos+fsize*j++,"Ex Flash Error!!",fsize); 
	}else temp=16*1024;			//16M�ֽڴ�С
	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "Ex Flash:     KB");			   
	LCD_ShowxNum(5+9*(fsize/2),ypos+fsize*j,temp,5,fsize,0);//��ʾflash��С  
	LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");   
	//����Ƿ���Ҫ����SPI FLASH?
	res=KEY_Scan(1);//
	if(res==KEY2_PRES)
	{
		res=system_files_erase(5,ypos+fsize*j,fsize);
		if(res)goto REINIT; 
	}
  //RTC���
  LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "RTC Check...");			   
 	if(RTC_Init())system_error_show(5,ypos+fsize*(j+1),"RTC Error!",fsize);//RTC���
	else 
	{ 
		RTC_Get();//��ȡʱ��
		LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");			   
	}
	//���SPI FLASH���ļ�ϵͳ
	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "FATFS Check...");//FATFS���			   
  res=	f_mount(fs[0],"0:",1); 		//����SD�� 
  //if(res) printf("����SD��ʧ��\r\n ");	
	//printf("��ʼ����FLASH\r\n ");	
  	f_mount(fs[1],"1:",1); 		//���ع���FLASH. 
 	LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");			   
	//SD�����
	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "SD Card:     MB");//FATFS���
	temp=0;	
 	do
	{
		temp++;
 		res=exf_getfree("0:",&dtsize,&dfsize);//�õ�SD��ʣ��������������
		delay_ms(200);		   
	}while(res&&temp<5);//�������5��
 	if(res==0)//�õ���������
	{ 
		gui_phy.memdevflag|=1<<0;	//����SD����λ.
		temp=dtsize>>10;//��λת��ΪMB
		stastr="OK";
 	}else 
	{
 		temp=0;//������,��λΪ0
		stastr="ERROR";
	}
 	LCD_ShowxNum(5+8*(fsize/2),ypos+fsize*j,temp,5,fsize,0);					//��ʾSD��������С
	LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize,stastr);	//SD��״̬			   
	//W25Q128���,����������ļ�ϵͳ,���ȴ���.
	temp=0;	
 	do
	{
		temp++;
 		res=exf_getfree("1:",&dtsize,&dfsize);//�õ�FLASHʣ��������������
		delay_ms(200);		   
	}while(res&&temp<20);//�������20��		  
	if(res==0X0D)//�ļ�ϵͳ������
	{
		LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "Flash Disk Formatting...");	//��ʽ��FLASH
		res=f_mkfs("1:",1,4096);//��ʽ��FLASH,1,�̷�;1,����Ҫ������,8������Ϊ1����
		if(res==0)
		{
			f_setlabel((const TCHAR *)"1:ALIENTEK");				//����Flash���̵�����Ϊ��ALIENTEK
			LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");//��־��ʽ���ɹ�
 			res=exf_getfree("1:",&dtsize,&dfsize);//���»�ȡ����
		}
	}   
	if(res==0)//�õ�FLASH��ʣ��������������
	{
		gui_phy.memdevflag|=1<<1;	//����SPI FLASH��λ.
		LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "Flash Disk:     KB");//FATFS���			   
		temp=dtsize; 	   
 	}else system_error_show(5,ypos+fsize*(j+1),"Flash Fat Error!",fsize);	//flash �ļ�ϵͳ���� 
 	LCD_ShowxNum(5+11*(fsize/2),ypos+fsize*j,temp,5,fsize,0);						//��ʾFLASH������С
	LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize,"OK");			//FLASH��״̬	
	//TPAD���		 
// 	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "TPAD Check...");			   
// 	if(TPAD_Init(6))system_error_show(5,ypos+fsize*(j+1),"TPAD Error!",fsize);//�����������
//	else LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK"); 
	//DM9000���   
//	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "DM9000 Check...");			   
// 	if(DM9000_Init(0))system_error_show(5,ypos+fsize*(j+1),"DM9000 Error!",fsize);//DM9000���
//	else LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK"); 
//	DM9000_RST=0;		//��λDM9000
//	EXTI->IMR&=~(1<<6);	//�ر�line6�ж�	 
	//24C02���
  LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "24C02 Check...");			   
 	if(AT24CXX_Check())system_error_show(5,ypos+fsize*(j+1),"24C02 Error!",fsize);//24C02���
	else LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");  
  //VS1053���
// 	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize,"VS1053 Check...");	 
//	VS_Init();		//��ʼ��VS1053�ӿ�
//	VS_Sine_Test();	//���Ҳ���
//	BEEP=0;		   
//	if(VS_Ram_Test()!=0X83FF)system_error_show(5,ypos+fsize*(j+1),"VS1053 Error!",fsize); 
//	else LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK"); 
//	VS_HD_Reset();
	//�ֿ���									    
   LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "Font Check...");
	res=KEY_Scan(1);//��ⰴ��
	if(res==KEY1_PRES)//���£�ȷ��
	{
		res=system_font_update_confirm(5,ypos+fsize*(j+1),fsize);
	}else res=0;
	if(font_init()||(res==1))//�������,������岻����/ǿ�Ƹ���,������ֿ�	
	{
		res=0;//������Ч
 		if(update_font(5,ypos+fsize*j,fsize,"0:")!=0)//��SD������
		{ 
			if(update_font(5,ypos+fsize*j,fsize,"1:")!=0)//��SPI FLASH����
			{ 
				res=system_usb_update_confirm(5,ypos+fsize*(j+1),fsize);	//�Ƿ��Ǵ�USB�����ļ���SPI FLASH?
				if(res)goto REINIT;
				system_error_show(5,ypos+fsize*(j+1),"Font Error!",fsize);	//�������
			} 
		}			
		LCD_Fill(5,ypos+fsize*j,lcddev.width,ypos+fsize*(j+1),BLACK);//����ɫ
    	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "Font Check...");			   
 	} 
	LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");//�ֿ���OK
	//ϵͳ�ļ����
  LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "SYSTEM Files Check...");			   
 	while(app_system_file_check("0"))//SPI FLASH ϵͳ�ļ����
	{
		delay_ms(100);
		LCD_Fill(5,ypos+fsize*j,lcddev.width,ypos+fsize*(j+1),BLACK);		//����ɫ
    LCD_ShowString(5,ypos+fsize*j,(fsize/2)*8,fsize,fsize, "Updating"); //��ʾupdating	
		app_boot_cpdmsg_set(5,ypos+fsize*j,fsize);		//���õ�����
		if(app_system_file_check("1"))					//���SD��ϵͳ�ļ�������
		{ 
			res=system_usb_update_confirm(5,ypos+fsize*(j+1),fsize);//SD�����������Ƿ��Ǵ�USB�����ļ���SPI FLASH?
			if(res)goto REINIT;
			system_error_show(5,ypos+fsize*(j+1),"SYSTEM File Error!",fsize); 	//����ʧ�ܣ�����  
		}
//	else 	if(app_system_update(app_boot_cpdmsg,"0:"))//SD������ 
//		{ 
//			system_error_show(5,ypos+fsize*(j+1),"SYSTEM File Error!",fsize);	//����ʧ�ܣ�����  
//		}
		LCD_Fill(5,ypos+fsize*j,lcddev.width,ypos+fsize*(j+1),BLACK);//����ɫ
    	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "SYSTEM Files Check..."); 
		if(app_system_file_check("1"))//������һ�Σ��ټ�⣬������в�ȫ��˵��SD���ļ��Ͳ�ȫ��
		{
			system_error_show(5,ypos+fsize*(j+1),"SYSTEM File Lost!",fsize);
		}else break;	
	}
	LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");	
 	//��������� 
	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "Touch Check...");			   
	res=KEY_Scan(1);//��ⰴ��			   
	if(TP_Init()||(res==KEY0_PRES&&(tp_dev.touchtype&0X80)==0))//�и���/������KEY0�Ҳ��ǵ�����,ִ��У׼ 	
	{
		if(res==1)TP_Adjust();
		res=0;//������Ч
		goto REINIT;				//���¿�ʼ��ʼ��
	}
	LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");//���������OK
   	//ϵͳ��������			   
 	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "SYSTEM Parameter Load...");			   
 	if(app_system_parameter_init())system_error_show(5,ypos+fsize*(j+1),"Parameter Load Error!",fsize);//��������
	else LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");			   
  	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "SYSTEM Starting...");  
	//�������̽�,��ʾ��������
	BEEP=1;
	delay_ms(100);
	BEEP=0; 
	myfree(SRAMIN,version);	
}

int main(void)
{	
	OS_ERR err;
	//FRESULT res;
	CPU_SR_ALLOC();
	hw_check();
//	delay_init();	    	//��ʱ������ʼ��	  
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
//	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
// 	LED_Init();			    //LED�˿ڳ�ʼ��
//	TFTLCD_Init();			//LCD��ʼ��	
//	KEY_Init();	 			//������ʼ��
//	RTC_Init();
//	W25QXX_Init();	
//	FSMC_SRAM_Init();		//��ʼ��SRAM
//	my_mem_init(SRAMIN); 	//��ʼ���ڲ��ڴ��
//	my_mem_init(SRAMEX);  	//��ʼ���ⲿ�ڴ��
//	slcd_dma_init();
//	gui_init();	  					//gui��ʼ��
//	piclib_init();					//piclib��ʼ��	
//	TP_Init();							//��������ʼ��	
//	LCD_Clear(WHITE);				//����
//	TP_Adjust();  					//��ĻУ׼  
//	TP_Get_Adjdata();	
	
	
//	while(SD_Init())
//	{
//		printf("SD_Init Error\r\n");
//		LED0=~LED0;LED1=~LED1;LED2=~LED2;LED3=~LED3;
//		delay_ms(800);
//	}

//	exfuns_init();			//Ϊfatfs�ļ�ϵͳ�����ڴ�
//	res=f_mount(fs[0],"0:",1);	//����SD��
//	if(res==NULL)printf("����SD�� Error\r\n");
//	res=f_mount(fs[1],"1:",1);	//����FLASH
//	if(res==NULL)printf("����FLASH Error\r\n");
	OSInit(&err);			//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();	//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
	while(1);
}

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		

	OS_CRITICAL_ENTER();	//�����ٽ���
	
	OSTaskCreate((OS_TCB*     )&EmwindemoTaskTCB,		
				 (CPU_CHAR*   )"Emwindemo task", 		
                 (OS_TASK_PTR )emwindemo_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )EMWINDEMO_TASK_PRIO,     
                 (CPU_STK*    )&EMWINDEMO_TASK_STK[0],	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE/10,	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	//����������
	OSTaskCreate((OS_TCB*     )&TouchTaskTCB,		
				 (CPU_CHAR*   )"Touch task", 		
                 (OS_TASK_PTR )touch_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )TOUCH_TASK_PRIO,     
                 (CPU_STK*    )&TOUCH_TASK_STK[0],	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE/10,	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);			 
	//LED0����
	OSTaskCreate((OS_TCB*     )&Led0TaskTCB,		
				 (CPU_CHAR*   )"Led0 task", 		
                 (OS_TASK_PTR )led0_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )LED0_TASK_PRIO,     
                 (CPU_STK*    )&LED0_TASK_STK[0],	
                 (CPU_STK_SIZE)LED0_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED0_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
}

//EMWINDEMO����
void emwindemo_task(void *p_arg)
{
	u8 selx;
	u16 tcnt=0;
	OS_ERR err; 
	spb_init();
	spb_load_mui();		//����SPB������
	slcd_frame_show(0);	//��ʾ����
	system_task_return=0;//���˳���־ 
	while(1)
	{
		selx=spb_move_chk(); 
		switch(selx)//������˫���¼�
		{   
			case 0:printf("ebook_play();\r\n");ebook_play();break;		//����ͼ�� 
			case 1:printf("picviewer_play();\r\n");picviewer_play();break;	//�������  
			case 2:printf("audio_play();\r\n");break;		//���ֲ��� 
 			case 3:printf("tomcat_play();\r\n");break;		//TOMè
			case 4:printf("calendar_play();\r\n");calendar_play();break;	//ʱ�� 
			case 5:printf("sysset_play();\r\n");sysset_play();break;		//ϵͳ����
			case 6:printf("nes_play();\r\n");nes_play();break;		//NES��Ϸ
 			case 7:printf("notepad_play();\r\n");notepad_play();break;	//���±�	
 			case 8:printf("exe_play();\r\n");exe_play();break;		//������
			case 9:printf("paint_play();\r\n");break;		//��д����
			case 10:printf("camera_play();\r\n");break;	//����ͷ
			case 11:printf("recorder_play();\r\n");break;	//¼����
			case 12:printf("usb_play();\r\n");usb_play();break;		//USB����
	    case 13:printf("net_play();\r\n");break;		//�������
			case 14:printf("wireless_play();\r\n");break;	//���ߴ���
 			case 15:printf("calc_play();\r\n");calc_play();break;		//������   
			case 16:printf("phone_play();\r\n");break;		//�绰����
 			case 17:printf("app_play();\r\n");app_play();break;		//APP 
 			case 18:printf("sms_play();\r\n");break;		//���Ź���
		}
		if(selx!=0XFF)spb_load_mui();//��ʾ������
		if(spbdev.spbheight==0&&spbdev.spbwidth==0)spb_load_mui();//��Ҫ���¼���
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//��ʱ5ms
		tcnt++;
		if(tcnt==500)	//500������Ϊ1����
		{
			tcnt=0;
			spb_stabar_msg_show(0);//����״̬����Ϣ
		}		
	}

}

//TOUCH����
void touch_task(void *p_arg)
{
	OS_ERR err;
//	u8 rerreturn=0; 
	while(1)
	{
		if(KEY_Scan(0)==WKUP_PRES)		//TPAD������һ��,�˺���ִ��,������Ҫ15ms.
		{
			system_task_return=1;
			if(gifdecoding)gifdecoding=0;	//���ٲ���gif
		}	 	
		//GUI_TOUCH_Exec();	
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//��ʱ5ms
	}
}

//LED0����
void led0_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//��ʱ500ms
	}
}
