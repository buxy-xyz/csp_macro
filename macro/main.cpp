#include <iostream>
#include <windows.h>

#include <string>
#include <array>
#include <cstdarg>
#include <vector>
#include <algorithm>
#include <filesystem>

#include <TlHelp32.h>

#include "discord_rpc.h"

#pragma warning(disable:4996) // ignore _CRT_SECURE_NO_WARNINGS

int toolIndex = 0;
std::string toolIndexString;
std::string toolIndexToolTip;

BYTE scan_code(DWORD pKey)
{
	const DWORD result = MapVirtualKey(pKey, MAPVK_VK_TO_VSC);

	return static_cast<BYTE>(result);
}

void press_key(DWORD pKey)
{
	keybd_event(static_cast<BYTE>(pKey), scan_code(pKey), 0, 0);
}

void release_key(DWORD pKey)
{
	keybd_event(static_cast<BYTE>(pKey), scan_code(pKey), KEYEVENTF_KEYUP, 0);
}
#define PRESS(x) press_key(x)
#define RELEASE(x) release_key(x)

bool bCharge = false;

void short_cuts()
{
	/*
	if ((GetAsyncKeyState(VK_NUMPAD0) & 0x8000)) // ctrl + shift + t
	{
		PRESS(VK_CONTROL);
		PRESS(VK_LSHIFT);
		PRESS(0x54);

		Sleep(20);

		RELEASE(VK_CONTROL);
		RELEASE(VK_LSHIFT);
		RELEASE(0x54);
	}
	*/

	if ((GetAsyncKeyState(VK_NUMPAD0) & 0x8000)) // ctrl + shift + t
	{
		PRESS(VK_CONTROL);
		PRESS(VK_LSHIFT);
		PRESS(0x54);

		Sleep(20);

		toolIndex = 1; // selection

		RELEASE(VK_CONTROL);
		RELEASE(VK_LSHIFT);
		RELEASE(0x54);
	}

	if ((GetAsyncKeyState(VK_DECIMAL) & 0x8000)) // ctrl + t
	{
		PRESS(VK_DELETE);

		Sleep(20);

		toolIndex = 1; // selection

		RELEASE(VK_DELETE);
	}

	if ((GetAsyncKeyState(VK_NUMPAD1) & 0x8000)) // p
	{
		PRESS(0x50);

		Sleep(20);

		toolIndex = 2; // ink

		RELEASE(0x50);
	}

	if ((GetAsyncKeyState(VK_NUMPAD2) & 0x8000)) // e
	{
		PRESS(0x45);

		Sleep(20); 

		toolIndex = 3; // eraser

		RELEASE(0x45);
	}

	if ((GetAsyncKeyState(VK_NUMPAD3) & 0x8000)) // m
	{
		PRESS(0x4D);

		Sleep(20);

		toolIndex = 1; // moving ?

		RELEASE(0x4D);
	}

	if ((GetAsyncKeyState(VK_NUMPAD7) & 0x8000)) // ctrl + z
	{
		PRESS(VK_CONTROL);
		PRESS(0x5A);

		Sleep(20);



		RELEASE(VK_CONTROL);
		RELEASE(0x5A);
	}

	if ((GetAsyncKeyState(VK_NUMPAD8) & 0x8000)) // ctrl + y  
	{
		PRESS(VK_CONTROL);
		PRESS(0x59);

		Sleep(20);

		RELEASE(VK_CONTROL);
		RELEASE(0x59);
	}

	/*
	if ((GetAsyncKeyState(VK_NUMPAD1) & 0x8000))
	{
		PRESS(VK_CONTROL);
		PRESS(0x54);

		Sleep(20);

		RELEASE(VK_CONTROL);
		RELEASE(0x54);
	}
	*/
}

inline std::string to_lower(std::string str)
{
	transform(str.begin(), str.end(), str.begin(), static_cast<int(*)(int)>(::tolower));
	return str;
}
inline std::string to_upper(std::string str)
{
	transform(str.begin(), str.end(), str.begin(), static_cast<int(*)(int)>(::toupper));
	return str;
}

inline bool open_process(std::string path, std::vector<std::string> att, PROCESS_INFORMATION& proc)
{
	STARTUPINFO si;
	{
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
	}

	std::string wstr{};
	wstr += path;

	for (const auto& i : att)
		wstr += " " + i;

	return CreateProcessA(nullptr, const_cast<char*>(wstr.c_str()), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &proc);
}

inline bool is_process_open(const std::string& proc)
{
	if (proc.empty())
		return false;

	auto file = proc;
	if (file.find_last_of(".") != std::string::npos)
		file.erase(file.find_last_of("."), std::string::npos);

	file += ".exe";
	const auto handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 entry; entry.dwSize = sizeof(entry);

	if (!Process32First(handle, &entry))
		return false;

	do
	{
		if (to_lower(entry.szExeFile).compare(to_lower(file)) == 0)
		{
			const auto process = OpenProcess(PROCESS_VM_READ, false, entry.th32ProcessID);
			if (process != nullptr)
				CloseHandle(process);

			CloseHandle(handle);
			return true;
		}
	} while (Process32Next(handle, &entry));

	return false;
}

std::string text;
std::time_t result = std::time(nullptr);

void update_presence()
{
	switch (toolIndex)
	{
	case 1:
		text = "Using Selection area";
		toolIndexString = "selection";
		toolIndexToolTip = "Using Selection area";
		break;

	case 2:
		text = "Using Pen";
		toolIndexString = "ink";
		toolIndexToolTip = "Using Pen";
		break;

	case 3:
		text = "Using Eraser";
		toolIndexString = "eraser";
		toolIndexToolTip = "Using Eraser";
		break;

	case 4:

		break;

	case 5:

		break;
	}

	/*
	srand(time(NULL));
	const char arrayNum[45] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13', '14', '15', '16', '17', '18', '19', '20', '21', '22', '23', '24', '25', '26', '27', '28', '29', '30', '31', '32', '33', '34', '35', '36', '37', '38', '39', '40', '41', '42' , '43', '44'};
	int RandIndex = rand() % 45;

	switch (RandIndex)
	{
	case 0:
		text = "application unresponsive";
		break;

	case 1:
		text = "bruh";
		break;

	case 2:
		text = "vine boom";
		break;

	case 3:
		text = "lights off...";
		break;

	case 4:
		text = "illegal activities";
		break;

	case 5:
		text = "tracing art";
		break;

	case 6:
		text = "charging 200$ for a commission";
		break;

	case 7:
		text = "vruh";
		break;

	case 8:
		text = "ripping PS brushes";
		break;

	case 9:
		text = "hhhhhh";
		break;

	case 10:
		text = "yea no";
		break;

	case 11:
		text = "doing ur comm rn, prob not";
		break;

	case 12:
		text = ":eyes:";
		break;

	case 13:
		text = "b to the u to the x to the y";
		break;

	case 14:
		text = "lol";
		break;

	case 15:
		text = "why are u reading this lol";
		break;

	case 16:
		text = "fucken";
		break;

	case 17:
		text = "how";
		break;

	case 18:
		text = ":^)";
		break;

	case 19:
		text = "bro ima mem leak";
		break;

	case 20:
		text = "what if we kissed at ko-fi.com/buxy_xyz";
		break;

	case 21:
		text = "bro";
		break;

	case 22:
		text = "butte";
		break;

	case 23:
		text = "lmfao";
		break;

	case 24:
		text = "lmao.csp";
		break;

	case 25:
		text = "poggg";
		break;

	case 26:
		text = "butch hartman";
		break;

	case 27:
		text = "brb taking a huge sh";
		break;

	case 28:
		text = "rgb razer gaming";
		break;

	case 29:
		text = "nvidia rtx 1031";
		break;

	case 30:
		text = "accidentally drew in sketch layer";
		break;

	case 31:
		text = "96% cpu usage";
		break;

	case 32:
		text = "corny";
		break;

	case 33:
		text = "playing pokemon go";
		break;

	case 34:
		text = "opening canvas";
		break;

	case 35:
		text = "windows update available";
		break;

	case 36:
		text = "yoo this gameplay is fire";
		break;

	case 37:
		text = "need more nut jokes";
		break;

	case 38:
		text = "buxbux";
		break;

	case 39:
		text = "out of wam";
		break;

	case 40:
		text = "cbt";
		break;

	case 41:
		text = "mario paint";
		break;

	case 42:
		text = "icarly";
		break;

	case 43:
		text = "snoozn";
		break;

	case 44:
		text = "ykno just vibin";
		break;
	}
	*/

	system("cls");

	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	//discordPresence.state = "";
	discordPresence.details = text.c_str();
	discordPresence.startTimestamp = result;
	discordPresence.largeImageKey = "clip_studio_paint";
	discordPresence.largeImageText = "Drawing in Clip Studio Paint";
	discordPresence.smallImageKey = toolIndexString.c_str();
	discordPresence.smallImageText = toolIndexToolTip.c_str();
	Discord_UpdatePresence(&discordPresence);

	printf_s(text.c_str());
}

static unsigned long __stdcall cspStartup(void*)
{
	if (!is_process_open("CLIPStudioPaint.exe"))
	{
		// C:\Program Files\CELSYS\CLIP STUDIO 1.5\CLIP STUDIO PAINT
		system("\"C:\\Program Files\\CELSYS\\CLIP STUDIO 1.5\\CLIP STUDIO PAINT\\CLIPStudioPaint.exe\"");
	}
}

static unsigned long __stdcall updateHook(void*)
{
	for (;;)
	{
		if (is_process_open("CLIPStudioPaint.exe") == true)
		{
			update_presence();
		}

		if (is_process_open("CLIPStudioPaint.exe") == false)
		{
			exit(0);
		}

		Sleep(20 * 10 * 100);
	}
	return EXIT_SUCCESS;
}

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	SetConsoleTitle("\n");

	HANDLE hCspStartup = NULL;
	hCspStartup = CreateThread(NULL, NULL, cspStartup, NULL, NULL, NULL);
	Sleep(10 * 10 * 100);
	TerminateThread(cspStartup, 0);

	HANDLE hUpdateHook = NULL;
	hUpdateHook = CreateThread(NULL, NULL, updateHook, NULL, NULL, NULL);

	DiscordEventHandlers Handler;
	memset(&Handler, 0, sizeof(Handler));
	Discord_Initialize("782365066636165160", &Handler, 1, NULL);

	while (true)
	{
		bCharge = true;

		while ((GetAsyncKeyState(VK_NUMPAD0) & 0x8000) ||
			(GetAsyncKeyState(VK_DECIMAL) & 0x8000) ||
			(GetAsyncKeyState(VK_NUMPAD1) & 0x8000) ||
			(GetAsyncKeyState(VK_NUMPAD2) & 0x8000) ||
			(GetAsyncKeyState(VK_NUMPAD3) & 0x8000) ||
			(GetAsyncKeyState(VK_NUMPAD7) & 0x8000) ||
			(GetAsyncKeyState(VK_NUMPAD8)) & 0x8000)
		{
			if (bCharge == true)
			{
				short_cuts();

				bCharge = false;
			}
			Sleep(1);
		}
		Sleep(1);
	}
	return 0;
}