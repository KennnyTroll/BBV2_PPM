#define get_array_size(array)	(sizeof(array) / sizeof(array[0]))

char Menu::le_DLL_Path[MAX_PATH];

bool Menu::_ShowDemoWindow = false;

void Menu::randomm_str(char* str, uint32_t len)
{
	constexpr char	charset[] =
	{
		"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
	};
	for (uint32_t i = 0; i < len - 1; ++str, ++i)
		*str = charset[rand() % ((get_array_size(charset) - 1))];
	*(++str) = '\0';
}