#include "CustomHooks.h"


using namespace std;
std::ofstream outfile;
std::mutex mtx;

unsigned int __stdcall ExitHook(void* data)
{
	Sleep(60000);
	MH_DisableHook(&LoadIconA);
	MH_Uninitialize();
	return 0;
}

HANDLE Current = GetCurrentProcess();


LPVOID GetArchiveFunctionAddress()
{
	unsigned short scanBytes[19] = { 0x41, 0x56, 0x48, 0x81, 0xEC, 0xF0, 0x00, 0x00, 0x00, 0x44, 0x8B, 0x42, 0x08, 0x4C, 0x8B, 0xF1, 0x45, 0x85, 0xC0 };
	return AOBScanner::GetSingleton()->Scan(scanBytes, 19);
}



typedef INT64* (*HOOKON)(INT64*, UINT64);

HOOKON fpHookOn = NULL;



INT64* tHookOn(INT64* unk1, INT64 a2)
{
	int rax = 0;
	GetRax(rax);
	int v2 = *(UINT*)(a2 + 8);
	char v10 = 0;
	GetAL(v10);
	int size = 0;
	mtx.lock();
	if ((DWORD)v2)
	{
		char* v5 = *(char**)a2;
		char* v6 = *(char**)a2;

		char* v8 = &v5[v2];

		while (v6 != v8)
		{
			char v9 = *v6;
			if (*v6 == '/' || v9 == '\\')
			{
				do
				{
					do {
						v10 = (v6++)[1];
						size += 1;
					} while (v10 == '/');
				} while (v10 == '\\');
			}
			else {
				if (v9 == '"' || v9 == '\'')
					break;
				++v6;
				size += 1;
			}
			

		}
		v6 -= size;

		
		std::string str(v5, size);


		for (int i = 0; i < size; i++)
		{
			if (str[i] == '/')
				outfile << '\\';
			else
				outfile << char(tolower(str[i]));
		}

	}
	SetRax(rax);

	INT64* res = fpHookOn(unk1, a2);

	if (size > 0)
	{
		outfile << ',';
		UINT64 hash = *res;
		outfile << hash;
		outfile << std::endl;
	}
	mtx.unlock();
	return res;
}


void UnHooks()
{
	MH_DisableHook(MH_ALL_HOOKS);
	outfile.close();
}

void SetupHooks()
{
	LPVOID hookAddress = GetArchiveFunctionAddress();
	MH_Initialize();

	outfile.open("Cyberpunk2077.log", std::ios::out | std::ios::app);
	if (MH_CreateHook(hookAddress, &tHookOn, reinterpret_cast<LPVOID*>(&fpHookOn)) != MH_OK)
	{
		return;
	}
	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
	{
		return;
	}

	//HANDLE myhandleB = (HANDLE)_beginthreadex(0, 0, &ExitHook, 0, 0, 0);
}



