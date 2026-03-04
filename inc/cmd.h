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
	};

	extern const char *CmdTy_names[5];

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

	struct Cmd {
		CmdTy ty;
		int sock;
		union {
			Create create;
			Insert insert;
			Get get;
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
			CASE(GET,    get.name = name)
			default: throw str_fmt(
				"{} has no entry field", type_name()
			);
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
			default: throw str_fmt(
				"{} has no type field", type_name()
			);
			}
			return *this;
		}

		inl auto row(S row) -> Cmd& {
			switch (ty) {
			CASE(INSERT, insert.row = row)
			default: throw str_fmt(
				"{} has no row field", type_name()
			);
			}
			return *this;
		}

		inl auto columns(A::A<Q::Q> cols) -> Cmd& {
			switch (ty) {
			CASE(INSERT, insert.cols = cols)
			default: throw str_fmt(
				"{} has no columns field", type_name()
			);
			}
			return *this;
		}

		inl auto columns(
			A::A<std::tuple<const char*, T::TColTy>> cols
		) -> Cmd& {
			switch (ty) {
			case CREATE:
				create.cols = cols.each<Col>([](
					std::tuple<const char*, T::TColTy> *tup
				) {
					auto n = str_to_var(std::get<0>(*tup));
					return std::tuple{n, std::get<1>(*tup)};
				});
				break;
			default: throw str_fmt(
				"{} has no valid columns field", type_name()
			);
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
