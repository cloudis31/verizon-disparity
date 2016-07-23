#include "appcontext.h"

int main(int argc, char* argv[])
{
	FreeConsole();

	AppContext::Instance()->Run();

	return 0;
}