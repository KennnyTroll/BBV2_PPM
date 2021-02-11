#include "common.hpp"
#include "features.hpp"
#include "fiber_pool.hpp"
#include "gui.hpp"
#include "logger.hpp"
#include "hooking.hpp"
#include "pointers.hpp"
#include "renderer.hpp"
#include "script_mgr.hpp"

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, PVOID)
{
	using namespace big;
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hmod);

		g_hmodule = hmod;

		static char g_logFilePath[MAX_PATH];
		memset(g_logFilePath, 0, sizeof(g_logFilePath));
		if (GetModuleFileNameA(hmod, g_logFilePath, MAX_PATH) != 0) {
			size_t slash = -1;

			for (size_t i = 0; i < strlen(g_logFilePath); i++) {
				if (g_logFilePath[i] == '/' || g_logFilePath[i] == '\\') {
					slash = i;
				}
			}
			if (slash != -1)
			{
				g_logFilePath[slash + 1] = '\0';
				strcpy_s(Menu::le_DLL_Path, g_logFilePath);
			}
		}

		g_main_thread = CreateThread(nullptr, 0, [](PVOID) -> DWORD
		{
			while (!FindWindow(L"grcWindow", L"Grand Theft Auto V"))
				std::this_thread::sleep_for(1s);

			auto logger_instance = std::make_unique<logger>();
			try
			{
				char chLogBuff[4096];
				struct tm current_tm;
				time_t current_time = time(NULL);
				localtime_s(&current_tm, &current_time);
				//sprintf_s(chLogBuff, "%02d/%02d/%d [%02d:%02d:%02d] PPM LOG Initialisation OK\n", current_tm.tm_mday, current_tm.tm_mon, (current_tm.tm_year + 1900), current_tm.tm_hour, current_tm.tm_min, current_tm.tm_sec);
				sprintf_s(chLogBuff, "%02d/%02d/%d Logger initialisation OK", current_tm.tm_mday, current_tm.tm_mon, (current_tm.tm_year + 1900));
				LOG(big::INFO_TO_FILE) << chLogBuff;
				LOG(big::INFO_TO_FILE) << Menu::le_DLL_Path;

				//Font Name: Bulbhead  
				//Font Name: Caligraphy2 
				//http: //patorjk.com/software/taag/#p=testall&f=Bulbhead&t=PPM
				LOG(RAW_GREEN_TO_CONSOLE) << u8R"kek(                                                    
        ##### ##         ##### ##         #####   ##    ##   
     ######  /###     ######  /###     ######  /#### #####   
    /#   /  /  ###   /#   /  /  ###   /#   /  /  ##### ##### 
   /    /  /    ### /    /  /    ### /    /  /   # ##  # ##  
       /  /      ##     /  /      ##     /  /    #     #     
      ## ##      ##    ## ##      ##    ## ##    #     #     
      ## ##      ##    ## ##      ##    ## ##    #     #     
    /### ##      /   /### ##      /     ## ##    #     #     
   / ### ##     /   / ### ##     /      ## ##    #     #     
      ## ######/       ## ######/       ## ##    #     ##    
      ## ######        ## ######        #  ##    #     ##    
      ## ##            ## ##               /     #      ##   
      ## ##            ## ##           /##/      #      ##   
      ## ##            ## ##          /  #####           ##  
 ##   ## ##       ##   ## ##         /     ##                 _  _  __ 
###   #  /       ###   #  /          #                       ( \/ )/  )
 ###    /         ###    /            ##                      \  /  )( 
  #####/           #####/                                      \/  (__)
    ###              ###                                     

)kek";

				g_settings.load();
				LOG(INFO) << "Settings Loaded.";
				
				auto pointers_instance = std::make_unique<pointers>();
				LOG(INFO) << "Pointers initialized.";

				auto renderer_instance = std::make_unique<renderer>();
				LOG(INFO) << "Renderer initialized.";

				auto fiber_pool_instance = std::make_unique<fiber_pool>(10);
				LOG(INFO) << "Fiber pool initialized.";

				auto hooking_instance = std::make_unique<hooking>();
				LOG(INFO) << "Hooking initialized.";

				g_script_mgr.add_script(std::make_unique<script>(&features::script_func));
				g_script_mgr.add_script(std::make_unique<script>(&gui::script_func));
				LOG(INFO) << "Scripts registered.";

				g_hooking->enable();
				LOG(INFO) << "Hooking enabled.";

				LOG(INFO) << ("============================================");
				LOG(HACKER) << ("Hello you !!");
				LOG(HACKER) << ("Keyboard control");
				LOG(HACKER) << ("Open/Close Menu Key = Inser");//OpenMenuKey
				LOG(HACKER) << ("Uninject Menu Key   = End");//UninjectLibraryKey
				LOG(INFO) << ("--------------------------------------------");
				LOG(HACKER) << ("XBox Gamepad control"); 
				LOG(HACKER) << ("Open/Close Menu Key = PAD_LEFT + PAD_A");//OpenMenuKey
				//LOG(HACKER) << ("Uninject Menu Key   = PAD_A");//UninjectLibraryKey
				LOG(INFO) << ("============================================");

				g_gui.m_opened = true;
									
				while (g_running)
				{
					std::this_thread::sleep_for(500ms);
				}

				g_hooking->disable();
				LOG(INFO) << "Hooking disabled.";

				std::this_thread::sleep_for(1000ms);

				g_script_mgr.remove_all_scripts();
				LOG(INFO) << "Scripts unregistered.";

				hooking_instance.reset();
				LOG(INFO) << "Hooking uninitialized.";

				fiber_pool_instance.reset();
				LOG(INFO) << "Fiber pool uninitialized.";

				renderer_instance.reset();
				LOG(INFO) << "Renderer uninitialized.";

				pointers_instance.reset();
				LOG(INFO) << "Pointers uninitialized.";
			}
			catch (std::exception const &ex)
			{
				LOG(WARNING) << ex.what();
				MessageBoxA(nullptr, ex.what(), nullptr, MB_OK | MB_ICONEXCLAMATION);
			}

			LOG(INFO) << "Farewell!";
			logger_instance.reset();

			CloseHandle(g_main_thread);
			FreeLibraryAndExitThread(g_hmodule, 0);
		}, nullptr, 0, &g_main_thread_id);
	}

	return true;
}
