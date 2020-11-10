#include "HomeworkMarker.hpp"
#include "HomeworkNatural.hpp"

int main()
{
	HomeworkBase *app;
	//app = new HomeworkMarker();
	app = new HomeworkNatural();
	
	app->Run();

	return 0;
}

