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
		GET,
		SET,
	};

	extern const char *CmdTy_names[6];

	using Col = std::tuple<var_t, T::TColTy>;

	/* a create command */
	struct Create {
		var_t name;
		Db::EntTy ty;
		std::optional<A::A<Col>> cols;

		inl Create()
			: name{empty_var()}
			, ty{Db::Int}
			, cols{std::nullopt}
		{};

		~Create() = default;

		inl auto cpy(const Create &x) -> void {
			name = x.name, ty = x.ty, cols = x.cols;
		}

		inl Create(const Create &x) : cols{x.cols} {cpy(x);}
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

	/* get an entry from the database */
	struct Get {
		var_t name;

		inl Get() : name{empty_var()} {}
		~Get() = default;

		inl auto cpy(const Get &x) -> void {
			name = x.name;
		}

		inl Get(const Get &x) {cpy(x);}
		inl Get &operator=(const Get &x) {cpy(x);return *this;}
	};

	struct Set {
		var_t name;
		Q::Q val;

		inl Set() {}
		~Set() = default;

		inl auto cpy(const Set &x) -> void {
			name = x.name, val = x.val;
		}

		inl Set(const Set &x) {cpy(x);}
		inl Set &operator=(const Set &x) {cpy(x);return *this;}
	};

	struct Cmd {
		CmdTy ty;
		int sock;
		union {
			Create create;
			Insert insert;
			Get get;
			Set set;
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

		Cmd &entry(var_t name);
		Cmd &entry(const char *name);
		Cmd &type(Db::EntTy ty);
		Cmd &row(S row);
		Cmd &columns(A::A<Q::Q> cols);
		Cmd &columns(A::A<std::tuple<const char*, T::TColTy>> cols);
		Cmd &value(Q::Q val);

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
