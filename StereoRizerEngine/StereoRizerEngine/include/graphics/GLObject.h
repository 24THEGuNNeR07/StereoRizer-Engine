#pragma once
#include <iostream>

#define COMMA ,
#define PARENTHESES_OPEN (
#define PARENTHESES_CLOSE )

#define GLOBJ_OVERRIDE(name) \
void release() override;\
void use() override;\
void free() override;\
uint32_t &getLastUsed() override;\
~name() {free();}\
static uint32_t lastUsed;

#define GLOBJ_DEFAULTS(name, bindFunc, freeFunc) \
void name::use()\
{\
	bindFunc(id);\
	lastUsed = id;\
}\
void name::free()\
{\
	freeFunc(id);\
	name::release();\
}\
void name::release()\
{\
	lastUsed = 0;\
	bindFunc(0);\
}

#define GLOBJ_DEFAULTS_MULTI_SPEC(name, bindFunc, bindArg, freeFunc) \
void name::use()\
{\
	bindFunc(bindArg, id);\
	lastUsed = id;\
}\
void name::free()\
{\
	freeFunc(1, &id);\
	name::release();\
}\
void name::release()\
{\
	lastUsed = 0;\
	bindFunc(bindArg, 0);\
}

#define GLOBJ_DEFAULTS_MULTI(name, bindFunc, bindArg, freeFunc) \
void name::use()\
{\
	bindFunc(id);\
	lastUsed = id;\
}\
void name::free()\
{\
	freeFunc(1, &id);\
	name::release();\
}\
void name::release()\
{\
	lastUsed = 0;\
	bindFunc(0);\
}

#define GLOBJ_LAST_USED(x) uint32_t x::lastUsed = 0;\
uint32_t &x::getLastUsed()\
{\
	return x::lastUsed;\
}

#include <iostream>
namespace stereorizer::graphics
{
	class GLObject
	{
	public:
		uint32_t id;

		virtual uint32_t& getLastUsed() = 0;
		virtual void use() = 0;
		virtual void useIfNecessary()
		{
			if (getLastUsed() != id)
				use();
		}
		virtual void release() = 0;
		virtual void free() = 0;
	};
}