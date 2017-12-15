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
 UCOSIII中以下优先级用户程序不能使用，ALIENTEK
 将这些优先级分配给了UCOSIII的5个系统内部任务
 优先级0：中断服务服务管理任务 OS_IntQTask()
 优先级1：时钟节拍任务 OS_TickTask()
 优先级2：定时任务 OS_TmrTask()
 优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
 优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()
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


//外部内存测试(最大支持1M字节内存测试)
//x,y:坐标
//fsize:字体大小
//返回值:0,成功;1,失败.
u8 system_exsram_test(u16 x,u16 y,u8 fsize)
{  
	u32 i=0;  	  
	u16 temp=0;	   
	u16 sval=0;	//在地址0读到的数据	  				   
  LCD_ShowString(x,y,lcddev.width,y+fsize,fsize,"Ex Memory Test:   0KB"); 
	//每隔1K字节,写入一个数据,总共写入1024个数据,刚好是1M字节
	for(i=0;i<512*1024;i+=1024)
	{
		FSMC_SRAM_WriteBuffer((u8*)&temp,i,2);
		//printf("FSMC_SRAM_WriteBuffer[%d] =%d\r\n",i/1024,temp);
		temp++;
	}
	//依次读出之前写入的数据,进行校验		
	temp=0;  
 	for(i=0;i<512*1024;i+=1024) 
	{
  	FSMC_SRAM_ReadBuffer((u8*)&temp,i,2);
		//printf("FSMC_SRAM_ReadBuffer[%d] =%d\r\n",i/1024,temp);
//		if(i==0)sval=temp;
// 		else if(temp<=sval)break;//后面读出的数据一定要比第一次读到的数据大.	   		   
		LCD_ShowxNum(x+15*(fsize/2),y,(u16)(temp-sval+1),4,fsize,0);//显示内存容量  
 	}
	if(i>=512*1024)
	{
		LCD_ShowxNum(x+15*(fsize/2),y,i/1024,4,fsize,0);//显示内存值  		
		return 0;//内存正常,成功
	}
	return 1;//失败
}
//显示错误信息
//x,y:坐标
//fsize:字体大小
//x,y:坐标.err:错误信息
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
//擦除整个SPI FLASH(即所有资源都删除),以快速更新系统.
//x,y:坐标
//fsize:字体大小
//x,y:坐标.err:错误信息
//返回值:0,没有擦除;1,擦除了
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
			gui_fill_rectangle(x,y+fsize,lcddev.width,fsize,BLACK);//清除显示
			t=0;
			LED0=!LED0;
		}
		key=KEY_Scan(0);
		if(key==KEY0_PRES)//不擦除,用户取消了
		{ 
			gui_fill_rectangle(x,y,lcddev.width,fsize*2,BLACK);//清除显示
			POINT_COLOR=WHITE;
			LED0=1;
			return 0;
		}
		if(key==KEY2_PRES)//要擦除,要重新来过
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
//字库更新确认提示.
//x,y:坐标
//fsize:字体大小 
//返回值:0,不需要更新;1,确认要更新
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
			gui_fill_rectangle(x,y+fsize,lcddev.width,fsize,BLACK);//清除显示
			t=0;
			LED0=!LED0;
		}
		key=KEY_Scan(0);
		if(key==KEY0_PRES)break;//不更新 
		if(key==KEY2_PRES){res=1;break;}//要更新 
		delay_ms(10);
	}
	LED0=1;
	gui_fill_rectangle(x,y,lcddev.width,fsize*2,BLACK);//清除显示
	POINT_COLOR=WHITE;
	return res;
}
extern u8 Max_Lun;	//支持的磁盘个数,0表示1个,1表示2个.
//USB更新SPI FLASH确认.
//x,y:坐标
//fsize:字体大小 
//返回值:0,不需要更新;1,确认更新
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
			gui_fill_rectangle(x,y+fsize,lcddev.width,fsize,BLACK);//清除显示
			t=0;
			LED0=!LED0;
		}
		delay_ms(10);
		key=KEY_Scan(0);
		if(key==KEY0_PRES)break;//不更新 
		if(key==KEY1_PRES){res=1;break;}//确定更新
	}
	gui_fill_rectangle(x,y,lcddev.width,fsize*2,BLACK);//清除显示
	if(res)
	{
//		//先擦除SPI FLASH
//		LCD_ShowString(x,y,lcddev.width,lcddev.height,fsize,"Erasing SPI FLASH...");
//		W25QXX_Erase_Chip();
//		//再格式化SPI FLASH
//		LCD_ShowString(x,y,lcddev.width,lcddev.height,fsize,"Formating SPI FLASH...");
//		res=f_mkfs("1:",1,4096);//格式化FLASH,1,盘符;1,不需要引导区,8个扇区为1个簇
//		if(res==0)
//		{
//			f_setlabel((const TCHAR *)"1:ALIENTEK");				//设置Flash磁盘的名字为：ALIENTEK
//		}
		LCD_ShowString(x,y,lcddev.width,lcddev.height,fsize,"Please push USB cable to USB_SLAVE");
		Data_Buffer=(u32*)gui_memin_malloc(BULK_MAX_PACKET_SIZE*2*4);	//为USB通讯申请内存
		Bulk_Data_Buff=gui_memin_malloc(BULK_MAX_PACKET_SIZE);			//为USB通讯申请内存
		//USB配置
		USB_Port_Set(0); 	//USB先断开
		delay_ms(700);  
		USB_Port_Set(1);	//USB再次连接  
		Max_Lun=0;			//仅一个磁盘. 	  
		Mass_Memory_Size[0]=1024*1024*12;	//前12M字节
		Mass_Block_Size[0] =512;			//设置SPI FLASH的操作扇区大小为512
		Mass_Block_Count[0]=Mass_Memory_Size[0]/Mass_Block_Size[0];  
		USB_Interrupts_Config();    
		Set_USBClock();   
		USB_Init();
		t=0;
		while(1)
		{
			t++;
			delay_ms(5);
			if(t==40)//200ms时间到了 
			{
				t=0;
				if(USB_STATUS_REG&0x10)
				{
					offline_cnt=0;//USB连接了,则清除offline计数器
					bDeviceState=CONFIGURED;
				}else//没有得到轮询 
				{
					offline_cnt++;  
					if(offline_cnt>10)bDeviceState=UNCONNECTED;//2s内没收到在线标记,代表USB被拔出了
				}
				USB_STATUS_REG=0;
				LED0=!LED0;
			}			
			if(USB_STA!=USB_STATUS_REG)//状态改变了 
			{	 						   
				gui_fill_rectangle(x,y+fsize*4,lcddev.width,fsize,BLACK);//清除显示 
				if(USB_STATUS_REG&0x01)//正在写		  
				{
					LCD_ShowString(x,y+fsize*4,lcddev.width,lcddev.height,fsize,"USB Writing..."); 
					USB_STATUS_REG&=0xFD;//清除读标记
				}
				USB_STA=USB_STATUS_REG;//记录最后的状态
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
					LCD_ShowString(x,y+fsize,lcddev.width,lcddev.height,fsize,"USB DisConnected ");//提示USB被拔出了
					gui_fill_rectangle(x,y+fsize*2,lcddev.width,fsize*3,BLACK);//清除显示 
				}
				Divece_STA=bDeviceState;
			} 
			if(bDeviceState==CONFIGURED)//连接状态下,才可以退出
			{
				key=KEY_Scan(0);
				if(key==KEY0_PRES){res=1;break;}
			}
		}
		USB_Port_Set(0); 				//USB先断开
		gui_memin_free(Data_Buffer);	//释放申请到的内存
		gui_memin_free(Bulk_Data_Buff);	//释放申请到的内存
	} 
	LED0=1;
	gui_fill_rectangle(x,y,lcddev.width,fsize*2,BLACK);//清除显示
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

	uart_init(115200);			//初始化串口波特率为115200
	//exeplay_app_check();		//检测是否需要运行APP代码.  
	delay_init();					//延时初始化  
 	LED_Init();							//初始化LED 
 	TFTLCD_Init();							//LCD初始化    
 	BEEP_Init();						//蜂鸣器初始化
 	KEY_Init();							//按键初始化 
 	FSMC_SRAM_Init();				//初始化外部SRAM
 	AT24CXX_Init();    			//EEPROM初始化
	W25QXX_Init();					//初始化W25Q128
	my_mem_init(SRAMIN);		//初始化内部内存池
	my_mem_init(SRAMEX);		//初始化外部SRAM内存池 
	tp_dev.init(); 					//触摸屏初始化
	gui_init();	  					//gui初始化
	piclib_init();					//piclib初始化	  
	slcd_dma_init(); 
	exfuns_init();					//FATFS 申请内存
	
	version=mymalloc(SRAMIN,31);//申请31个字节内存
REINIT:
	LCD_Clear(BLACK);			//黑屏
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
	sprintf((char*)verbuf,"LCD ID:%04X",lcddev.id);		//LCD ID打印到verbuf里面
	LCD_ShowString(5,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, verbuf);	//显示LCD ID 
	//开始硬件检测初始化 
	LED0=0;LED1=0;				//同时点亮两个LED
	LCD_ShowString(5,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "CPU:STM32F103ZET6 72Mhz");
	LCD_ShowString(5,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "FLASH:512KB   SRAM:64KB");	
	if(system_exsram_test(5,ypos+fsize*j,fsize))system_error_show(5,ypos+fsize*j++,"EX Memory Error!",fsize);
	LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize,"OK");			 
	my_mem_init(SRAMEX);		//初始化外部内存池,必须放在内存检测之后
	LED0=1;LED1=1;				//同时关闭两个LED 
	if(W25QXX_ReadID()!=W25Q128)//检测不到W25Q128
	{	 
		system_error_show(5,ypos+fsize*j++,"Ex Flash Error!!",fsize); 
	}else temp=16*1024;			//16M字节大小
	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "Ex Flash:     KB");			   
	LCD_ShowxNum(5+9*(fsize/2),ypos+fsize*j,temp,5,fsize,0);//显示flash大小  
	LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");   
	//检测是否需要擦除SPI FLASH?
	res=KEY_Scan(1);//
	if(res==KEY2_PRES)
	{
		res=system_files_erase(5,ypos+fsize*j,fsize);
		if(res)goto REINIT; 
	}
  //RTC检测
  LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "RTC Check...");			   
 	if(RTC_Init())system_error_show(5,ypos+fsize*(j+1),"RTC Error!",fsize);//RTC检测
	else 
	{ 
		RTC_Get();//获取时间
		LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");			   
	}
	//检查SPI FLASH的文件系统
	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "FATFS Check...");//FATFS检测			   
  res=	f_mount(fs[0],"0:",1); 		//挂载SD卡 
  //if(res) printf("挂载SD卡失败\r\n ");	
	//printf("开始挂载FLASH\r\n ");	
  	f_mount(fs[1],"1:",1); 		//挂载挂载FLASH. 
 	LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");			   
	//SD卡检测
	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "SD Card:     MB");//FATFS检测
	temp=0;	
 	do
	{
		temp++;
 		res=exf_getfree("0:",&dtsize,&dfsize);//得到SD卡剩余容量和总容量
		delay_ms(200);		   
	}while(res&&temp<5);//连续检测5次
 	if(res==0)//得到容量正常
	{ 
		gui_phy.memdevflag|=1<<0;	//设置SD卡在位.
		temp=dtsize>>10;//单位转换为MB
		stastr="OK";
 	}else 
	{
 		temp=0;//出错了,单位为0
		stastr="ERROR";
	}
 	LCD_ShowxNum(5+8*(fsize/2),ypos+fsize*j,temp,5,fsize,0);					//显示SD卡容量大小
	LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize,stastr);	//SD卡状态			   
	//W25Q128检测,如果不存在文件系统,则先创建.
	temp=0;	
 	do
	{
		temp++;
 		res=exf_getfree("1:",&dtsize,&dfsize);//得到FLASH剩余容量和总容量
		delay_ms(200);		   
	}while(res&&temp<20);//连续检测20次		  
	if(res==0X0D)//文件系统不存在
	{
		LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "Flash Disk Formatting...");	//格式化FLASH
		res=f_mkfs("1:",1,4096);//格式化FLASH,1,盘符;1,不需要引导区,8个扇区为1个簇
		if(res==0)
		{
			f_setlabel((const TCHAR *)"1:ALIENTEK");				//设置Flash磁盘的名字为：ALIENTEK
			LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");//标志格式化成功
 			res=exf_getfree("1:",&dtsize,&dfsize);//重新获取容量
		}
	}   
	if(res==0)//得到FLASH卡剩余容量和总容量
	{
		gui_phy.memdevflag|=1<<1;	//设置SPI FLASH在位.
		LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "Flash Disk:     KB");//FATFS检测			   
		temp=dtsize; 	   
 	}else system_error_show(5,ypos+fsize*(j+1),"Flash Fat Error!",fsize);	//flash 文件系统错误 
 	LCD_ShowxNum(5+11*(fsize/2),ypos+fsize*j,temp,5,fsize,0);						//显示FLASH容量大小
	LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize,"OK");			//FLASH卡状态	
	//TPAD检测		 
// 	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "TPAD Check...");			   
// 	if(TPAD_Init(6))system_error_show(5,ypos+fsize*(j+1),"TPAD Error!",fsize);//触摸按键检测
//	else LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK"); 
	//DM9000检测   
//	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "DM9000 Check...");			   
// 	if(DM9000_Init(0))system_error_show(5,ypos+fsize*(j+1),"DM9000 Error!",fsize);//DM9000检测
//	else LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK"); 
//	DM9000_RST=0;		//复位DM9000
//	EXTI->IMR&=~(1<<6);	//关闭line6中断	 
	//24C02检测
  LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "24C02 Check...");			   
 	if(AT24CXX_Check())system_error_show(5,ypos+fsize*(j+1),"24C02 Error!",fsize);//24C02检测
	else LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");  
  //VS1053检测
// 	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize,"VS1053 Check...");	 
//	VS_Init();		//初始化VS1053接口
//	VS_Sine_Test();	//正弦测试
//	BEEP=0;		   
//	if(VS_Ram_Test()!=0X83FF)system_error_show(5,ypos+fsize*(j+1),"VS1053 Error!",fsize); 
//	else LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK"); 
//	VS_HD_Reset();
	//字库检测									    
   LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "Font Check...");
	res=KEY_Scan(1);//检测按键
	if(res==KEY1_PRES)//更新？确认
	{
		res=system_font_update_confirm(5,ypos+fsize*(j+1),fsize);
	}else res=0;
	if(font_init()||(res==1))//检测字体,如果字体不存在/强制更新,则更新字库	
	{
		res=0;//按键无效
 		if(update_font(5,ypos+fsize*j,fsize,"0:")!=0)//从SD卡更新
		{ 
			if(update_font(5,ypos+fsize*j,fsize,"1:")!=0)//从SPI FLASH更新
			{ 
				res=system_usb_update_confirm(5,ypos+fsize*(j+1),fsize);	//是否考虑从USB拷贝文件到SPI FLASH?
				if(res)goto REINIT;
				system_error_show(5,ypos+fsize*(j+1),"Font Error!",fsize);	//字体错误
			} 
		}			
		LCD_Fill(5,ypos+fsize*j,lcddev.width,ypos+fsize*(j+1),BLACK);//填充底色
    	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "Font Check...");			   
 	} 
	LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");//字库检测OK
	//系统文件检测
  LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "SYSTEM Files Check...");			   
 	while(app_system_file_check("0"))//SPI FLASH 系统文件检测
	{
		delay_ms(100);
		LCD_Fill(5,ypos+fsize*j,lcddev.width,ypos+fsize*(j+1),BLACK);		//填充底色
    LCD_ShowString(5,ypos+fsize*j,(fsize/2)*8,fsize,fsize, "Updating"); //显示updating	
		app_boot_cpdmsg_set(5,ypos+fsize*j,fsize);		//设置到坐标
		if(app_system_file_check("1"))					//检查SD卡系统文件完整性
		{ 
			res=system_usb_update_confirm(5,ypos+fsize*(j+1),fsize);//SD卡不完整，是否考虑从USB拷贝文件到SPI FLASH?
			if(res)goto REINIT;
			system_error_show(5,ypos+fsize*(j+1),"SYSTEM File Error!",fsize); 	//更新失败，报错  
		}
//	else 	if(app_system_update(app_boot_cpdmsg,"0:"))//SD卡更新 
//		{ 
//			system_error_show(5,ypos+fsize*(j+1),"SYSTEM File Error!",fsize);	//更新失败，报错  
//		}
		LCD_Fill(5,ypos+fsize*j,lcddev.width,ypos+fsize*(j+1),BLACK);//填充底色
    	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "SYSTEM Files Check..."); 
		if(app_system_file_check("1"))//更新了一次，再检测，如果还有不全，说明SD卡文件就不全！
		{
			system_error_show(5,ypos+fsize*(j+1),"SYSTEM File Lost!",fsize);
		}else break;	
	}
	LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");	
 	//触摸屏检测 
	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "Touch Check...");			   
	res=KEY_Scan(1);//检测按键			   
	if(TP_Init()||(res==KEY0_PRES&&(tp_dev.touchtype&0X80)==0))//有更新/按下了KEY0且不是电容屏,执行校准 	
	{
		if(res==1)TP_Adjust();
		res=0;//按键无效
		goto REINIT;				//重新开始初始化
	}
	LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");//触摸屏检测OK
   	//系统参数加载			   
 	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "SYSTEM Parameter Load...");			   
 	if(app_system_parameter_init())system_error_show(5,ypos+fsize*(j+1),"Parameter Load Error!",fsize);//参数加载
	else LCD_ShowString(5+okoffset,ypos+fsize*j++,lcddev.width,lcddev.height,fsize, "OK");			   
  	LCD_ShowString(5,ypos+fsize*j,lcddev.width,lcddev.height,fsize, "SYSTEM Starting...");  
	//蜂鸣器短叫,提示正常启动
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
//	delay_init();	    	//延时函数初始化	  
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
//	uart_init(115200);	 	//串口初始化为115200
// 	LED_Init();			    //LED端口初始化
//	TFTLCD_Init();			//LCD初始化	
//	KEY_Init();	 			//按键初始化
//	RTC_Init();
//	W25QXX_Init();	
//	FSMC_SRAM_Init();		//初始化SRAM
//	my_mem_init(SRAMIN); 	//初始化内部内存池
//	my_mem_init(SRAMEX);  	//初始化外部内存池
//	slcd_dma_init();
//	gui_init();	  					//gui初始化
//	piclib_init();					//piclib初始化	
//	TP_Init();							//触摸屏初始化	
//	LCD_Clear(WHITE);				//清屏
//	TP_Adjust();  					//屏幕校准  
//	TP_Get_Adjdata();	
	
	
//	while(SD_Init())
//	{
//		printf("SD_Init Error\r\n");
//		LED0=~LED0;LED1=~LED1;LED2=~LED2;LED3=~LED3;
//		delay_ms(800);
//	}

//	exfuns_init();			//为fatfs文件系统分配内存
//	res=f_mount(fs[0],"0:",1);	//挂载SD卡
//	if(res==NULL)printf("挂载SD卡 Error\r\n");
//	res=f_mount(fs[1],"1:",1);	//挂载FLASH
//	if(res==NULL)printf("挂载FLASH Error\r\n");
	OSInit(&err);			//初始化UCOSIII
	OS_CRITICAL_ENTER();	//进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);  //开启UCOSIII
	while(1);
}

//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		

	OS_CRITICAL_ENTER();	//进入临界区
	
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
	//触摸屏任务
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
	//LED0任务
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
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//退出临界区
}

//EMWINDEMO任务
void emwindemo_task(void *p_arg)
{
	u8 selx;
	u16 tcnt=0;
	OS_ERR err; 
	spb_init();
	spb_load_mui();		//加载SPB主界面
	slcd_frame_show(0);	//显示界面
	system_task_return=0;//清退出标志 
	while(1)
	{
		selx=spb_move_chk(); 
		switch(selx)//发生了双击事件
		{   
			case 0:printf("ebook_play();\r\n");ebook_play();break;		//电子图书 
			case 1:printf("picviewer_play();\r\n");picviewer_play();break;	//数码相框  
			case 2:printf("audio_play();\r\n");break;		//音乐播放 
 			case 3:printf("tomcat_play();\r\n");break;		//TOM猫
			case 4:printf("calendar_play();\r\n");calendar_play();break;	//时钟 
			case 5:printf("sysset_play();\r\n");sysset_play();break;		//系统设置
			case 6:printf("nes_play();\r\n");nes_play();break;		//NES游戏
 			case 7:printf("notepad_play();\r\n");notepad_play();break;	//记事本	
 			case 8:printf("exe_play();\r\n");exe_play();break;		//运行器
			case 9:printf("paint_play();\r\n");break;		//手写画笔
			case 10:printf("camera_play();\r\n");break;	//摄像头
			case 11:printf("recorder_play();\r\n");break;	//录音机
			case 12:printf("usb_play();\r\n");usb_play();break;		//USB连接
	    case 13:printf("net_play();\r\n");break;		//网络测试
			case 14:printf("wireless_play();\r\n");break;	//无线传书
 			case 15:printf("calc_play();\r\n");calc_play();break;		//计算器   
			case 16:printf("phone_play();\r\n");break;		//电话功能
 			case 17:printf("app_play();\r\n");app_play();break;		//APP 
 			case 18:printf("sms_play();\r\n");break;		//短信功能
		}
		if(selx!=0XFF)spb_load_mui();//显示主界面
		if(spbdev.spbheight==0&&spbdev.spbwidth==0)spb_load_mui();//需要重新加载
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//延时5ms
		tcnt++;
		if(tcnt==500)	//500个节拍为1秒钟
		{
			tcnt=0;
			spb_stabar_msg_show(0);//更新状态栏信息
		}		
	}

}

//TOUCH任务
void touch_task(void *p_arg)
{
	OS_ERR err;
//	u8 rerreturn=0; 
	while(1)
	{
		if(KEY_Scan(0)==WKUP_PRES)		//TPAD按下了一次,此函数执行,至少需要15ms.
		{
			system_task_return=1;
			if(gifdecoding)gifdecoding=0;	//不再播放gif
		}	 	
		//GUI_TOUCH_Exec();	
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//延时5ms
	}
}

//LED0任务
void led0_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//延时500ms
	}
}
