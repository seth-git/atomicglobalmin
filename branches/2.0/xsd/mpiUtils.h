
#ifndef __MPI_UTILS_H__
#define __MPI_UTILS_H__

#include <mpi.h>

#define DIE_TAG                   1
#define WORK_TAG                  2
#define FINISH_TAG                3
#define ENERGY_CAL_FAILURE_TAG    4
#define WALL_TIME_TAG             5

#define MPI_DEBUG	false

class MpiUtil {
public:
	MpiUtil() {}

	/**************************************************************************
	 * Purpose: to receive an xml message
	 * Parameters: source: rank of the sending process or MPI_ANY_SOURCE
	 *             char_buf: where the xml should be stored
	 *             status: contains information about the message
	 *             blocking: if true, waits until a message is received.
	 * Returns: 1 if a message was received (always the case for blocking)
	 */
	static int receiveString(int source, char* &char_buf, MPI_Status& status, bool blocking);

	/**************************************************************************
	 * Purpose: to receive an integer array
	 * Parameters: source: rank of the sending process or MPI_ANY_SOURCE
	 *             int_buf: where the array should be stored
	 *             status: contains information about the message
	 *             blocking: if true, waits until a message is received.
	 * Returns: 1 if a message was received (always the case for blocking)
	 */
	static int receiveArray(int source, int* &int_buf, int &buffer_size, MPI_Status& status, bool blocking);
};

#endif
