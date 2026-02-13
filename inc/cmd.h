#ifndef __3DB_CMD_H__
#define __3DB_CMD_H__

#include <Q.h>
#include <db.h>
#include <net.h>

namespace Cmd {
	enum CmdTy {
		NIL,
		SELECT,
		INSERT,
	};

	extern const char *CmdTy_names[3];

	struct Cmd {
		CmdTy ty;
		bool net;
		int sock;

		Cmd(const char *addr);
		Cmd(CmdTy ty);
		virtual ~Cmd();
		Cmd(const Cmd &x);
		Cmd &operator=(const Cmd &x);

		/* return the type as a string */
		inl auto type() -> const char* {
			return CmdTy_names[(S)ty];
		}

		/* set the entry the command is targeting */
		virtual Cmd &entry(var_t name) = 0;
		inl auto entry(const char *name) -> Cmd& {
			auto n = str_to_var(name);
			return this->entry(n);
		}

		virtual var_t entry_name() = 0;

		/* exe executes the query on the backend. It modifies
		 * the array on disk. for networked access, use send. */
		virtual R<Db::Ent*> exe() = 0;

		/* send a command over the wire. */
		virtual R<Db::Ent*> send() = 0;
	};

	struct Insert : public Cmd {
		var_t name;
		S row;
		A::A<Q::Q> cols;

		Insert(const char *addr);
		Insert(var_t name, S row, A::A<Q::Q> cols);
		~Insert() = default;
		Insert(const Insert &x);
		Insert &operator=(const Insert &x);

		Cmd &entry(var_t name);

		R<void> insert(T::T*);
		virtual R<Db::Ent*> exe();
		virtual R<Db::Ent*> send();
	};
}

namespace Fmt {
	std::string Fmt(Cmd::Cmd *x);
}

#endif
