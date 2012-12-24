
#include "main.h"

// ----------------------------------------------------------------------
// main() for printing files named on the command line
// ----------------------------------------------------------------------
int main(int argc, char* argv[])
{
	if (!Strings::init())
		return 0;
	if (!Handbook::init())
		return 0;
	TiXmlBase::SetCondenseWhiteSpace(false);
	for (int i=1; i<argc; i++)
	{
		Input input;
		if (input.load(argv[i]))
			input.save("out.xml");
	}
	return 0;
}
