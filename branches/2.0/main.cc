
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

	if (!Strings::init())
		return 0;
	if (!Handbook::init())
		return 0;
	TiXmlBase::SetCondenseWhiteSpace(false);

	MPI_Init(&argc, &argv);
	int iRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &iRank);
	int returnValue;
	Input input;
	if (!Random::init(iRank))
		returnValue = 0;
	else
		returnValue = (int)input.run(argv[1]);
	MPI_Finalize();
	return returnValue;
}
