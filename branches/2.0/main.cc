
#include "main.h"

// ----------------------------------------------------------------------
// main() for printing files named on the command line
// ----------------------------------------------------------------------
int main(int argc, char* argv[])
{
	if (argc != 2) {
		printf("The agml program takes one input file as a parameter.\n");
		return 0;
	}

	if (!Random::init(1))
		return 0;
	if (!Strings::init())
		return 0;
	if (!Handbook::init())
		return 0;
	TiXmlBase::SetCondenseWhiteSpace(false);

	Input input;
	if (!input.load(argv[1]))
		return 0;
	if (!input.m_pAction->run())
		return 0;
	return 1;
}
