//------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Array2D<T>::Init(const IntVec2& size, const T& data)
{
	m_size = size;

	m_data.clear();

	int vectorLength = m_size.x * m_size.y;
	for (int index = 0; index < vectorLength; index++)
	{
		m_data.push_back(data);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Array2D<T>::Array2D()
{
	//Empty constructor
}

//------------------------------------------------------------------------------------------------------------------------------
template <typename T>
Array2D<T>::Array2D(const IntVec2& size, const T& data)
{
	Init(size, data);
}

//------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Array2D<T>::Set(const IntVec2& cell, const T& value)
{
	int index = cell.x + (cell.y * m_size.y);
	m_data[index] = value;
}

//------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Array2D<T>::SetAll(const T& value)
{
	int vectorLength = (int)m_data.size();
	for (int index = 0; index < vectorLength; index++)
	{
		m_data[index] = value;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T& Array2D<T>::Get(const IntVec2& cell)
{
	int index = cell.x + (cell.y * m_size.y);
	return m_data[index];
}

//------------------------------------------------------------------------------------------------------------------------------
template <typename T>
T Array2D<T>::TryGet(const IntVec2& cell, const T& defaultValue)
{
	int index = cell.x + (cell.y * m_size.y);

	if ((int)m_data.size() > index)
	{
		return m_data[index];
	}
	else
	{
		return defaultValue;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
template <typename T>
const T& Array2D<T>::Get(const IntVec2& cell) const
{
	int index = cell.x + (cell.y * m_size.y);
	return m_data[index];
}

//------------------------------------------------------------------------------------------------------------------------------
template <typename T>
uint Array2D<T>::GetIndex(const IntVec2& cell) const
{
	uint index = cell.x * cell.y;
	return index;
}

//------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Array2D<T>::SetAtIndex(const uint index, const T& value)
{
	IntVec2 cell;
	cell.x = index % m_size.x;
	cell.y = index / m_size.x;

	Set(cell, value);
}

//------------------------------------------------------------------------------------------------------------------------------
template <typename T>
bool Array2D<T>::ContainsCell(const IntVec2& cell)
{
	int index = cell.x + (cell.y * m_size.y);
	if ((int)m_data.size() > index)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
template <typename T>
IntVec2 Array2D<T>::GetSize()
{
	return m_size;
}

//------------------------------------------------------------------------------------------------------------------------------
template <typename T>
int Array2D<T>::GetHeight()
{
	return m_size.y;
}

//------------------------------------------------------------------------------------------------------------------------------
template <typename T>
int Array2D<T>::GetWidth()
{
	return m_size.x;
}