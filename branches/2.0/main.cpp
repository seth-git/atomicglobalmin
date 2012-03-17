
#include "main.h"

// ----------------------------------------------------------------------
// main() for printing files named on the command line
// ----------------------------------------------------------------------
int main(int argc, char* argv[])
{
	for (int i=1; i<argc; i++)
	{
		Input input;
		input.load(argv[i]);
//		input.save("out.xml");
	}
	return 0;
}
