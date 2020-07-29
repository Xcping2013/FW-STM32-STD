#include "app_eeprom_24xx.h"

//#define EEPROM_DEBUG

#ifdef 	EEPROM_DEBUG
#define EEPROM_TRACE         printf
#else
#define EEPROM_TRACE(...)  				     
#endif

#define EEPROM_RW_PAGE_ADDR		55	
#define EEPROM_PAGE_LEN				200
#define EEPROM_PAGE_BUF				64
/**********************************************************************************************************************/

PARAM_T g_tParam;						

at24cxx_t at24c256=
{
	{0},
	{PB_4,PB_3},	//SCL SDA
	0xA0,
};

/**********************************************************************************************************************/
static uint8_t WritePageStringToEEPROM(uint8_t _pageNumber , char *_string);
static uint8_t ReadPageStringFromEEPROM(uint8_t _pageNumber, char *_string);

static void SaveParamToEeprom(void);
static void LoadParamFromEeprom(void);

static void LoadSettingViaProID(void);
static void printf_cmdList_eeprom(void);

/************************************************************************************************************************************************************/
static uint8_t WritePageStringToEEPROM(uint8_t _pageNumber , char *_string)
{
	uint8_t result = REPLY_OK;
	uint8_t _len=strlen(_string);

	if(_len<(EEPROM_PAGE_BUF+1)	&& 0< _pageNumber &&  _pageNumber < (EEPROM_PAGE_LEN+1)	)
	{
		if(_len!=EEPROM_PAGE_BUF)	{_string[_len]='\0';_len=_len+1;}
		at24cxx.write(at24c256,(EEPROM_RW_PAGE_ADDR+_pageNumber)*64,(uint8_t *)_string,_len);	
	}
	else 
	{
		result = REPLY_INVALID_VALUE;
	}
	return  result;
}

static uint8_t ReadPageStringFromEEPROM(uint8_t _pageNumber , char *_string)
{
	uint8_t result = REPLY_OK;
	if(0< _pageNumber &&  _pageNumber <( EEPROM_PAGE_LEN+1)	)
	{
		uint8_t i;
		at24cxx.read(at24c256 , (EEPROM_RW_PAGE_ADDR+_pageNumber)*64, _string, 64);
		for(i=0;i<EEPROM_PAGE_BUF;i++)	
		{
			if(_string[i]>127) { _string[i]='\0';break;}
		}
	}
	else 
	{
		result = REPLY_INVALID_VALUE;
	}
	return  result;
}
/*
*********************************************************************************************************
*	函 数 名: SaveParamToEeprom
*	功能说明: 将全局变量g_tParam 写入到EEPROM
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void SaveParamToEeprom(void)
{
	at24cxx.write(at24c256,PARAM_ADDR,(uint8_t *)&g_tParam,sizeof(PARAM_T));
}
/*
*********************************************************************************************************
*	函 数 名: LoadParamFromEeprom
*	功能说明: 从EEPROM读参数到g_tParam
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void LoadParamFromEeprom(void)
{
	at24cxx.read(at24c256 , PARAM_ADDR, (uint8_t *)&g_tParam, sizeof(PARAM_T));

	EEPROM_TRACE("read data from eeprom when power up,fw ver:%d\n",	g_tParam.ParamVer);
	EEPROM_TRACE("g_tParam.Baudrate=%d\n",													g_tParam.SlaveBaudrate);
	EEPROM_TRACE("g_tParam.Project_ID=%d\n",												g_tParam.Project_ID);
	EEPROM_TRACE("g_tParam.tmc429_VMax[0]=%d\n",										g_tParam.tmc429_VMax[0]);
	EEPROM_TRACE("g_tParam.speed_home[1]=%d\n",											g_tParam.speed_home[1]);
	EEPROM_TRACE("g_tParam.speed_home[2]=%d\n",											g_tParam.speed_home[2]);
	
	if (g_tParam.ParamVer != PARAM_VER)
	{
		g_tParam.ParamVer   = PARAM_VER;
		g_tParam.MasterBaudrate  = 115200;
		g_tParam.SlaveBaudrate   = 115200;
		g_tParam.Project_ID = COMMON;
	
#if defined(USING_TMC429_CLK_12MHZ) 
		
		g_tParam.tmc429_VMax[0] = 140;
		g_tParam.tmc429_AMax[0] = 1000;
		g_tParam.tmc429_PulseDiv[0]=5;
		g_tParam.tmc429_RampDiv[0]=8;
		g_tParam.speed_home[0]=839;

		g_tParam.tmc429_VMax[1] = 140;
		g_tParam.tmc429_AMax[1] = 1000;
		g_tParam.tmc429_PulseDiv[1]=5;
		g_tParam.tmc429_RampDiv[1]=8;
		g_tParam.speed_home[1]=839;


		g_tParam.tmc429_VMax[2] = 140;
		g_tParam.tmc429_AMax[2] = 1000;
		g_tParam.tmc429_PulseDiv[2]=5;
		g_tParam.tmc429_RampDiv[2]=8;
		g_tParam.speed_home[2]=839;
		
#endif

#if defined(USING_TMC429_CLK_16MHZ) 
		
		g_tParam.tmc429_VMax[0] = 839;
		g_tParam.tmc429_AMax[0] = 1000;
		g_tParam.tmc429_PulseDiv[0]=5;
		g_tParam.tmc429_RampDiv[0]=8;
		g_tParam.speed_home[0]=839;

		g_tParam.tmc429_VMax[1] = 839;
		g_tParam.tmc429_AMax[1] = 1000;
		g_tParam.tmc429_PulseDiv[1]=5;
		g_tParam.tmc429_RampDiv[1]=8;
		g_tParam.speed_home[1]=839;


		g_tParam.tmc429_VMax[2] = 839;
		g_tParam.tmc429_AMax[2] = 1000;
		g_tParam.tmc429_PulseDiv[2]=5;
		g_tParam.tmc429_RampDiv[2]=8;
		g_tParam.speed_home[2]=839;
		
#endif
		SaveParamToEeprom();						
	}
	switch(g_tParam.Project_ID)
	{
		case BUTTON_ONLINE: 
			printf(" --- type: BUTTON_ONLINE\n");		//button_online
			break;
		case BUTTON_OFFLINE: 
			printf(" --- type: BUTTON_OFFLINE\n");	//button_offline
			break;
		case OQC_FLEX: 																//OQC-Flex || BUTTON-AXIS-1
			printf(" --- type: OQC_FLEX\n");				
			break;
		case BUTTON_ROAD: 														//button_road	|| BUTTON-AXIS-2
			printf(" --- type: BUTTON_ROAD\n");
			break;
		case AOI_2AXIS: 
			printf(" --- type: AOI-2AXIS\n");
			break;
		case COMMON: 
			printf(" --- type: common user\n");
			break;
		default:
			printf("\ncontroller init ok --- type: common user\n");
		break;
	}
}


static void LoadSettingViaProID(void)
{
	
}
void printf_cmdList_eeprom(void)
{
	printf("EEPROM Usage: \r\n");
	printf("savedata: save data to page 1~200\r\n");
	printf("readdata: read data from page 1~200\r\n");
	printf("\r\nParamSave Usage: \r\n");
	printf("ParamSave ProjectType button_online:\r\n");	
	printf("ParamSave ProjectType button_offline:\r\n");		
	printf("ParamSave ProjectType button_road:\r\n");		
	printf("ParamSave ProjectType OQC-Flex:\r\n");
	printf("ParamSave ProjectType lidopen:\r\n");		
	printf("ParamSave ProjectType BUTTON-AXIS-1:\r\n");
	printf("ParamSave ProjectType BUTTON-AXIS-2:\r\n");
	printf("ParamSave ProjectType AOI-AXIS-2:\r\n");
	printf("ParamSave ProjectType common_use:\r\n");			
}
/*********************************************************************************************************************/
void at24cxx_hw_init(void)
{
	at24c256.eeprom=C24LC256;		//C24LC256;
	at24cxx.init(at24c256);
	if (at24cxx.check(at24c256) == 0)
	{
		printf("at24cxx_hw_init()...fail: no eeprom found!!!\n");
	}
	else 
	{
		printf("at24cxx_hw_init()...ok");
		LoadParamFromEeprom();
		
	}	
}
int savedata(int argc, char **argv)
{
	uint8_t result = REPLY_OK;

	if (argc ==3 )
	{
		int pageNum=atoi(argv[1]);
		
		if(!WritePageStringToEEPROM(pageNum,argv[2]))
		{
			printf("savedata[%d] ok\n",pageNum);
		}
		else 
		{
			printf("pageNum must be 1~200,data length must be 1~64\n");
		}
	}
	else
	{
			printf("Please input: savedata <page number> <data to save>\n");
			result = REPLY_INVALID_CMD;
	}	
	return result;
}
//
int readdata(int argc, char **argv)
{
	uint8_t result = REPLY_OK;

	if (argc ==2)
	{
		int pageNum=atoi(argv[1]);
		char read_info[64];
		if(!ReadPageStringFromEEPROM(pageNum,read_info))
		{
			printf("readdata[%d]=%.64s\n",pageNum,read_info);
		}
		else printf("pageNum must be 1~200\n");
	}
	else
	{
			printf("Please input: readdata <page number>\n");
			result = REPLY_INVALID_CMD;
	}	
	return result;
}
//
int ParamSave(int argc, char **argv)
{
	int result = REPLY_OK;
	
	if (argc == 3)
	{
		if 			(!strcmp(argv[1], "MasterBaudrate")) 
			g_tParam.MasterBaudrate  = atoi(argv[2]);		
		else if (!strcmp(argv[1], "SlaveBaudrate")) 
			g_tParam.SlaveBaudrate  = atoi(argv[2]);		
		else if (!strcmp(argv[1], "ProjectType"))
		{
			if 			(!strcmp(argv[2], "button_online")) 	g_tParam.Project_ID  = BUTTON_ONLINE;	
			else if (!strcmp(argv[2], "button_offline")) 	g_tParam.Project_ID  = BUTTON_OFFLINE;	
			else if (!strcmp(argv[2], "OQC-Flex")) 		    g_tParam.Project_ID  = OQC_FLEX;				//单轴
			else if (!strcmp(argv[2], "lidopen")) 				g_tParam.Project_ID  = LIDOPEN;	
			else if (!strcmp(argv[2], "button_road")) 		g_tParam.Project_ID  = BUTTON_ROAD;			//双轴
			
			else if (!strcmp(argv[2], "BUTTON-AXIS-1")) 	g_tParam.Project_ID  = OQC_FLEX;				//单轴
			else if (!strcmp(argv[2], "BUTTON-AXIS-2")) 	g_tParam.Project_ID  = BUTTON_ROAD;			//双轴
			else if (!strcmp(argv[2], "AOI-AXIS-2")) 		  g_tParam.Project_ID  = AOI_2AXIS;			//双轴
			
			else if (!strcmp(argv[2], "common_use")) 		  g_tParam.Project_ID  = COMMON;			//双轴
			
			else 	result = REPLY_INVALID_VALUE;
		}
		else result = REPLY_INVALID_CMD;
	}
	else
	{
		result = REPLY_INVALID_CMD;
	}
	if(result == REPLY_OK ) 
	{
		SaveParamToEeprom();	
		printf("save parameter ok and controller start to load setting......");	//reboot();	
		LoadSettingViaProID();
		printf("ok\n");
	}
	else 
	{
		printf_cmdList_eeprom();
	}
	return result;
}
//
uint8_t Command_analysis_eeprom(char *string)
{
	uint8_t result = REPLY_OK;
	if( !strcmp(string,"eeprom help") )		 printf_cmdList_eeprom();
	else if( !strncmp(string,"savedata ",9) ) 
	{
		char *p = NULL;
		char *s = &string[9];	
		uint32_t PageNum=0;	
		PageNum=strtol(s, &p, 10);	
		if( (*p==' ')	&& (!WritePageStringToEEPROM(PageNum,(char *)p+1)))
		{
			printf("savedata[%d] ok\r\n",PageNum);
		}		
		else 
		{
			//result = REPLY_INVALID_VALUE;
			printf("pageNum must be 1~200,data length must be 1~64\r\n");
		}
	}
	else if( !strncmp(string,"readdata ",9) ) 
	{
		char *p = NULL;
		char *s = &string[9];	
		uint32_t PageNum=0;	
		uint8_t read_info[64];
		PageNum=strtol(s, &p, 10);	
		if( (*p=='\0')	&& (!ReadPageStringFromEEPROM(PageNum,(char *)read_info)))
		{
			printf("readdata[%d]=%.64s\r\n",PageNum,read_info);
		}		
		else 
		{
			//result = REPLY_INVALID_VALUE;
			printf("pageNum must be 1~200\r\n");
		}
	}
	else if( !strcmp(string,"ParamSave ProjectType OQC-Flex") ) 			
	{
		g_tParam.Project_ID  = OQC_FLEX;	SaveParamToEeprom();	printf("save parameter ok and controller start to load setting......ok\r\n");
	}
	else if( !strcmp(string,"ParamSave ProjectType BUTTON-AXIS-1") ) 
	{
		g_tParam.Project_ID  = OQC_FLEX;	SaveParamToEeprom();	printf("save parameter ok and controller start to load setting......ok\r\n");
	}
	else if( !strcmp(string,"ParamSave ProjectType button_road") ) 
	{
		g_tParam.Project_ID  = BUTTON_ROAD;	SaveParamToEeprom();	printf("save parameter ok and controller start to load setting......ok\r\n");
	}
	else if( !strcmp(string,"ParamSave ProjectType BUTTON-AXIS-2") ) 
	{
		g_tParam.Project_ID  = BUTTON_ROAD;	SaveParamToEeprom();	printf("save parameter ok and controller start to load setting......ok\r\n");
	}
	else if( !strcmp(string,"ParamSave ProjectType common_use") ) 
	{
		g_tParam.Project_ID  = COMMON;	SaveParamToEeprom();	printf("save parameter ok and controller start to load setting......ok\r\n");
	}	
	else result = REPLY_INVALID_CMD;
	return result;
}

/*********************************************************************************************************************/



