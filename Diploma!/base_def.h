#ifndef SRC_BASEDEF_H
#define SRC_BASEDEF_H

typedef unsigned int u_int;
typedef unsigned long u_long;

#if __cplusplus >= 201103L
#define NOEXCEPT_AVAL noexcept
#define CONSTEXPR_AVAL constexpr
#endif

#endif