#ifndef __3DB_MEM_H__
#define __3DB_MEM_H__

#include <u.h>

/* make a buffer of X with length L */
template<typename X>
inl auto mk(S L) -> X* {
	std::cout << Z(X)*L << std::endl;
	auto P = malloc(Z(X)*L);
	if (!P) {
		/* NOTE: don't use fatal() for compile time reasons */
		std::cerr << "malloc() returned null" << std::endl;
		exit(-1);
	}
	return (X*)P;
}

/* realloc buffer P with length L */
template<typename X>
inl auto remk(X *P, S L) -> X* {
	P = (X*)realloc((void*)P, Z(X)*L);
	if (!P) {
		std::cerr << "realloc() returned null" << std::endl;
		exit(-1);
	}
	return P;
}

#endif
