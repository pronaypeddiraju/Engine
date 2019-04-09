//------------------------------------------------------------------------------------------------------------------------------
#pragma once

//------------------------------------------------------------------------------------------------------------------------------
class RandomNumberGenerator
{
public:
	RandomNumberGenerator(unsigned int seed = 0);
	~RandomNumberGenerator();

	//Public Methods
	int					GetRandomIntLessThan(int minInt);
	int					GetRandomIntInRange(int minInt, int maxInt);
	float				GetRandomFloatZeroToOne();
	float				GetRandomFloatInRange(float minFloat, float maxFloat);
	bool				PercentChance(float probabilityToReturnTrue);
	void				Seed (unsigned int newSeed);

private:
	//Private Methods
	unsigned int		m_currentSeed = 0;
	unsigned int		m_position = 0;
};
