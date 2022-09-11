#pragma once
#include "Types.h"


/**********************************************************************************************\
*                                                                                              *
*  Order operator for pair<> with map, set, vector, sort, ...                                  *
*                                                                                              *
*  General structure:                                                                          *
*                                                                                              *
*  template<class T> bool functionName(const T& a, const T& b) { return a.??? < b.???; }       *
*                                                                                              *
*  or                                                                                          *
*                                                                                              *
*  template<class T> struct funcObjectName                                                     *
*                    {                                                                         *
*                        bool operator()(const T& a, const T& b) { return a.??? < b.???; }     *
*                    };                                                                        *
*                                                                                              *
* Ex.: sort(vec.begin(), vec.end(), sortPairSecondOperator<int,double>);                       *
* Ex.: sort(vec.begin(), vec.end(), sortPairSecondFunctor<int,double>()); <- 1.5-2.5x FASTER ! *
*                                                                                              *
\**********************************************************************************************/
template<class FirstType, class SecondType>
inline bool sortPairFirstOperator(const pair<FirstType,SecondType>& a, const pair<FirstType,SecondType>& b)
{
	return a.first < b.first;
}

template<class FirstType, class SecondType>
inline bool sortPairSecondOperator(const pair<FirstType,SecondType>& a, const pair<FirstType,SecondType>& b)
{
	return a.second < b.second;
}

template<class FirstType, class SecondType>
struct sortPairFirstFunctor
{
	inline bool operator()(const pair<FirstType,SecondType>& a, const pair<FirstType,SecondType>& b) const
	{
		return a.first < b.first;
	}
};

template<class FirstType, class SecondType>
struct sortPairSecondFunctor
{
	inline bool operator()(const pair<FirstType,SecondType>& a, const pair<FirstType,SecondType>& b) const
	{
		return a.second < b.second;
	}
};

#define findInVector(Val,Vec)	(std::find((Vec).cbegin(),(Vec).cend(),(Val)) != (Vec).cend())
#define findInMap(Val,Map)		((Map).find(Val) != (Map).cend())

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
template<class T>
void forceClearVector(vector<T>& vec)	// Force a vector to really release memory
{
#if __cplusplus > 199711L
	vec.clear();
	vec.shrink_to_fit();
#else
	vector<T>().swap(vec); // swap content to a temp vector
#endif
}

/**************************************************************************\
*                                                                          *
*  Inverse la map<key,val> en map<val,key>.                                *
*  Attention, fonctionne pour une map ayant des valeurs uniques !!!        *
*                                                                          *
\**************************************************************************/
template<class K, class V>
void inverseMap(const map<K,V>& mapIn, map<V,K>& mapOut)
{
	// Invert the map
	mapOut.clear();
	for (auto it=mapIn.cbegin(),end=mapIn.cend(); it!=end; ++it)
	{
		mapOut[it->second] = it->first;
	}
}


void toByteVector(vector<uint8_t>& vec, const void* buf, size_t size, size_t count);
