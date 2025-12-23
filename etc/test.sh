#!/usr/bin/env bash

for f in o/run/*.com; do
	b="`basename "$f"`"
	R="`$f`"
	E="`cat run/"$b.txt"`"
	if [[ "$R" == "$E" ]]; then
		printf "%s: \e[0;32msuccess\e[0;0m\n" \
			"$f"
	else
		printf "$f:\n\e[0;31m$R\e[0;0m\n"
	fi
done
