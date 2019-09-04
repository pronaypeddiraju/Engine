//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <vector>
typedef unsigned int uint;

template <typename T>
class Array2D
{
public:
	Array2D();
	Array2D(const IntVec2& size, const T& data);

	void			Init(const IntVec2& size, const T& data);
	void			Set(const IntVec2& cell, const T& value);
	void			SetAll(const T& value);
	void		    SetAtIndex(const uint index, const T& value);

	T&				Get(const IntVec2& cell);
	T				TryGet(const IntVec2& cell, const T& defaultValue);
	const T&		Get(const IntVec2& cell) const;

	int				GetWidth();
	int				GetHeight();
	IntVec2			GetSize();
	bool			ContainsCell(const IntVec2& cell);

	inline T&		operator[](const IntVec2& cell)				{ return Get(cell); }
	inline const T& operator[](const IntVec2& cell) const		{ return Get(cell); }

private:
	uint			GetIndex(const IntVec2& cell) const;
private:
	IntVec2			m_size;
	std::vector<T>	m_data;
};

#include "Array2D.inl"

