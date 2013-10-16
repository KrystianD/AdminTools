#ifndef __COMMON_H__
#define __COMMON_H__

#include <vector>
using namespace std;

typedef vector<char> buffer_t;

int recvall (int sockfd, void* data, int size, int timeout);
int sendall (int sockfd, void* data, int size);

#endif
