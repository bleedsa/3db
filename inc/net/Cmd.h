#ifndef __3DB_NET_CMD_H__
#define __3DB_NET_CMD_H__

#include <cmd.h>

namespace Net {
	char *send_Cmd(int sock, Cmd::Cmd *x);
	Cmd::Cmd get_cmd(int sock);

	char *send_Insert(int sock, Cmd::Insert *x);
	char *recv_Insert(int sock, Cmd::Insert *x);

	char *send_Create(int sock, Cmd::Create *x);
	char *recv_Create(int sock, Cmd::Create *x);

}

#endif
