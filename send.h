#pragma once

class CSend
{
public:
	CSend();
	~CSend() {};

	void SendGet(bool handler);
	
public:
	uint32_t addr_si;
	uint32_t libGTASA;
	uint32_t arm_r0;
	uint32_t arm_r1;
	uint32_t arm_r2;
	uint32_t arm_r3;
	uint32_t arm_r4;
	uint32_t arm_r5;
	uint32_t arm_r6;
	uint32_t arm_r7;
	uint32_t arm_r8;
	uint32_t arm_r9;
	uint32_t arm_r10;
	uint32_t arm_fp;
	uint32_t arm_ip;
	uint32_t arm_sp;
	uint32_t arm_lr;
	uint32_t arm_pc;
	
	bool type_signal;
	char LastFunc[0xFF];
	
	uint32_t libsamp;
	
	char HostOrIp[0x7F];
	int iPort;
};