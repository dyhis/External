#include <windows.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <TlHelp32.h>
#include <thread>
#include <fstream>
#include <filesystem>

#include "render/render.h"
#include "cheat/SDK.h"
#include "cheat/loops/loop.h"
#include "cheat/ue5/ue5.h"
#include "cheat/driver/driver.h"
#include "cheat/driver/bytes.h"
#include "cheat/aim/aim.h"
#include "../backend/auth/auth.hpp"
#include "../backend/encryption/Importer.h"
#include "../backend/encryption/SpoofCall.h"
#include "../backend/mapper/global.h"
#include "../backend/mapper/binary/dropper.h"

//#include "../backend/encryption/qengine/engine/qengine.hpp"


std::string name = ("luminox");
std::string ownerid = ("y9zQNhWV1V");
std::string secret = ("1db33b3302a1a3bde407bf6190e1609cbe7e3120b118ccf3237591611369628f");
std::string version = ("1.0");
std::string url = ("https://keyauth.win/api/1.2/"); // change if you're self-hosting
std::string key;

const wchar_t* v = L"C:\\Windows\\luminox.sys";
const wchar_t* g = L"C:\\Windows\\gdrv.sys";

KeyAuth::api KeyAuthApp(name, ownerid, secret, version, url);

std::string drv;

int main()
{
	ue5::width = LI_FN(GetSystemMetrics).forwarded_safe_cached()(SM_CXSCREEN);
	ue5::height = LI_FN(GetSystemMetrics).forwarded_safe_cached()(SM_CYSCREEN);


	KeyAuthApp.init();

	std::cout << "\n  [+] Enter Licence -> ";

	std::cin >> key;

	KeyAuthApp.license(key);

	if (!KeyAuthApp.data.success)
	{
		exit(0);
	}

	std::cout << "\n  [+] load driver y/n -> ";

	std::cin >> drv;

	if (drv == "y")
	{
		if (DropDriverFromBytes(g))
		{
			system("cls");

			std::ofstream file(v, std::ios::binary);

			if (!file.is_open())
			{
				std::cerr << "Error Mapping Driver (3)!" << std::endl;
				Sleep(3000);
				exit(0);
			}

			// Write bytes to file
			file.write(reinterpret_cast<char*>(our_driver), sizeof(our_driver));

			// Close file stream
			file.close();

			NTSTATUS status = WindLoadDriver((PWCHAR)g, (PWCHAR)v, FALSE);

			Sleep(3000);

			system("cls");
		}

		DeleteFile(g);
		DeleteFile(v);
	}
	else if (drv == "n")
	{
	}
	else
	{
		std::cout << "\n  [-] Invalid Selection ";
		Sleep(3000);
		exit(0);
	}

	luminox->setup_driver();

	int result = MessageBox(NULL, L"Open Fortnite and press OK in lobby.", L"Luminox", MB_OK);

	if (result == IDOK) 
	{
		Sleep(500);
	}

	luminox->attach(L"FortniteClient-Win64-Shipping.exe");

	luminox->base = luminox->get_base_address();

	luminox->fix_dtb();

	std::cout << "\n  [+] Fortnite Base -> " << luminox->base;

	if (!luminox->base) 
	{
		std::cout << "\n  [+] Failed to get base press any key to close";
		std::cin.get();
		exit(0);
	}
	if (!_discord_->init())
	{
		std::cout << "\n  [+] Failed to overlay";
		std::cin.get();
		exit(0);
	}
	if (!_render::imgui())
	{
		std::cout << "\n  [+] Failed to imgui";
		std::cin.get();
		exit(0);
	}
	
	std::thread(loop::cache_pointers).detach();
	std::thread(loop::cache_entities).detach();
	std::thread(loop::local_weapon).detach();

	HWND console = GetConsoleWindow();
	ShowWindow(console, SW_HIDE);

	_render::render();

	return 0;
}
