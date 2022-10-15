#pragma once
#include <Windows.h>
#include <TlHelp32.h>

namespace um
{
	inline HANDLE hProcess;
	inline DWORD pid;

	static bool Init(HWND hwnd)
	{
		GetWindowThreadProcessId(hwnd, &pid);
		if (!pid)
		{
			return false;
		}
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
		if (!hProcess)
		{
			return false;
		}

		return true;
	}

	template<typename T>
	static NTSTATUS ReadMemory(void* TargetAddress, T* Buffer)
	{
		return ReadProcessMemory(hProcess, TargetAddress, Buffer, sizeof(T), 0) == false;
	}
	static NTSTATUS ReadMemory(void* TargetAddress, void* Buffer, DWORD size)
	{
		return ReadProcessMemory(hProcess, TargetAddress, Buffer, size, 0) == false;
	}
	template<typename T>
	static NTSTATUS WriteMemory(void* TargetAddress, T* Buffer)
	{
		return WriteProcessMemory(hProcess, TargetAddress, Buffer, sizeof(T), 0) == false;
	}
	static void* GetModuleBase(const wchar_t* pName)
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
		void* dwModuleBaseAddress = 0;
		if (hSnapshot != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32 ModuleEntry32 = { 0 };
			ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
			if (Module32First(hSnapshot, &ModuleEntry32))
			{
				do
				{
					if (wcscmp(ModuleEntry32.szModule, pName) == 0)
					{
						dwModuleBaseAddress = ModuleEntry32.modBaseAddr;
						break;
					}
				} while (Module32Next(hSnapshot, &ModuleEntry32));
			}
			CloseHandle(hSnapshot);
		}
		return dwModuleBaseAddress;
	}
	static size_t GetModuleSize(const wchar_t* pName)
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
		size_t szModuleSize = 0;
		if (hSnapshot != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32 ModuleEntry32 = { 0 };
			ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
			if (Module32First(hSnapshot, &ModuleEntry32))
			{
				do
				{
					if (wcscmp(ModuleEntry32.szModule, pName) == 0)
					{
						szModuleSize = ModuleEntry32.modBaseSize;
						break;
					}
				} while (Module32Next(hSnapshot, &ModuleEntry32));
			}
			CloseHandle(hSnapshot);
		}
		return szModuleSize;
	}
	static uintptr_t SignatureScan(uintptr_t start, unsigned long long size, const char* sig)
	{
		unsigned char* data = (unsigned char*)VirtualAlloc(0x0, size, MEM_COMMIT, PAGE_READWRITE);
		if (!data)
			return 0;

		ReadMemory((void*)start, data, size);

		static auto pattern_to_byte = [](const char* pattern) {
			auto bytes = std::vector<int>{};
			auto start = const_cast<char*>(pattern);
			auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current) {
				if (*current == '?') {
					++current;
					if (*current == '?')
						++current;
					bytes.push_back(-1);
				}
				else {
					bytes.push_back(strtoul(current, &current, 16));
				}
			}
			return bytes;
		};

		auto patternBytes = pattern_to_byte(sig);
		auto scanBytes = reinterpret_cast<unsigned char*>(data);

		auto s = patternBytes.size();
		auto d = patternBytes.data();

		for (auto i = 0ul; i < size - s; ++i) {
			bool found = true;
			for (auto j = 0ul; j < s; ++j) {
				if (scanBytes[i + j] != d[j] && d[j] != -1) {
					found = false;
					break;
				}
			}
			if (found) {
				VirtualFree(data, size, MEM_DECOMMIT);
				return start + i;
			}
		}

		VirtualFree(data, size, MEM_DECOMMIT);
		return 0;
	}
	static uintptr_t SignatureScan(const wchar_t* ModuleName, const char* sig)
	{
		return SignatureScan((uintptr_t)GetModuleBase(ModuleName), GetModuleSize(ModuleName), sig);
	}
}


namespace helper
{
	static uintptr_t relativeToAbsolute(uintptr_t address, int offset_to_delta = 1)
	{
		if (!address)
			return 0;
		int delta;
		um::ReadMemory((void*)(address + offset_to_delta), &delta);

		return (uintptr_t)(address + offset_to_delta + 4 + delta);
	}
}