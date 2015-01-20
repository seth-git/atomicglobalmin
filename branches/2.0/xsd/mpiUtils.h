
#ifndef __MPI_UTILS_H__
#define __MPI_UTILS_H__

#include <mpi.h>
#include <list>

#define MPI_DEBUG	false

class MpiUtil {
public:
	MpiUtil() {}
	~MpiUtil();

	void nonBlockingSend(const int* arr, int length, int dest, int tag);
	void completeNonBlockingSends();

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
	 * Purpose: to receive an integer
	 * Parameters: source: rank of the sending process or MPI_ANY_SOURCE
	 *             int_ptr: where the int should be stored
	 *             status: contains information about the message
	 *             blocking: if true, waits until a message is received.
	 * Returns: 1 if a message was received (always the case for blocking)
	 */
	static int receiveInt(int source, int* int_ptr, MPI_Status& status, bool blocking);

	/**************************************************************************
	 * Purpose: to receive an integer array
	 * Parameters: source: rank of the sending process or MPI_ANY_SOURCE
	 *             int_buf: where the array should be stored
	 *             status: contains information about the message
	 *             blocking: if true, waits until a message is received.
	 * Returns: 1 if a message was received (always the case for blocking)
	 */
	static int receiveInts(int source, int* &int_buf, int &buffer_size, MPI_Status& status, bool blocking);

protected:
	typedef std::pair<int*,MPI_Request*> IntSendRequestPair;
	std::list<IntSendRequestPair> m_intSendRequests;
};

#endif
