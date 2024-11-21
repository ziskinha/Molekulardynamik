#pragma once


//namespace md::debug{
//
//}


#ifndef NDEBUG

#define assert(Expr, Msg) _assert(#Expr, (Expr), __FILE__, __LINE__, (Msg))
#define w_assert(Expr, Msg) _wassert(#Expr, (Expr), __FILE__, __LINE__, (Msg))

void _wassert(const char* expr_str, bool expr, const char* file, int line, const char* msg);
void _assert(const char* expr_str, bool expr, const char* file, int line, const char* msg);

#else

#define assert(Expr, Msg)
#define w_assert(Expr, Msg)

#endif