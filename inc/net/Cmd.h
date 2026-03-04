#ifndef __3DB_NET_CMD_H__
#define __3DB_NET_CMD_H__

#include <net.h>
#include <cmd.h>

namespace Net {
	char *send_Cmd(int sock, Cmd::Cmd *x);
	char *recv_Cmd(int sock, Cmd::Cmd *x);

	char *send_Insert(int sock, Cmd::Insert *x);
	char *recv_Insert(int sock, Cmd::Insert *x);

	char *send_Create(int sock, Cmd::Create *x);
	char *recv_Create(int sock, Cmd::Create *x);

	char *send_Get(int sock, Cmd::Get *x);
	char *recv_Get(int sock, Cmd::Get *x);
}

#endif
