#include <db.h>

namespace Net {
	char *recv_Ent(int sock, Db::Ent *x);
	char *send_Ent(int sock, Db::Ent *x);
}
