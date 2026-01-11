#ifndef __3DB_NET_Q_H__
#define __3DB_NET_Q_H__

#include <Q.h>

namespace Net {
	char *send_Q(int sock, Q::Q *x);
	char *recv_Q(int sock, Q::Q *x);
}

#endif
