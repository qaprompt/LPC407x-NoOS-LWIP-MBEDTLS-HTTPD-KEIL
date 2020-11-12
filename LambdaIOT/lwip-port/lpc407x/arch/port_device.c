/*
  port_device.c
  Author: Straight Coder<simpleisrobust@gmail.com>
  Date: June 20, 2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lwip/err.h>
#include <lwip/sys.h>

#include "port.h"

extern void LogPrint(int level, char* format, ...);
extern unsigned long GetLongIP(void);
extern unsigned long GetSubnet(void);
extern unsigned long GetGateway(void);
extern void LWIP_sprintf(char* buf, char* format, ...);

//////////////////////////////////////////////////////////////////////////////////////
// Constant global variables

static unsigned char g_byMyMAC[] = { '\x00', '\x00', '\x00', '\x20', '\xAA', '\x77' };

static char* g_szVendor = "Straight";
static char* g_szVendorURL = "https://github.com/straight-coding/";

static char* g_szModel = "straight-httpd";
static char* g_szModelURL = "https://github.com/straight-coding/straight-httpd";

static char* g_szDeviceName = "Virtual Device";
static char* g_szDeviceVersion = "v1.0.0";
static char* g_szDeviceSN = "2020-04-10";
static char* g_szDeviceUUID = "A5757C42-0234-4600-8E0F-8551B7EAEB7A";

char* GetVendor(void)			{	return g_szVendor;	}
char* GetVendorURL(void)		{	return g_szVendorURL; }
char* GetModel(void)			{	return g_szModel;	}
char* GetModelURL(void)			{	return g_szModelURL;	}
char* GetDeviceName(void)		{	return g_szDeviceName;	}
char* GetDeviceSN(void)			{	return g_szDeviceSN;	}
char* GetDeviceUUID(void)		{	return g_szDeviceUUID;	}
char* GetDeviceVersion(void)	{	return g_szDeviceVersion;	}
unsigned char* GetMyMAC(void)	{	return (unsigned char*)g_byMyMAC;	}

//////////////////////////////////////////////////////////////////////////////////////
// Editable global variables

//static sys_mutex_t g_devInfoMutex;

u32_t  g_nNetIsUp = 0xFFFFFFFF;
static DeviceConfig g_WorkConfig;
static DeviceConfig g_TempConfig;

void InitDevInfo(void)
{
	memset(&g_WorkConfig, 0, sizeof(g_WorkConfig));

	g_WorkConfig.nDhcpEnabled = 1;// 0 or 1;
	
	g_WorkConfig.dwIP = 0;		// 0xC0A80563;
	g_WorkConfig.dwSubnet = 0;	// 0xFFFFFF00;
	g_WorkConfig.dwGateway = 0;	// 0xC0A80501;

	g_WorkConfig.nLog = 0;
	g_WorkConfig.nSessionTimeout = 3 * 60 * 1000;

	g_TempConfig = g_WorkConfig;

	if (sys_mutex_new(&g_devInfoMutex) != ERR_OK) {}
}

static void lockDevInfo(void)
{
	sys_mutex_lock(&g_devInfoMutex);
}

static void unlockDevInfo(void)
{
	sys_mutex_unlock(&g_devInfoMutex);
}

//////////////////////////////////////////////////////////////////////////////////////
// Get/Set running configuration

unsigned long IsDhcpEnabled(void)
{
	return g_WorkConfig.nDhcpEnabled;
}

int FillDhcp(void* context, char* buffer, int maxSize)
{
	int len = 0;
	if (g_WorkConfig.nDhcpEnabled > 0)
		LWIP_sprintf(buffer, "%u", 1);
	else
		LWIP_sprintf(buffer, "%u", 0);

	len = strlen(buffer);
	return (len < maxSize) ? len : maxSize;
}

unsigned long GetMyIP(void)
{
	return GetLongIP();
	//return g_WorkConfig.dwIP;
}

u32_t GetLongIP(void)
{
	return g_WorkConfig.dwIP;	
}

unsigned long GetGateway(void)
{
	return g_WorkConfig.dwGateway;
}

unsigned long GetSubnet(void)
{
	return g_WorkConfig.dwSubnet;
}

void SetLongIP(u32_t addr)
{
	g_WorkConfig.dwIP = addr;
}

void SetGateway(u32_t addr)
{
	g_WorkConfig.dwGateway = addr;
}

void SetSubnet(u32_t addr)
{
	g_WorkConfig.dwSubnet = addr;
}

long GetSessionTimeout(void)
{
	return g_WorkConfig.nSessionTimeout;
}

int FillSessionTimeout(void* context, char* buffer, int maxSize)
{
	int len = 0;

	LWIP_sprintf(buffer, "%u", g_WorkConfig.nSessionTimeout / (60 * 1000));

	len = strlen(buffer);
	return (len < maxSize) ? len : maxSize;
}

int FillMAC(void* context, char* buffer, int maxSize)
{
	int len = 0;
	char szTemp[32];
	unsigned char* mac = GetMyMAC();

	LWIP_sprintf(szTemp, "%02X:%02X:%02X:%02X:%02X:%02X",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	strncpy(buffer, szTemp, maxSize);
	len = strlen(buffer);

	return (len < maxSize) ? len : maxSize;
}

int FillIP(void* context, char* buffer, int maxSize)
{
	int len = 0;
	char szTemp[32];
	long ip = GetLongIP();

	LWIP_sprintf(szTemp, "%d.%d.%d.%d",
		(int)((ip & 0xFF000000) >> 24),
		(int)((ip & 0x00FF0000) >> 16),
		(int)((ip & 0x0000FF00) >> 8),
		(int)((ip & 0x000000FF) >> 0));

	strncpy(buffer, szTemp, maxSize);
	len = strlen(buffer);

	return (len < maxSize) ? len : maxSize;
}

int FillGateway(void* context, char* buffer, int maxSize)
{
	int len = 0;
	char szTemp[32];
	long gw = GetGateway();

	LWIP_sprintf(szTemp, "%d.%d.%d.%d",
		(int)((gw & 0xFF000000) >> 24),
		(int)((gw & 0x00FF0000) >> 16),
		(int)((gw & 0x0000FF00) >> 8),
		(int)((gw & 0x000000FF) >> 0));

	strncpy(buffer, szTemp, maxSize);
	len = strlen(buffer);

	return (len < maxSize) ? len : maxSize;
}

int FillSubnet(void* context, char* buffer, int maxSize)
{
	int len = 0;
	char szTemp[32];
	long mask = GetSubnet();

	LWIP_sprintf(szTemp, "%d.%d.%d.%d",
		(int)((mask & 0xFF000000) >> 24),
		(int)((mask & 0x00FF0000) >> 16),
		(int)((mask & 0x0000FF00) >> 8),
		(int)((mask & 0x000000FF) >> 0));

	strncpy(buffer, szTemp, maxSize);
	len = strlen(buffer);

	return (len < maxSize) ? len : maxSize;
}

char* GetLocation(void)
{
	return g_WorkConfig.szLocation;
}

char* GetColor(void)
{
	return g_WorkConfig.szColor;
}

char* GetDate(void)
{
	return g_WorkConfig.szDate;
}

char* GetFont(void)
{
	return g_WorkConfig.szFont;
}

int FillLog(void* context, char* buffer, int maxSize)
{
	int len = 0;

	if (g_WorkConfig.nLog > 0)
		LWIP_sprintf(buffer, "%u", 1);
	else
		LWIP_sprintf(buffer, "%u", 0);

	len = strlen(buffer);
	return (len < maxSize) ? len : maxSize;
}

//////////////////////////////////////////////////////////////////////////////////////
// Edit/Apply configuration

void LoadConfig4Edit(void)
{
	lockDevInfo();
		g_TempConfig = g_WorkConfig;
	unlockDevInfo();

	LogPrint(0, "Load configurations\r\n");
}

extern long g_ipIsReady;
extern void LwipLinkUp(void);
extern void LwipLinkDown(void);

void AppyConfig(void)
{
	int dhcpChanged = 0;
	lockDevInfo();
		if (g_WorkConfig.nDhcpEnabled != g_TempConfig.nDhcpEnabled)
		{
			LogPrint(0, "DHCP changed from %d to %d\r\n", g_WorkConfig.nDhcpEnabled, g_TempConfig.nDhcpEnabled);
			dhcpChanged = 1;
		}
		g_WorkConfig = g_TempConfig;
	unlockDevInfo();

	if (dhcpChanged > 0)
	{ //need restart lwip stack
		LogPrint(0, "lwip stack will restart\r\n");
		g_ipIsReady = 0;
	}
}

void SetLocation(char* value)
{
	memset(g_TempConfig.szLocation, 0, sizeof(g_TempConfig.szLocation));
	strncpy(g_TempConfig.szLocation, value, sizeof(g_TempConfig.szLocation) - 1);
}

void SetColor(char* value)
{
	memset(g_TempConfig.szColor, 0, sizeof(g_TempConfig.szColor));
	strncpy(g_TempConfig.szColor, value, sizeof(g_TempConfig.szColor) - 1);
}

void SetTestDate(char* value)
{
	memset(g_TempConfig.szDate, 0, sizeof(g_TempConfig.szDate));
	strncpy(g_TempConfig.szDate, value, sizeof(g_TempConfig.szDate) - 1);
}

void SetFont(char* value)
{
	memset(g_TempConfig.szFont, 0, sizeof(g_TempConfig.szFont));
	strncpy(g_TempConfig.szFont, value, sizeof(g_TempConfig.szFont)-1);
}

void SetTestLog(char* value)
{
	g_TempConfig.nLog = atol(value);
}

void SetDhcpEnabled(char* value)
{
	g_TempConfig.nDhcpEnabled = (atol(value) != 0) ? 1 : 0;
}

void SetSessionTimeout(char* value)
{
	g_TempConfig.nSessionTimeout = 60*1000*atol(value);
}

void SetMyIPString(char* addr)
{
	g_TempConfig.dwIP = 0;//GetIpAddress(addr);
}

void SetGatewayString(char* addr)
{
	g_TempConfig.dwGateway = 0;//GetIpAddress(addr);
}

void SetSubnetString(char* addr)
{
	g_TempConfig.dwSubnet = 0;//GetIpAddress(addr);
}

void SetLogReady(int bReady)
{
}
