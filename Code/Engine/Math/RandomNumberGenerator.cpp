//------------------------------------------------------------------------------------------------------------------------------
#include "RandomNumberGenerator.hpp"
#include "Engine/Math/Noise/RawNoise.hpp"
#include "Engine/Math/MathUtils.hpp"

RandomNumberGenerator* g_RNG = nullptr;

//------------------------------------------------------------------------------------------------------------------------------
RandomNumberGenerator::RandomNumberGenerator(unsigned int seed)
	: m_currentSeed(seed)
	, m_position(0)
{

}

//------------------------------------------------------------------------------------------------------------------------------
RandomNumberGenerator::~RandomNumberGenerator()
{

}

//------------------------------------------------------------------------------------------------------------------------------
// Returns an int less than minInt
//------------------------------------------------------------------------------------------------------------------------------
int RandomNumberGenerator::GetRandomIntLessThan( int maxInt )
{
	//int ranInt = rand() % minInt;
	//return ranInt;

	unsigned int randNum = Get1dNoiseUint( m_position, m_currentSeed );
	m_position++;
	return randNum % maxInt;
}

//------------------------------------------------------------------------------------------------------------------------------
// Returns an int in range of minInt and maxInt
//------------------------------------------------------------------------------------------------------------------------------
int RandomNumberGenerator::GetRandomIntInRange( int minInt, int maxInt )
{
	//return minInt + (rand() % (maxInt - minInt + 1));

	unsigned int randNum = Get1dNoiseUint(m_position, m_currentSeed);
	m_position++;
	//get ranfe here

	return minInt + randNum % (maxInt - minInt + 1);

}

//------------------------------------------------------------------------------------------------------------------------------
// Returns a float between 0 and 1
//------------------------------------------------------------------------------------------------------------------------------
float RandomNumberGenerator::GetRandomFloatZeroToOne()
{
	float randNum = Get1dNoiseZeroToOne(m_position, m_currentSeed);
	m_position++;
	return randNum;
}

//------------------------------------------------------------------------------------------------------------------------------
// Returns a float in range of minFloat to maxFloat
//------------------------------------------------------------------------------------------------------------------------------
float RandomNumberGenerator::GetRandomFloatInRange( float minFloat, float maxFloat )
{
	//float ranFloat = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	//return minFloat + ranFloat / (maxFloat - minFloat);

	float randZeroToOne = Get1dNoiseZeroToOne(m_position, m_currentSeed);
	m_position++;
	float randNum = RangeMapFloat(randZeroToOne, 0.0f, 1.0f, minFloat, maxFloat);
	return randNum;
}

//------------------------------------------------------------------------------------------------------------------------------
void RandomNumberGenerator::Seed( unsigned int newSeed )
{
	m_currentSeed = newSeed;
	m_position = 0;
}

