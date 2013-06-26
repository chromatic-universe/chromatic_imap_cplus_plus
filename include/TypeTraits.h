#ifndef __TYPETRAITS_H__
#define __TYPETRAITS_H__

template <typename DDS_STRUCT_T>
struct TypeTraits
{
};


#define DEFINE_TYPETRAITS(TYPE) \
template <> \
struct TypeTraits<TYPE> \
{ \
	typedef TYPE##TypeSupport TypeSupport; \
	 typedef TYPE##TypeSupportImpl TypeSupportImpl; \
	 typedef TYPE##DataWriter Writer; \
	 typedef TYPE##DataReader Reader; \
};


#endif
