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

	/* a create command */
	struct Create {
		var_t name;
		Db::EntTy ty;

		inl Create() : name{empty_var()}, ty{Db::Int} {};
		~Create() = default;

		inl auto cpy(const Create &x) -> void {
			name = x.name, ty = x.ty;
		}

		inl Create(const Create &x) {cpy(x);}
		Create &operator=(const Create &x) {cpy(x);return *this;}
	};

	/* an insert command */
	struct Insert {
		var_t name;
		S row;
		A::A<Q::Q> cols;

		inl Insert() : name{empty_var()},row{0},cols{A::A<Q::Q>(0)} {}
		~Insert() = default;

		inl auto cpy(const Insert &x) -> void {
			name = x.name, row = x.row, cols = x.cols;
		}

		Insert(const Insert &x) : cols{x.cols} {cpy(x);}
		Insert &operator=(const Insert &x) {cpy(x);return *this;}
	};

	struct Cmd {
		CmdTy ty;
		int sock;
		union {
			Create create;
			Insert insert;
		};

		Cmd(int fd);
		Cmd(CmdTy ty, int fd);
		Cmd(CmdTy ty, const char *addr);
		~Cmd();
		void cpy(const Cmd &x);
		Cmd(const Cmd &x);
		Cmd &operator=(const Cmd &x);

		/* return the type as a string */
		inl auto type_name() -> const char* {
			return CmdTy_names[(S)ty];
		}

		inl auto entry(var_t name) -> Cmd& {
			switch (ty) {
			CASE(CREATE, create.name = name)
			CASE(INSERT, insert.name = name)
			default: throw str_fmt("{} has no entry field", type_name());
			}
			return *this;
		}

		inl auto entry(const char *name) -> Cmd& {
			auto n = str_to_var(name);
			return entry(n);
		}

		inl auto type(Db::EntTy ty) -> Cmd& {
			switch (this->ty) {
			CASE(CREATE, create.ty = ty)
			default: throw str_fmt("{} has no type field", type_name());
			}
			return *this;
		}

		/* exe executes the query on the backend. It modifies
		 * the array on disk. for networked access, use send. */
		R<Db::Ent*> exe();

		/* send a command over the wire. */
		R<Db::Ent> send();
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
