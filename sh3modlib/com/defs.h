#ifndef COM_DEFS_H
#define COM_DEFS_H

#define COM_CONTEXT_DECL(x) \
	typedef void* x##_Context; \
	x##_Context x##_CreateContext(); \
	void x##_DestroyContext(); \
	void x##_SetContext(x##_Context ctx); \
	x##_Context x##_GetContext();

#define COM_CONTEXT_IMPL(x) \
static _impl_##x##_Context *g_CurrentContext = nullptr; \
x##_Context x##_CreateContext() { \
	if (g_CurrentContext != nullptr) \
		return g_CurrentContext; \
	g_CurrentContext = new _impl_##x##_Context; \
	return (void *)g_CurrentContext; \
} \
void x##_DestroyContext() { \
	delete g_CurrentContext; \
} \
void x##_SetContext(x##_Context ctx) { \
	g_CurrentContext = reinterpret_cast<_impl_##x##_Context*>(ctx); \
} \
x##_Context x##_GetContext() { \
	return reinterpret_cast<void *>(g_CurrentContext); \
}

#define COM_4CHAR_TAG(a, b, c, d) ((a << 24) | (b << 16) | (c << 8) | (d << 0))

#define COM_REGISTER_CRK(x) mem_RegisterTag((crk::x##_tag), "crk::"#x);

#endif