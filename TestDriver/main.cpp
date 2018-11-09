#define _CRT_SECURE_NO_WARNINGS
#include"syscallmon.h"
#include<iostream>
int main()
{
	CSyscallMon Mon;
   	if (!Mon.Initialize())
	{
		std::cout << "initial falied .0" << std::endl;
		//return 0;
	}
	int i = 1;
	while (i)
	{
		std::cin >> i;
		Sleep(1000);
	}
	Mon.Uninitialize();

	return 0;
}