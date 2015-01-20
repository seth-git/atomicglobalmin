
#include "mpiUtils.h"

MpiUtil::~MpiUtil() {
	completeNonBlockingSends();
}

void MpiUtil::nonBlockingSend(const int* arr, int count, int dest, int tag) {
	IntSendRequestPair sendPair;
	if (NULL == arr) {
		sendPair.first = NULL;
	} else {
		sendPair.first = new int[count];
		memcpy(sendPair.first, arr, sizeof(int) * count);
	}
	sendPair.second = new MPI_Request;
	m_intSendRequests.push_back(sendPair);
	MPI_Isend(sendPair.first, count, MPI_INT, dest, tag, MPI_COMM_WORLD, sendPair.second);
}

void MpiUtil::completeNonBlockingSends() {
	MPI_Status status;
	int flag;
	for (std::list<IntSendRequestPair>::iterator it = m_intSendRequests.begin(); it != m_intSendRequests.end(); ++it) {
		MPI_Test(it->second, &flag, &status);
		if (!flag)
			MPI_Wait(it->second, &status);
		if (NULL != it->first)
			delete it->first;
		delete it->second;
	}
	m_intSendRequests.clear();
}

int MpiUtil::receiveString(int source, char* &char_buf, MPI_Status& status, bool blocking) {
	if (blocking) {
		MPI_Probe(source, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	} else {
		int flag = 0;
		MPI_Iprobe(source, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
		if (!flag)
			return flag;
	}
	int buffer_size;
	MPI_Get_count(&status, MPI_CHAR, &buffer_size);
	char_buf = new char[buffer_size];
	MPI_Recv(char_buf, buffer_size, MPI_CHAR, source, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	if (0 == buffer_size)
		delete char_buf;
	return 1;
}

int MpiUtil::receiveInt(int source, int* int_ptr, MPI_Status& status, bool blocking) {
	if (blocking) {
		MPI_Probe(source, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	} else {
		int flag = 0;
		MPI_Iprobe(source, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
		if (!flag)
			return flag;
	}
	int buffer_size;
	MPI_Get_count(&status, MPI_INT, &buffer_size);
	if (buffer_size > 1)
		throw "In MpiUtil::receiveInt, the buffer size should not be greater than one.";
	else
		MPI_Recv(int_ptr, buffer_size, MPI_INT, source, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	return 1;
}

int MpiUtil::receiveInts(int source, int* &int_buf, int &buffer_size, MPI_Status& status, bool blocking) {
	if (blocking) {
		MPI_Probe(source, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	} else {
		int flag = 0;
		MPI_Iprobe(source, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
		if (!flag)
			return flag;
	}
	MPI_Get_count(&status, MPI_INT, &buffer_size);
	int_buf = new int[buffer_size];
	MPI_Recv(int_buf, buffer_size, MPI_INT, source, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	if (0 == buffer_size)
		delete int_buf;
	return 1;
}
