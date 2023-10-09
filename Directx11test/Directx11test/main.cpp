#include <iostream>
#include "FWork.h"
#include "MyRender.h"

int main() {
	

	BJFrameWork::FWork frame;
	MyRender* render = new MyRender();

	frame.SetRender(render);

	frame.Init(L"Hello", L"World", 1600, 1200, BJEUtils::_WINDOWED);
	render->InitParams();
	
	frame.Run();
	
	frame.Close();

	return 0;
}
