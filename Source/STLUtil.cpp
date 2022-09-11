#include "STLUtil.h"


/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
void toByteVector(vector<uint8_t>& vec, const void* buf, size_t size, size_t count)
{
	vec.resize(size*count);
//	memcpy_s(&vec.front(),vec.size(),buf,vec.size());
	::memcpy(vec.data(),buf,vec.size());
}
