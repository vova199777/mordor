#pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include <list>
#include <unistd.h>

#define SAMP_VERSION	"0.3.7"

#define MAX_PLAYERS		1004
#define MAX_VEHICLES	2000
#define MAX_PLAYER_NAME	24

#define CLIENT_VERS		101

#define RAKSAMP_CLIENT
#define NETCODE_CONNCOOKIELULZ 0x6969
#include "vendor/RakNet/SAMP/samp_netencr.h"
#include "vendor/RakNet/SAMP/SAMPRPC.h"

#define STBI_ONLY_PNG
#include "vendor/imgui/stb_image.h"

#include "util/util.h"

#include "obfuscate.h"

extern uintptr_t g_libGTASA;
extern uintptr_t g_libSCAnd;
extern void *g_libBASS;
extern const char* g_pszStorage;
extern bool ShowAll;

extern int	sensx[18];
extern int	sensy[18];

extern char materials[2048];

extern bool climb;

void Log(const char *fmt, ...);
uint32_t GetTickCount();

void lastfunc(const char *fmt, ...);
void DebugLog(const char *fmt);
void DebugRPC(const char *fmt);
void DebugHook(const char *fmt);

void InitFiles();

uint32_t s2h(const char* enter);

int s2d(const char* enter);
float s2f(const char* enter);

unsigned long pack(const char* enter);
unsigned long packSC(const char* enter);
int decPACK(const char* enter);

extern uintptr_t g_atomicmodels;

#define Log(a, ...) Log(BOEV(a),##__VA_ARGS__)
#define lastfunc(a, ...) lastfunc(BOEV(a),##__VA_ARGS__)

#define DebugLog(a) DebugLog(BOEV(a))
#define DebugRPC(a) DebugRPC(BOEV(a))
#define DebugHook(a) DebugHook(BOEV(a))

#define pack(a) pack(BOEV(a))
#define packSC(a) packSC(BOEV(a))
#define decPACK(a) decPACK(BOEV("a"))