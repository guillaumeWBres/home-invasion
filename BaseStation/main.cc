// C
#include <stdlib.h>
#include <fcntl.h>

// CXX
#include <sstream>
#include <fstream>
#include <iostream>

#include "BaseStation.h"

using namespace std;

int main(int argc, char **argv){
	BaseStation bs = BaseStation();
	bs.print();
	return 0;
}
