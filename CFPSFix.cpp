#include "CFPSFix.h"
#include "main.h"
#include <sys/syscall.h>

static void setThreadAffinityMask(pid_t tid, uint32_t mask)
{
	//Log("setThreadAffinityMask %d %d", tid, mask);
	if (syscall(__NR_sched_setaffinity, tid, sizeof(mask), &mask))
	{
		Log("Could not set thread affinity: mask=0x%x err=0x%x", mask, errno);
	}
}

void CFPSFix::Routine()
{

	while (true)
	{
		m_Mutex.lock();
		for (auto& i : m_Threads)
		{
			uint32_t mask = 0xff;

			setThreadAffinityMask(i, mask);
		}
		m_Mutex.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

CFPSFix::CFPSFix()
{
	std::thread(&CFPSFix::Routine, this).detach();
}

CFPSFix::~CFPSFix()
{
}

void CFPSFix::PushThread(pid_t tid)
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	m_Threads.push_back(tid);
}
