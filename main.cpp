#include <jni.h>
#include <android/log.h>
#include <ucontext.h>
#include <pthread.h>

/*
	* Автор - Vadima Boeva(Dalbayob)
	* Судим за осквернение своих трудов
	* Опорочил своё лицо перед всей аудиторией
	* Манипулировал тем, что не пренадлежит ему
	* Создавал видимость работы
	* Травит игроков против того, кто не повелся на его манипуляции
	* Занимается всем неполезным в своей жизни
	* Обещал за 2 недели сделать дрЫст сервер - обосрался
	* Нарушил свою же притчу о читерах
	* Будет считать себя самым умным, однако на деле болен и глуп
	* Предаст того, кто поднял его с колен
	* Предаст того, кто дал ему возможность развиваться
	* Будет пытаться нанести ущерб невинным людям и отбирать их хлеб
	* Будет мечтать о своём CRMP проекте, но это лишь мечты
	* При любом удобном случае - насрёт там, где и кормился
*/

/*
	Դուք աննշան կտոր եք, ով չի գնահատում իր կյանքում որևէ բան,
	նույնիսկ իր սեփական մաշկը: Դու դեմքով ընկել ես ցեխի մեջ,
	քեզ աջակցել են, օգնել են,
	բայց դու էշի պես ես վարվել
*/

/*
	Сез үз тормышыгызда бернәрсә дә, хәтта үз тирегезне дә кадерләмәгән бернәрсә дә түгел. 
	сез йөзегезне пычракка салдыгыз, сезгә ярдәм иттегез, 
	ләкин сез ишәк кебек эш иттегез
*/

/*
	Сіз өзіңіздің өміріңізде ештеңе бағаламайсыз, тіпті өз теріңіз де. 
	сіз бетіңізді ластадыңыз, сіз көмектестіңіз, 
	бірақ сіз есек сияқты әрекет выіңіз
*/

/*
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%%#%%@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%#*+-:::-*%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%#####%@
	@@@@@@@@@@@@@@@@@#*+#@@@@@@@@@@@@@@@%**++==+*##@@@@@@%%%%@@@@@@@@@@@@@@@@@%%@@@@@@%####%%@
	@@@@@@@@@@@@@@@@@#++*@@@@@@@@@@%%%###%%%@@@@@@@@@@@@@@%*#@@@@@@@@@@@@@@@@@%%@@@@@@@%%##%%@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#++######%@@@@@@@@@@@@@@@@@@%%%@@@@@@@@@@@@@@@@@@@@@@@@@%%#%@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@%++*####%%@@@@@@@@@@@@@@@%%%%%%%%%@@@@@@@@@@@@@@@@@@@@%%%%%%#%@
	@@@@@@@@@@@@@@@@@@@@@@@%#*+*###%%@@@@@@@@@@@@@@@@@@%%%%##@%##%%%@@@@@@@@@@@@@@@@@%######%@
	@@@@@@@@@@@@@@@@@@@@%*=-:=**+*%@@@@@@@@@@@@@@@@@@%%%#####@@@%###%%@@@@@@@@@@@@@@@@%##%%%%%
	@@@@@@@@@@@@@@@@@@%*+=--====#@@@@@@@@@@@@@@@@@@@%%@%%%%@@@@@@%@%##%%@@@@@@@@@@@@@@@@%%%#%%
	@@@@@@@@@@@@@@@@@%*=*=-=-+#@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%##%%@%%###%#@@@@@@@@@@@@@%%%%%#%%
	@@@@@@@@@@@@@@@@%++*+=+*%@@@@@@@@@@@@@@%%#######%%%%%%#*+==+**###%%%%@@@@@@@@@@@@%%#####%%
	@@@@@@@@@@@@@@@%*#%#%%%@@@@@@@@@@@%%%%%%%###*********++++=====+*#%%%%%@@@@@@@@@@%%%%#%%%%%
	@@@@@@@@@@@@@@@#*@@@%@@@@@@@@@@@@@%%%%%%%###**++*++++++++++++++++#%@@@@@@@@@@@@@@@@@%%%%%%
	@@@@@@@@@@@@@@@%+%@@@@@@@@@@@@@@@@@%%%%%%####****++++++++++++++++*%@@@@@@@@@@@@@@@@@@%%%%%
	@@@@@@@@@@@@@@@@#*%@@@@@@@@@@@@@@@@%#%%####****+*+++*++*++++++++++%@@@@@@@@@@@@@@@@@@%%%%%
	@@@@@@@@@@@@@@@@#+*@@@@@@@@@@@@@@@@%##%####****+++++****+==+++++++%@@@@@@@@@@@@@@@@@@%%%%%
	@@@@@@@@@@@@@@@%++#@@@@@@@@@@@@@@@@%%%%%%%%##***+++=+**+++++++++++#@@@@@@@@@@@@@@@@@@@%%%%
	@@@@@@@@@@@@@@@%++%@@@@@@@@@@@@@@@@%%####%%%#%#***+=+**+**++++++++%@@@@@@@@@@@@@@@@@@@%%%@
	@@@@@@@@@@@@@@@%+%@@@@@@@@@@@@@@@@@%%%#%%%%%##*#*%*#*#*********+++@@@@@@@@@@@@@@@@@@%%%%%%
	@@@@@@@@@@@@@@@*+@@@@@@@@@@@@@@@@@@@@@@%@@@@@@@@%%%%*+%@@@@@%%###*@@@@@@@@@@@@@@@@@%%%%@@%
	@@@@@@@@@@@@@@%+*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%*=%@@@%%%%%#*+%@@@@@@@@@@@@@@@@%%%@@@@
	@@@@@@@@@@@@@@@%*@@@@@@@@@@@@@@@%###%%%%%#####%%@@%#+=+#####**++++#@@@@@@@@@@@@@@@@@@@@@@%
	@@@@@@@@@@@@@@%%#%@@@@@@@@@@@@@@%%%%##*#***###%%#@@%*==+++++++++++#%%%%%%%%@@@@@@@@@@@@@%%
	%@@@@@@@@@@@@@%%%%%#@@@@@@@@@@@@%##%###******#%%#%@@#+=+++++++++*+********#%@@@@@@@@@@@%%%
	%@@@@@@@@@@%%#%@@%%%#@@@@@@@@@@@@%%%%##*+*++*#%@%@@@#+=+**+++++++*********#%@@@@@@@@@@@%%#
	%@@@@@@@@@@%###%@@%%#%@@@@@@@@@@@@@%%%##*****#@%#@@%#*==+%#++++********++*#%@@@@@@@@@@@%%%
	%@@@@@@@@@@%%#%@@@@@@@%@@@@@@@@@@@@@@@%%%#***@@@@@@@#**++*#*******#**++++*#%@@@@@@@@@@@@%%
	@@@@@@@@@@@%#*#%@@@@@@@@@@@@@@@@@@@@@%%%%%#**%@@@@@@%#*+==+******%@%####%%%@@@@@@@@@@@@@@%
	@@@@@@@@@@#+=+*@@@@@@@@@@@@@@@@@@@@@@@%%%%#####%%%#*++++++++++**+@@@@@@@@@@@@@@@@@@@@@@@@%
	@@@@@@@@@*+==+#@@@@@@@@@@@@@@@@@@@@@@@@@@%#%%#####*+=++++++++**+#@@@@@@@@@@@@@@@@@@@@@@@@@
	%%@@@@@@*+==+*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%%%%#%%%%%##**+++**@@@@@@@@@@@@@@@@@@@@@@@@@@
	@%@@@@@#+==+*%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%@@@#*+******++++*%@@@@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@%#**#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%%%@@@%%%%%#**+++*%@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@%@@@@@@@@@@@@@@%%%%%%%%%%%###*++*%@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@*+@@@@@@@@@@@@@@%##%%%##*****#**++@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@@@%-#%@@@@@@@@@@@@@%#*###****+++***+:*@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@%#*-@@@@@@@@@@@@@@@@%####********##=.:%@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@%**@#=%@@@@@@@@@@@@@@@@@%%%###%####*+:..*@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@%#*#%@@#%@@@@@@@@@@@@@@@@@@@@%###****+-::.+@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@%###*#%%%@@@@@@@@@@@@@@@@@@@@@@%%##*****=-:::.+@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@%%%%@@@@%%@%%%%@@@@@@@@@@@@@@@@@@@@%%%#######*+--:::::#@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@@@%%###%@@@@@@@@@@%@@@@@@@@@@@@@@%%##*#%%##########*=-::::::=%@@@@@@%%%@@@@@@@@@@@@@@@@@@
	%###%%@@@@@@@@@@@@@@@@@@@@@@@@@@#***+++=+*#######%*=-::::-:::*@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#***++++==--+##%#+::::::--::=%@@@@@@@@@@%@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%##**+++=++*#####*++=--::::=#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
*/
/*
	::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	::::::::::::::::::::::::::::::::::::-=-----:::::-----:::::::::::::::::::::::::::::::::::::
	::::::::::::::::::::::::::::::-=++*############%%%#####**+=--:::::::::::::::::::::::::::::
	::::::::::::::::::::::::::-+#################%%%%%%%##%%%#**######**+-::::::::::::::::::::
	:::::::::::::::::::::--=*#%%%#################%@@%%%%###%%%#%%%%%%%%###*=-::::::::::::::::
	:::::::::::::::::-+####%%##%##%%##%############@@@%%%######%#%@@%%%%%%**##*-::::::::::::::
	:::::::::::::::=#%##**%%%%%###%%%%%%%%%#####%##@@%%%%%#****##%%%%%#*#%%#%###*-::::::::::::
	::::::::::::-*########%%%%%%####@@@%%%%#####%##%%%####+++*####%%%%%*+#%%%###%#*:::::::::::
	:::::::::::*##**#####%%@%%%##%#%%@@@%%%%#+############**######%#%@%%%###%%%%%%##-:::::::::
	:::::::::=######%%###%%@@%%###%#%%@@@@%%#*%####*+=+*+**#####%%%%%%%%%%#%%%@@%%%%#-::::::::
	::::::::+#####%%%%####%%@%%%%%##%%%%@@@%%%%#%%%##**##%%#%##%%%%#%%%%%%%%%%%@%%%%%#=:::::::
	::::::-*#####%@%%%%##%%%@@@%%%%%%%%%%%#%%%%%%%%%%%%%%%##%%%%%%%%%%%%%%%##%%%%%%%%##-::::::
	:::::-*######%@%%%%%###%%%@@%%%%%#%####%%%####%%######%%%####%%%%%%%%%###%%@@%%%%%#+::::::
	:::::*#######%@%%%%%%%#%%%%@@@%%%#%###############***##*++*%%%%%%%%%%###%%%@@%%%%###::::::
	::::+#########@@%%%%%%%%%%%%%@%%%%%%%%%%%####*#**++++##%%%%%%%%%%%%%%%##%%%%%%%%%%%#-:::::
	:::-*#%#####%%@@@@%%%%%##%%%%%%%%%%%%%%%%%%%%%%%##%%%%%%%%%%%%####%%%%%%##%%%%%%%%##-:::::
	:::=##%%%%###%%%@@@@%%%%%%%##%%%%%%##%%%%%%%%%%%%%%%%###%%%#**###%%%%%%%%%%%%%%%%##*::::::
	:::-##%%%%%%%%%%%@@@@@%%%%%###%############%%#####%##%%%#*+#%%%%%%%%%%%%%%%%%######-::::::
	:::-*#*%%%%%#%%%%%@@@@@@%%#%%#%%%###############%#######%%%%%%%%%%%%%%%%%%%%%####*-:::::::
	::::=###%%%%%%%%%%%%@@@@@@%@%%%%%%%%%%%%###%%%####%%%%%%%%#%%%%%%##%##%%#%%%####+:::::::::
	:::::+#%##%%%%%%%%%%%%%%%@%@@@%%%@%%%%#%%%%%%@%%%%%####%%%%%%%####****#######*+-::::::::::
	::::::=#%%##%%%%%%%%%%%%%%%%%#%##%@%%%%%%%%%%%%%%%%%%%%%%%#####*##**#######*+-::::::::::::
	::::::::=#%%###%%%%%%%%%%###%#%####%%%%%%%%%%%%%%%%%###*********#######**=-:::::::::::::::
	::::::::::-*#%##%%%%%%%%%#####%%%%%%%%%%%%%%%%%%##**+==+*##########**+-:::::::::::::::::::
	:::::::::::::=*######%%%%%#####%%%%%######****##*++**#########***=-:::::::::::::::::::::::
	:::::::::::::::-=+##################*#####**++**########**+==--::.::::::::..::::::::::::::
	::::::::::....:::::-=+###########################***++-::.....:.................::::::::::
	::::::::.........:::::::=+*################**++=-::..........................:::::::::::::
	:::::::::...............:::::-==++++++==---::..................................:::::::::::
	::::::::......................................................................::::::::::::
	::::::::.....................................................................:::::::::::::
*/

#include "main.h"
#include "game/game.h"
#include "game/RW/RenderWare.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "chatwindow.h"
#include "playertags.h"
#include "dialog.h"
#include "keyboard.h"
#include "settings.h"

#include "util/armhook.h"

#include "showmain.h"
#include "seatcar.h"
#include "chatbubble.h"

#include "game/snapshothelper.h"
#include "game/audiostream.h"
//#include "game/removebuilding.h"
#include "game/world.h"

#include "keyboard_history.h"

#include "send.h"

#include "game/world.h"

#include "util/CJavaWrapper.h"

#include <stdio.h>
#include <signal.h>
#include <stdio.h>
//#include <execinfo.h>

uintptr_t g_libGTASA = 0;
uintptr_t g_libSCAnd = 0;
void *g_libBASS = NULL;
const char* g_pszStorage = nullptr;
uintptr_t g_atomicmodels = 0;

int	sensx[18] = { 0 };
int sensy[18] = { 0 };

char materials[2048] = { 0 };

bool climb = true;

//const char* debugrpc = nullptr;

//int counter = 0; 
//const char* debuglog[] = {"","","","","",""};
//int counterhook = 0;
//const char* debughook[] = {"","","","","",""};

//======== nullptr =======================
CGame *pGame = nullptr;
CNetGame *pNetGame = nullptr;
CChatWindow *pChatWindow = nullptr;
CPlayerTags *pPlayerTags = nullptr;
CDialogWindow *pDialogWindow = nullptr;

CGUI *pGUI = nullptr;
CKeyBoard *pKeyBoard = nullptr;
CSettings *pSettings = nullptr;

CShowMain *pShowMain = nullptr;
CSeatCar *pSeatCar = nullptr;
CChatBubble *pBubble = nullptr;
CSnapShotHelper* pSnapShotHelper = nullptr;

CAudioStream *pAudioStream = nullptr;

//CRemoveBuilding *pRemoveBuilding = nullptr;
CWorld *pWorld = nullptr;

CKeyboardHistory *pKeyboardHistory = nullptr;

CSend *pSend = nullptr;

void InitHookStuff();
void InstallSpecialHooks();
void InitRenderWareFunctions();
void ApplyInGamePatches();
void ApplyPatches_level0();
void MainLoop();

extern int g_iLastRenderedObject;
extern int g_iLastRenderedVehicle;

std::string linux_error[32] = {
		"NULL", "SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP", "SIGABRT", "SIGBUS", "SIGFPE", "SIGKILL", "SIGUSR1",
		"SIGSEGV", "SIGUSR2", "SIGPIPE", "SIGALRM", "SIGTERM", "SIGSTKFLT", "SIGCHLD", "SIGCONT", "SIGSTOP", "SIGTSTP",
		"SIGTTIN", "SIGTTOU", "SIGURG", "SIGXCPU", "SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGIO", "SIGPWR", "SIGSYS"
};


extern "C"
{
	JNIEXPORT void JNICALL Java_com_nvidia_dev_1one_NvEventQueueActivity_initSAMP(JNIEnv* pEnv, jobject thiz)
	{
		g_pJavaWrapper = new CJavaWrapper(pEnv, thiz);
	}	
}

void InitInMenu()
{
	Log("InitInMenu");
	pGame = new CGame();
	pGame->InitInMenu();

	pGUI = new CGUI();
	pKeyBoard = new CKeyBoard();
	pKeyboardHistory = new CKeyboardHistory();
	pChatWindow = new CChatWindow();
	pPlayerTags = new CPlayerTags();
	pBubble = new CChatBubble();
	pDialogWindow = new CDialogWindow();
	
	pSnapShotHelper = new CSnapShotHelper();
	
	pAudioStream = new CAudioStream();
	//pRemoveBuilding = new CRemoveBuilding();
	
	pSend = new CSend();
	
	pWorld = new CWorld();
}

int get_server = 0;

#include "vendor/ini/config.h"
void InitInGame()
{
	static bool bGameInited = false;
	static bool bNetworkInited = false;

	if(!bGameInited)
	{
		pGame->InitInGame();
		pGame->SetMaxStats();
		
		pShowMain = new CShowMain();
		
		pSeatCar = new CSeatCar();

		bGameInited = true;
		return;
	}

	if(!bNetworkInited)
	{
		//pShowMain->Show(true);
		
		//load settings for Climb
		ini_table_s* config = ini_table_create();
		char pathtmp[0xFF] = { 0 };
		sprintf(pathtmp, BOEV("%s/SAMP/game.ini"), g_pszStorage);		
		if (ini_table_read_from_file(config, pathtmp)) 
		{
			climb = ini_table_get_entry_as_int(config, "game", "climb", 1);
		}
		ini_table_destroy(config);
		
		get_server = pSettings->Get().szserverid;
		
		pNetGame = new CNetGame( BOEV("s1.mordor-rp.ru"),s2d(BOEV("7777")),pSettings->Get().szNickName,"");
		
		pGame->DisplayHUD(false);
		pGame->DisplayWidgets(false);
		
		bNetworkInited = true;
		return;
	}
}

void MainLoop()
{
	InitInGame();
	
	//experimental fix
	if(pSettings->Get().iAndroidKeyboard == true)
	{
		if (g_pJavaWrapper)
		{
			//g_pJavaWrapper->HideInputLayout();
			if(!g_pJavaWrapper->IsKeyboardOpen())
			{
				if(pKeyboardHistory) pKeyboardHistory->Show(false);				
			}
		}
	}	

	if(pNetGame) pNetGame->Process();
}

std::string removeSpaces1(std::string str)
{
    size_t position = 0;
    for (position = str.find(" "); position != std::string::npos; position = str.find(" ", position))
    {
        str.replace(position, 1, "_"); //space for pidors, yes apache?
    }
	return str;
}

#include <unistd.h> // system api
#include <sys/mman.h>
#include <assert.h> // assert()
#include <dlfcn.h> // dlopen
void printAddressBacktrace(const unsigned address, void* pc, void* lr)
{
	char filename[0xFF];
	sprintf(filename, BOEV("/proc/%d/maps"), getpid());
	FILE* m_fp = fopen(filename, BOEV("rt"));
	if (m_fp == nullptr)
	{
		Log("ERROR: can't open file %s", filename);
		return;
	}
	Dl_info info_pc, info_lr;
	memset(&info_pc, 0, sizeof(Dl_info));
	memset(&info_lr, 0, sizeof(Dl_info));
	dladdr(pc, &info_pc);
	dladdr(lr, &info_lr);
 
	rewind(m_fp);
	char buffer[2048] = { 0 };
	while (fgets(buffer, sizeof(buffer), m_fp))
	{
		const auto start_address = strtoul(buffer, nullptr, 16);
		const auto end_address = strtoul(strchr(buffer, '-') + 1, nullptr, 16);
 
		if (start_address <= address && end_address > address)
		{
			if (*(strchr(buffer, ' ') + 3) == 'x')
				Log("Call: %X (GTA: %X PC: %s LR: %s) (SAMP: %X) (libc: %X)", address, address - g_libGTASA, info_pc.dli_sname, info_lr.dli_sname, address - FindLibrary("libsampONE.so"), address - FindLibrary("libc.so"));
			break;
		}
	}
}

#include <sys/system_properties.h>
int GetAndroidVers(char *value)
{
	return __system_property_get(BOEV("ro.build.version.release"), value);
}

int GetProductBrand(char *value)
{
	return __system_property_get(BOEV("ro.product.brand"), value);
}

int GetProductModel(char *value)
{
	return __system_property_get(BOEV("ro.product.model"), value);
}

void handler(int signum, siginfo_t *info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;
	
	const char* date_h = __DATE__;
	const char* time_h = __TIME__;
	
	char StrOS[16] = "";
	GetAndroidVers(StrOS);
	
	char Brand[PROP_VALUE_MAX];	
	GetProductBrand(Brand);	
	
	char Model[PROP_VALUE_MAX];	
	GetProductModel(Model);
	
	Log("==========Android Info========");
	Log(">> %s %s (version: %s) <<",Brand, Model, StrOS);
	//Log("==============================");
	Log("========CRASH EXCEPTED========");
	Log("SA:MP build date: " __DATE__ " " __TIME__);
	//Last rendered model: 98
	//Renderqueue offset: 38 | name: rqDisableCull
	//TrianglePlanes 0xA34AA6E0 12
	Log("==============================");
	Log("%s | SIGNAL %d | Fault address: 0x%X",linux_error[info->si_signo].c_str(), info->si_signo, info->si_addr);
	Log("Base addresses:");
	Log("libGTASA: 0x%X", g_libGTASA);
	Log("libSAMP: 0x%X", FindLibrary(BOEV("libsampONE.so")));
	Log("libc: 0x%X", FindLibrary(BOEV("libc.so")));
	Log("libm: 0x%X", FindLibrary(BOEV("libm.so")));
	Log("libEGL: 0x%X", FindLibrary(BOEV("libEGL.so")));
	Log("libGLESv2: 0x%X", FindLibrary(BOEV("libGLESv2.so")));
	Log("libImmEmulatorJ: 0x%X", FindLibrary(BOEV("libImmEmulatorJONE.so")));
	Log("libOpenSLES: 0x%X", FindLibrary(BOEV("libOpenSLES.so")));
	Log("libstdc++: 0x%X", FindLibrary(BOEV("libstdc++.so")));
	Log("=========LAST RENDERS=========");
	Log("Object ID: %d", g_iLastRenderedObject);
	Log("Vehicle ID: %d", g_iLastRenderedVehicle);
	
	if(pSettings->Get().objdebug)
	{
		Log("==========DEBUG MODE==========");
		Log("%s",materials);
	}
	
	Log("==============================");
	Log("Register states:");
	Log("r0: 0x%X, r1: 0x%X, r2: 0x%X, r3: 0x%X", context->uc_mcontext.arm_r0, context->uc_mcontext.arm_r1, context->uc_mcontext.arm_r2,context->uc_mcontext.arm_r3);
	Log("r4: 0x%x, r5: 0x%x, r6: 0x%x, r7: 0x%x",context->uc_mcontext.arm_r4,context->uc_mcontext.arm_r5,context->uc_mcontext.arm_r6,context->uc_mcontext.arm_r7);
	Log("r8: 0x%x, r9: 0x%x, sl: 0x%x, fp: 0x%x",context->uc_mcontext.arm_r8,context->uc_mcontext.arm_r9,context->uc_mcontext.arm_r10,context->uc_mcontext.arm_fp);
	Log("ip: 0x%x, sp: 0x%x, lr: 0x%x, pc: 0x%x",context->uc_mcontext.arm_ip,context->uc_mcontext.arm_sp,context->uc_mcontext.arm_lr,context->uc_mcontext.arm_pc);
	Log("==============================");
	Log("Backtrace:");
	Log("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA);
	Log("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA);
	Log("1: libSAMP.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary(BOEV("libsampONE.so")));
	Log("2: libSAMP.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary(BOEV("libsampONE.so")));
	Log("1: libc.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary(BOEV("libc.so")));
	Log("2: libc.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary(BOEV("libc.so")));
	Log("1: libm.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary(BOEV("libm.so")));
	Log("2: libm.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary(BOEV("libm.so")));
	Log("1: libEGL.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary(BOEV("libEGL.so")));
	Log("2: libEGL.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary(BOEV("libEGL.so")));
	Log("1: libGLESv2.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary(BOEV("libGLESv2.so")));
	Log("2: libGLESv2.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary(BOEV("libGLESv2.so")));
	Log("1: libImmEmulatorJ.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary(BOEV("libImmEmulatorJONE.so")));
	Log("2: libImmEmulatorJ.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary(BOEV("libImmEmulatorJONE.so")));
	Log("1: libOpenSLES.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary(BOEV("libOpenSLES.so")));
	Log("2: libOpenSLES.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary(BOEV("libOpenSLES.so")));
	Log("1: libstdc++.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary(BOEV("libstdc++.so")));
	Log("2: libstdc++.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary(BOEV("libstdc++.so")));	
	Log("==============================");
	//Log("Last function name:");
	//Log("PC: _ZN9CPhysical3AddEv
	//Log("==============================
	Log("=======Stacktrace start=======");
	for (auto i = 0; i < 100; ++i)
	{
		const auto address = *reinterpret_cast<uintptr_t*>(context->uc_mcontext.arm_sp + 4 * i);
 
		printAddressBacktrace(address, (void*)(context->uc_mcontext.arm_pc + 4 * i), (void*)(context->uc_mcontext.arm_lr + 4 * i));
	}

	Log("========Stacktrace end========");		

	exit(0);	
	
	return;
}

void *Init(void *p)
{
	ApplyPatches_level0();
	
	pthread_exit(0);
}

#include "CFPSFix.h"
static CFPSFix g_fpsFix;
void (*ANDRunThread)(void* a1);
void ANDRunThread_hook(void* a1)
{
	g_fpsFix.PushThread(gettid());

	ANDRunThread(a1);
}

uint32_t m_dwWaitAFK = GetTickCount();
void (*CTimer__StartUserPause)();
void CTimer__StartUserPause_hook()
{
	// process pause event
	if (g_pJavaWrapper)
	{
		g_pJavaWrapper->PlayerIsAfk(true);
		if (pKeyBoard)
		{
			if (pKeyBoard->IsNewKeyboard())
			{
				pKeyBoard->Close();
			}
		}
		if(pNetGame)
		{
			//...
			
	//if(pNetGame)
	//{
			if(GetTickCount() - m_dwWaitAFK > 1000 )
			{		
				RakNet::BitStream bsSend;
				bsSend.Write((uint8_t)179);
				bsSend.Write((uint8_t)1);
				pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
				
				m_dwWaitAFK = GetTickCount();
			}
	//}			
		}
	}

	*(uint8_t*)(g_libGTASA + 0x008C9BA3) = 1;
}

void (*CTimer__EndUserPause)();
void CTimer__EndUserPause_hook()
{
	// process resume event
	if (g_pJavaWrapper)
	{
		g_pJavaWrapper->PlayerIsAfk(false);
		if(pNetGame)
		{
			/*if(ShowAll)
			{
				if(g_pJavaWrapper->StateBonus)
				{
					g_pJavaWrapper->showMrpBonus(true);
				}
				g_pJavaWrapper->showMrpDate(true);
				g_pJavaWrapper->showMrpTime(true);
				g_pJavaWrapper->showMrpDateBackground(true);
				g_pJavaWrapper->showMrpTimeBackground(true);
				if(g_pJavaWrapper->StateLogoID)
				{
					g_pJavaWrapper->showMrpLogo(0);	
				}
				if(g_pJavaWrapper->StateLogoOnline)
				{
					g_pJavaWrapper->showMrpLogo(1);	
				}			
			}
			else
			{
				g_pJavaWrapper->showMrpBonus(false);
				g_pJavaWrapper->showMrpDate(false);
				g_pJavaWrapper->showMrpTime(false);
				g_pJavaWrapper->showMrpDateBackground(false);
				g_pJavaWrapper->showMrpTimeBackground(false);
				g_pJavaWrapper->hideMrpLogo();
			}*/
		}
	}

	*(uint8_t*)(g_libGTASA + 0x008C9BA3) = 0;
}

extern "C"
{
	JavaVM* javaVM = NULL;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	javaVM = vm;

	g_libGTASA = FindLibrary(BOEV("libGTASAONE.so"));
	if(g_libGTASA == 0)
	{
		Log("ERROR: libGTASA.so address not found!");
		return 0;
	}

	Log("libGTASA.so image base address: 0x%X", g_libGTASA);
	
	//init array sens in memory
	for(int i = 0; i < 18; i++)
	{
		sensx[i] = 0;
		sensy[i] = 0;
	}
	
	// init 
	for(int i=0;i<2048;i++)
	{
		materials[i] = '0';
	}
	materials[2048] = '\0';
	
	climb = true;
	
	srand(time(0));

	InitHookStuff();
	InitRenderWareFunctions();
	InstallSpecialHooks();
	
	Hook("0x23768C", (uintptr_t)ANDRunThread_hook, (uintptr_t*)&ANDRunThread);
	
	// change 0x80000 to 0x1000000
	WriteMemory("0x1A7EF2", (uintptr_t)"\x4F\xF4\x40\x10\x4F\xF4\x40\x10", 8);
	WriteMemory("0x1A7F34", (uintptr_t)"\x4F\xF4\x40\x10\x4F\xF4\x40\x10", 8);
	
    NOP("0x2E1EDC", 2); //CUpsideDownCarCheck
    NOP("0x398972", 2); //CRoadBlocks::GenerateRoadBlocks
	
	Hook("0x3BF784", (uintptr_t)CTimer__StartUserPause_hook, (uintptr_t*)& CTimer__StartUserPause);
	Hook("0x3BF7A0", (uintptr_t)CTimer__EndUserPause_hook, (uintptr_t*)& CTimer__EndUserPause);	
	
	pthread_t thread;
	pthread_create(&thread, 0, Init, 0);
	//ApplyPatches_level0();

	struct sigaction act;
	act.sa_sigaction = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &act, 0);
	sigaction(SIGBUS, &act, 0);
	
	//from GTA
	sigaction(SIGILL, &act, 0);
	sigaction(SIGABRT, &act, 0);
	sigaction(SIGFPE, &act, 0);
	sigaction(SIGSTKFLT, &act, 0);
	sigaction(SIGPIPE, &act, 0);
	//linux
	sigaction(SIGHUP, &act, 0);
	sigaction(SIGINT, &act, 0);
	sigaction(SIGQUIT, &act, 0);
	sigaction(SIGTRAP, &act, 0);
	sigaction(SIGKILL, &act, 0);
	sigaction(SIGUSR1, &act, 0);
	sigaction(SIGUSR2, &act, 0);
	sigaction(SIGALRM, &act, 0);
	sigaction(SIGTERM, &act, 0);
	sigaction(SIGCHLD, &act, 0);
	sigaction(SIGCONT, &act, 0);
	sigaction(SIGSTOP, &act, 0);
	sigaction(SIGTSTP, &act, 0);
	sigaction(SIGTTIN, &act, 0);
	sigaction(SIGTTOU, &act, 0);
	sigaction(SIGURG, &act, 0);
	sigaction(SIGXCPU, &act, 0);
	sigaction(SIGXFSZ, &act, 0);
	sigaction(SIGVTALRM, &act, 0);
	sigaction(SIGPROF, &act, 0);
	sigaction(SIGWINCH, &act, 0);
	sigaction(SIGIO, &act, 0);
	sigaction(SIGPWR, &act, 0);
	sigaction(SIGSYS, &act, 0);

	return JNI_VERSION_1_4;
}

#undef Log(a)
void Log(const char *fmt, ...)
{	
	char buffer[0xFF];
	static FILE* flLog = nullptr;

	if(flLog == nullptr && g_pszStorage != nullptr)
	{
		sprintf(buffer, BOEV("%sSAMP/samp_log.txt"), g_pszStorage);
		flLog = fopen(buffer, BOEV("w"));
	}

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);
	
	//__android_log_write(ANDROID_LOG_INFO, "BOEV", buffer);
	
	char buf[80];
	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	//const char* format = BOEV("%d.%m.%y %H:%M:%S");
	const char* format = BOEV("%H:%M:%S");
	strftime(buf, 80, format, timeinfo);

	if(flLog == nullptr) return;
	fprintf(flLog, BOEV("[%s] %s\n"), buf,buffer);
	fflush(flLog);

	return;
}

#undef lastfunc(a)
void lastfunc(const char *fmt, ...)
{	
	//memset(buffer_lastfunc, 0, sizeof(buffer_lastfunc));

	//va_list arg;
	//va_start(arg, fmt);
	//vsnprintf(buffer_lastfunc, sizeof(buffer_lastfunc), fmt, arg);
	//va_end(arg);

	return;
}

// nice debug !

//char tmp_debug_log[0xFF];

#undef DebugLog(a)
void DebugLog(const char *fmt)
{
	/*memset(tmp_debug_log, 0, sizeof(tmp_debug_log));
	va_list arg;
	va_start(arg, fmt);
	vsnprintf(tmp_debug_log, sizeof(tmp_debug_log), fmt, arg);
	va_end(arg);
	
	std::string str(tmp_debug_log);
	*/
	/*if(counter >= 0 && counter <= 5)
	{
		//debuglog[counter] = str.c_str();
		debuglog[counter] = fmt;
		counter++;
	}
	else
	{
		counter = 0;
	}*/
	return;
}

//char tmp_debug_rpc[0xFF];

#undef DebugRPC(a)
void DebugRPC(const char *fmt)
{
	/*memset(tmp_debug_rpc, 0, sizeof(tmp_debug_rpc));
	va_list arg;
	va_start(arg, fmt);
	vsnprintf(tmp_debug_rpc, sizeof(tmp_debug_rpc), fmt, arg);
	va_end(arg);
	
	std::string str(tmp_debug_rpc);	
	*/
	//debugrpc = str.c_str();
	//debugrpc = fmt;
	return;
}

char tmp_debug_hook[0xFF];

#undef DebugHook(a)
void DebugHook(const char *fmt)
{
	/*memset(tmp_debug_hook, 0, sizeof(tmp_debug_hook));
	va_list arg;
	va_start(arg, fmt);
	vsnprintf(tmp_debug_hook, sizeof(tmp_debug_hook), fmt, arg);
	va_end(arg);
	
	std::string str(tmp_debug_hook);	
	*/
	/*if(counterhook >= 0 && counterhook <= 5)
	{
		//debughook[counterhook] = str.c_str();
		debughook[counterhook] = fmt;
		counterhook++;
	}
	else
	{
		counterhook = 0;
	}*/
	return;
}

// end debug

uint32_t GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return (tv.tv_sec*1000+tv.tv_usec/1000);
}

#undef pack(a)
unsigned long pack(const char* enter)
{
	unsigned long uiAddr = strtoul(enter, nullptr, 16);
	uiAddr = g_libGTASA + uiAddr;
    return uiAddr;
}

#undef packSC(a)
unsigned long packSC(const char* enter)
{
	unsigned long uiAddr = strtoul(enter, nullptr, 16);
	uiAddr = g_libSCAnd + uiAddr;
    return uiAddr;
}

#undef decPACK(a)
int decPACK(const char* enter)
{
	int result;
    sscanf(enter, BOEV("%d"), &result);
    return result;
}

uint32_t s2h(const char* enter)
{
    uint32_t result;
    sscanf(enter,BOEV("%x"),&result);
    return result;
}

int s2d(const char* enter)
{
    int result;
    sscanf(enter, BOEV("%d"), &result);
    return result;
}

float s2f(const char* enter)
{
    float result;
    sscanf(enter, BOEV("%f"), &result);
    return result;
}