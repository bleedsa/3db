#ifndef __3DB_CMD_H__
#define __3DB_CMD_H__

#include <Q.h>
#include <db.h>
#include <MaybePtr.h>

namespace Cmd {
	enum CmdTy {
		NIL,
		SELECT,
		INSERT,
		CREATE,
		GET,
		SET,
		DEL,
	};

	extern const char *CmdTy_names[6];

	using Col = std::tuple<var_t, T::TColTy>;

	/* a create command */
	struct Create {
		var_t name;
		Db::EntTy ty;
		std::optional<std::vector<Col>> cols;

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
		std::vector<Q::Q> cols;

		inl Insert() : name{empty_var()},row{0},cols{{}} {}
		~Insert() = default;

		inl auto cpy(const Insert &x) -> void {
			name = x.name, row = x.row, cols = x.cols;
		}

		Insert(const Insert &x) : cols{{}} {cpy(x);}
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

	enum WhereTy {
		Eq,
		Lt,
		Gt,
	};

	struct Where {
		WhereTy ty;
		var_t col;
		Q::Q val;

		inl Where(WhereTy ty)
			: ty{ty}, col{empty_var()}, val{Q::Q()}
		{}
		~Where() = default;

		inl auto cpy(const Where &x) -> void {
			ty = x.ty, col = x.col, val = x.val;
		}

		inl Where(const Where &x) : val{Q::Q()} {cpy(x);}
		inl Where &operator=(const Where &x) {cpy(x);return *this;}

		/* set the column name to compare */
		Where &name(const char *n);

		/* set the value to compare against */
		Where &value(Q::Q q);

		template<typename X>
		inl auto value(X x) -> Where& {
			return value(Q::Q(x));
		}

		bool operator()(T::T *t, S y);
	};

	struct Select {
		var_t name;
		A::A<var_t> cols;
		std::optional<Where> where;

		inl Select() : cols{A::A<var_t>(0)}, where{std::nullopt} {}
		~Select() = default;

		inl auto cpy(const Select &x) -> void {
			name = x.name, cols = x.cols, where = x.where;
		}

		inl Select(const Select &x)
			: cols{A::A<var_t>(0)}, where{std::nullopt}
		{cpy(x);}
		inl Select &operator=(const Select &x) {cpy(x);return *this;}
	};

	struct Del {
		var_t name;
		Where where;

		inl Del() : name{empty_var()}, where{Where(Eq)} {}
		~Del() = default;

		inl auto cpy(const Del &x) -> void {
			name = x.name, where = x.where;
		}

		inl Del(const Del &x) : where{Where(Eq)} {cpy(x);}
		inl Del &operator=(const Del &x) {cpy(x);return *this;}
	};

	struct Cmd {
		CmdTy ty;
		int sock;
		union {
			Select select;
			Create create;
			Insert insert;
			Get get;
			Set set;
			Del del;
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

		/* set the entry targeted by this command */
		Cmd &entry(var_t name);
		Cmd &entry(const char *name);

		/* set the "columns" field for this command */
		Cmd &columns(std::vector<Q::Q> cols);
		Cmd &columns(std::vector<std::tuple<const char*, T::TColTy>> cols);
		Cmd &columns(A::A<var_t> cols);
		Cmd &columns(A::A<const char*> cols);

		Cmd &value(Q::Q val);
		Cmd &type(Db::EntTy ty);
		Cmd &row(S row);
		Cmd &where(Where where);

		/* exe executes the query on the backend. It modifies
		 * the array on disk. for networked access, use send. */
		R<MaybePtr<Db::Ent>> exe();

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
