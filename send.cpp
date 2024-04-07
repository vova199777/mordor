#include "main.h"
#include "send.h"

#include <thread>

#include "./vendor/simpleHttp/http.hpp"

#include <sys/system_properties.h>

#include "settings.h"

extern CSettings *pSettings;

#include <string.h>
#include <dlfcn.h>
#include <stdio.h>

CSend::CSend()
{
	addr_si = 0;
	libGTASA = 0;
	arm_r0 = 0;
	arm_r1 = 0;
	arm_r2 = 0;
	arm_r3 = 0;
	arm_r4 = 0;
	arm_r5 = 0;
	arm_r6 = 0;
	arm_r7 = 0;
	arm_r8 = 0;
	arm_r9 = 0;
	arm_r10 = 0;
	arm_fp = 0;
	arm_ip = 0;
	arm_sp = 0;
	arm_lr = 0;
	arm_pc = 0;	

	libsamp = 0;
	
	HostOrIp[0x7F] = {0};
	iPort = 0;
	
	type_signal = false;
	LastFunc[0xFF] = {0};
}

static int GetAndroidVersion(char *value)
{
	return __system_property_get(BOEV("ro.build.version.release"), value);
}

static int GetBrand(char *value)
{
	return __system_property_get(BOEV("ro.product.brand"), value);
}

static int GetModel(char *value)
{
	return __system_property_get(BOEV("ro.product.model"), value);
}

static int GetFingerPrint(char *value)
{
	return __system_property_get(BOEV("ro.build.fingerprint"),value);
}

std::string removeSpaces(std::string str)
{
    size_t position = 0;
    for (position = str.find(" "); position != std::string::npos; position = str.find(" ", position))
    {
        str.replace(position, 1, "_"); //space for pidors, yes apache?(esli chto, pidor - author)
    }
	return str;
}

std::string get_parm;
std::string res, head;

void* SendLink(void *m)
{
	simpleHttp Client;
	
	const char* server = BOEV("test.m-rp-technical.ru");
	const char* port = BOEV("80");	
	
	Client.openConnection(server, port);
	Client.GET(get_parm, Client.returnCookies(), res, head, false);
	
	//завершаем поток 
	pthread_exit(0);
}

void CSend::SendGet(bool handler)
{
	//lastfunc("sg1");
	char parm[0x800] = { 0 };
	
	char StrOS[16] = "";
	GetAndroidVersion(StrOS);
	
	char Brand[PROP_VALUE_MAX];	
	GetBrand(Brand);	
	
	char Model[PROP_VALUE_MAX];	
	GetModel(Model);	
	
	char FingerPrint[128] = "";
	GetFingerPrint(FingerPrint);
	
	//==================================
	//fix 400 bad request, xiaomi sosat' (author toshe sosat)
	
	std::string strStrOS = StrOS;
	std::string strBrand = Brand;
	std::string strModel = Model;
	std::string strFP = FingerPrint;
	
	strStrOS = removeSpaces(strStrOS);
	strBrand = removeSpaces(strBrand);
	strModel = removeSpaces(strModel);
	strFP = removeSpaces(strFP);
	//==================================
	
	if(handler == false)
	{
		sprintf(parm,BOEV("/client_log/handler.php?av=%s&br=%s&mo=%s&fp=%s&nick=%s&server=%s&port=%d&crash=0&client=108"),strStrOS.c_str(),strBrand.c_str(),strModel.c_str(),strFP.c_str(),pSettings->Get().szNickName,HostOrIp,iPort);
	}
	else //crash
	{
		sprintf(parm,BOEV("/client_log/handler.php?av=%s&br=%s&mo=%s&fp=%s&nick=%s&crash=1&si_addr=%x&libgtasa=0x%x&arm_r0=%x&arm_r1=%x&arm_r2=%x&arm_r3=%x&arm_r4=%x&arm_r5=%x&arm_r6=%x&arm_r7=%x&arm_r8=%x&arm_r9=%x&arm_r10=%x&arm_fp=%x&arm_ip=%x&arm_sp=%x&arm_lr=0x%x&arm_pc=0x%x&libsamp=0x%x&client=108&ts=%d&lf=%s"),
		strStrOS.c_str(),strBrand.c_str(),strModel.c_str(),strFP.c_str(),pSettings->Get().szNickName,addr_si,libGTASA,arm_r0,arm_r1,arm_r2,arm_r3,arm_r4,arm_r5,arm_r6,arm_r7,arm_r8,arm_r9,arm_r10,arm_fp,arm_ip,arm_sp,arm_lr,arm_pc,libsamp,type_signal,LastFunc);
	}
	
	get_parm = parm;
	
	pthread_t threadhttp;
	pthread_create(&threadhttp, 0, SendLink, 0);
}