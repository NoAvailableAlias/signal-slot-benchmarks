#include <neolib/neolib.hpp>
#include <neolib/scoped.hpp>
#include <neolib/win32_message_queue.hpp>

#ifdef _WIN32

namespace neolib
{
    std::map<UINT_PTR, win32_message_queue*> win32_message_queue::sTimerMap;

    win32_message_queue::win32_message_queue(async_task& aIoTask, std::function<bool()> aIdleFunction, bool aCreateTimer) :
		iIoTask{ aIoTask },
		iIdleFunction{ aIdleFunction },
		iInIdle{ false }
    {
        if (aCreateTimer)
        {
            iTimer = ::SetTimer(NULL, 0, 10, &win32_message_queue::timer_proc);
            sTimerMap[iTimer] = this;
        }
    }

    win32_message_queue::~win32_message_queue()
    {
        for (auto& t : sTimerMap)
            ::KillTimer(NULL, t.first);
    }

    bool win32_message_queue::have_message() const
    {
        MSG msg;
        return ::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) != 0;
    }

    int win32_message_queue::get_message() const
    {
        MSG msg;
        int result = ::GetMessage(&msg, NULL, 0, 0);
        if (result)
        {
            if (result != -1)
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }
        return result;
    }

    void win32_message_queue::bump()
    {
        ::PostMessage(NULL, WM_NULL, 0, 0);
    }

	bool win32_message_queue::in_idle() const
	{
		return iInIdle;
	}

    void win32_message_queue::idle()
    {
        if (!in_idle() && iIdleFunction)
        {
			scoped_flag sf{ iInIdle };
            iIdleFunction();
        }
    }

    void CALLBACK win32_message_queue::timer_proc(HWND, UINT, UINT_PTR aId, DWORD)
    {
        win32_message_queue& instance = *sTimerMap[aId];
        instance.idle();
    }
}

#endif //_WIN32