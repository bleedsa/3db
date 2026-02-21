#ifndef __3DB_CMD_H__
#define __3DB_CMD_H__

#include <Q.h>
#include <db.h>

namespace Cmd {
	enum CmdTy {
		NIL,
		SELECT,
		INSERT,
		CREATE,
	};

	extern const char *CmdTy_names[4];

	struct Cmd {
		CmdTy ty;
		bool net;
		int sock;

		Cmd(int fd);
		Cmd(CmdTy ty, int fd);
		Cmd(CmdTy ty, const char *addr);
		Cmd(CmdTy ty);
		virtual ~Cmd() {};
		Cmd(const Cmd &x);
		Cmd &operator=(const Cmd &x);

		virtual Cmd &entry(var_t name);
		virtual Cmd &entry(const char *name);

		/* cpy */
		inl auto cpy(const Cmd &x) {
			ty = x.ty, net = x.net, sock = x.sock;
		}

		/* return the type as a string */
		inl auto type_name() -> const char* {
			return CmdTy_names[(S)ty];
		}

		inl auto type(CmdTy t) -> Cmd& {
			ty = t;
			return *this;
		}

		/* exe executes the query on the backend. It modifies
		 * the array on disk. for networked access, use send. */
		virtual R<Db::Ent*> exe();

		/* send a command over the wire. */
		virtual R<Db::Ent> send();
	};

	/* a create command */
	struct Create : public Cmd {
		var_t name;
		Db::EntTy entty;

		Create();
		Create(int fd);
		Create(const char *addr);
		Create(var_t name, Db::EntTy ty);
		virtual ~Create() override;
		Create(const Create &x);
		Create &operator=(const Create &x);

		inl auto cpy(const Create &x) -> void {
			name = x.name, entty = x.entty;
		}

		virtual Create &entry(var_t name) override;
		virtual Create &entry(const char *name) override;

		virtual R<Db::Ent*> exe() override;
		virtual R<Db::Ent> send() override;
	};

	/* an insert command */
	struct Insert : public Cmd {
		var_t name;
		S row;
		A::A<Q::Q> cols;

		Insert();
		Insert(int fd);
		Insert(const char *addr);
		Insert(var_t name, S row, A::A<Q::Q> cols);
		virtual ~Insert() override;
		Insert(const Insert &x);
		Insert &operator=(const Insert &x);

		inl auto cpy(const Insert &x) -> void {
			name = x.name, row = x.row, cols = x.cols;
		}

		virtual Insert &entry(var_t name) override;
		virtual Insert &entry(const char *name) override;

		R<void> insert(T::T*);
		virtual R<Db::Ent*> exe() override;
		virtual R<Db::Ent> send() override;
	};
}

typedef union {
	Cmd::Cmd cmd;
	Cmd::Create create;
	Cmd::Insert insert;
} cmd_t;

namespace Fmt {
	std::string Fmt(Cmd::Cmd *x);
}

#endif
