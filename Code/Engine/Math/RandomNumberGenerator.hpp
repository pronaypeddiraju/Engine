//------------------------------------------------------------------------------------------------------------------------------
#pragma once
typedef unsigned int uint;

//------------------------------------------------------------------------------------------------------------------------------
class RandomNumberGenerator
{
public:
	RandomNumberGenerator(unsigned int seed = 0);
	~RandomNumberGenerator();

	//Public Methods
	int					GetRandomIntLessThan(int minInt);
	int					GetRandomIntInRange(int minInt, int maxInt);
	uint				GetCurrentSeed();
	float				GetRandomFloatZeroToOne();
	float				GetRandomFloatInRange(float minFloat, float maxFloat);
	bool				PercentChance(float probabilityToReturnTrue);
	void				Seed (unsigned int newSeed);

private:
	//Private Methods
	uint				m_currentSeed = 0;
	uint				m_position = 0;
};
