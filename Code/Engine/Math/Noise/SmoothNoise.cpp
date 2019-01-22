//-----------------------------------------------------------------------------------------------
// SmoothNoise.cpp
//
#include "Engine/Math/Noise/SmoothNoise.hpp"
#include "Engine/Math/Noise/RawNoise.hpp"			// for raw bit-noise base functions (SquirrelNoise4)
#include "Engine/Math/MathUtils.hpp"	// for SmoothStep3(); see "SmoothStep" on Wikipedia
#include "Engine/Math/Vec2.hpp"			// for Vec2( float x,y ) class/struct
#include "Engine/Math/Vec3.hpp"			// for Vec3( float x,y,z ) class/struct
#include "Engine/Math/Vec4.hpp"			// for Vec4( float x,y,z,w ) class/struct

/////////////////////////////////////////////////////////////////////////////////////////////////
// For all fractal (and Perlin) noise functions, the following internal naming conventions
//	are used, primarily to help me visualize 3D and 4D constructs clearly.  They need not
//	have any actual bearing on / relationship to actual external coordinate systems.
//
// 1D noise: only X (+east / -west)
// 2D noise: also Y (+north / -south)
// 3D noise: also Z (+above / -below)
// 4D noise: also T (+after / -before)
/////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------------------------
float Compute1dFractalNoise( float position, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float currentPosition = position * (1.f / scale);

	for( unsigned int octaveNum = 0; octaveNum < numOctaves; ++ octaveNum )
	{
		// Determine noise values at nearby integer "grid point" positions
		float positionFloor = floorf( currentPosition );
		int indexWest = (int) positionFloor;
		int indexEast = indexWest + 1;
		float valueWest = Get1dNoiseZeroToOne( indexWest, seed );
		float valueEast = Get1dNoiseZeroToOne( indexEast, seed );

		// Do a smoothed (nonlinear) weighted average of nearby grid point values
		float distanceFromWest = currentPosition - positionFloor;
		float weightEast = SmoothStep3( distanceFromWest ); // Gives rounder (nonlinear) results
		float weightWest = 1.f - weightEast;
		float noiseZeroToOne = (valueWest * weightWest) + (valueEast * weightEast);
		float noiseThisOctave = 2.f * (noiseZeroToOne - 0.5f); // Map from [0,1] to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPosition *= octaveScale;
		currentPosition += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used!
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to [0,1]
		totalNoise = SmoothStep3( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
float Compute2dFractalNoise( float posX, float posY, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = (1.f / scale);
	Vec2 currentPos( posX * invScale, posY * invScale );

	for( unsigned int octaveNum = 0; octaveNum < numOctaves; ++ octaveNum )
	{
		// Determine noise values at nearby integer "grid point" positions
		Vec2 cellMins( floorf( currentPos.x ), floorf( currentPos.y ) );
		int indexWestX = (int) cellMins.x;
		int indexSouthY = (int) cellMins.y;
		int indexEastX = indexWestX + 1;
		int indexNorthY = indexSouthY + 1;
		float valueSouthWest = Get2dNoiseZeroToOne( indexWestX, indexSouthY, seed );
		float valueSouthEast = Get2dNoiseZeroToOne( indexEastX, indexSouthY, seed );
		float valueNorthWest = Get2dNoiseZeroToOne( indexWestX, indexNorthY, seed );
		float valueNorthEast = Get2dNoiseZeroToOne( indexEastX, indexNorthY, seed );

		// Do a smoothed (nonlinear) weighted average of nearby grid point values
		Vec2 displacementFromMins = currentPos - cellMins;
		float weightEast  = SmoothStep3( displacementFromMins.x );
		float weightNorth = SmoothStep3( displacementFromMins.y );
		float weightWest  = 1.f - weightEast;
		float weightSouth = 1.f - weightNorth;

		float blendSouth = (weightEast * valueSouthEast) + (weightWest * valueSouthWest);
		float blendNorth = (weightEast * valueNorthEast) + (weightWest * valueNorthWest);
		float blendTotal = (weightSouth * blendSouth) + (weightNorth * blendNorth);
		float noiseThisOctave = 2.f * (blendTotal - 0.5f); // Map from [0,1] to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offsets to noise position components
		currentPos.y += OCTAVE_OFFSET; //	at each octave to break up their grid alignment
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to [0,1]
		totalNoise = SmoothStep3( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
float Compute3dFractalNoise( float posX, float posY, float posZ, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = (1.f / scale);
	Vec3 currentPos( posX * invScale, posY * invScale, posZ * invScale );

	for( unsigned int octaveNum = 0; octaveNum < numOctaves; ++ octaveNum )
	{
		// Determine noise values at nearby integer "grid point" positions
		Vec3 cellMins( floorf( currentPos.x ), floorf( currentPos.y ), floorf( currentPos.z ) );
		int indexWestX  = (int) cellMins.x;
		int indexSouthY = (int) cellMins.y;
		int indexBelowZ = (int) cellMins.z;
		int indexEastX  = indexWestX + 1;
		int indexNorthY = indexSouthY + 1;
		int indexAboveZ = indexBelowZ + 1;

		// Noise grid cell has 8 corners in 3D
		float aboveSouthWest = Get3dNoiseZeroToOne( indexWestX, indexSouthY, indexAboveZ, seed );
		float aboveSouthEast = Get3dNoiseZeroToOne( indexEastX, indexSouthY, indexAboveZ, seed );
		float aboveNorthWest = Get3dNoiseZeroToOne( indexWestX, indexNorthY, indexAboveZ, seed );
		float aboveNorthEast = Get3dNoiseZeroToOne( indexEastX, indexNorthY, indexAboveZ, seed );
		float belowSouthWest = Get3dNoiseZeroToOne( indexWestX, indexSouthY, indexBelowZ, seed );
		float belowSouthEast = Get3dNoiseZeroToOne( indexEastX, indexSouthY, indexBelowZ, seed );
		float belowNorthWest = Get3dNoiseZeroToOne( indexWestX, indexNorthY, indexBelowZ, seed );
		float belowNorthEast = Get3dNoiseZeroToOne( indexEastX, indexNorthY, indexBelowZ, seed );

		// Do a smoothed (nonlinear) weighted average of nearby grid point values
		Vec3 displacementFromMins = currentPos - cellMins;

		float weightEast  = SmoothStep3( displacementFromMins.x );
		float weightNorth = SmoothStep3( displacementFromMins.y );
		float weightAbove = SmoothStep3( displacementFromMins.z );
		float weightWest  = 1.f - weightEast;
		float weightSouth = 1.f - weightNorth;
		float weightBelow = 1.f - weightAbove;

		// 8-way blend (8 -> 4 -> 2 -> 1)
		float blendBelowSouth = (weightEast * belowSouthEast) + (weightWest * belowSouthWest);
		float blendBelowNorth = (weightEast * belowNorthEast) + (weightWest * belowNorthWest);
		float blendAboveSouth = (weightEast * aboveSouthEast) + (weightWest * aboveSouthWest);
		float blendAboveNorth = (weightEast * aboveNorthEast) + (weightWest * aboveNorthWest);
		float blendBelow = (weightSouth * blendBelowSouth) + (weightNorth * blendBelowNorth);
		float blendAbove = (weightSouth * blendAboveSouth) + (weightNorth * blendAboveNorth);
		float blendTotal = (weightBelow * blendBelow) + (weightAbove * blendAbove);
		float noiseThisOctave = 2.f * (blendTotal - 0.5f); // Map from [0,1] to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offsets to noise position components
		currentPos.y += OCTAVE_OFFSET; //	at each octave to break up their grid alignment
		currentPos.z += OCTAVE_OFFSET;
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to [0,1]
		totalNoise = SmoothStep3( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
float Compute4dFractalNoise( float posX, float posY, float posZ, float posT, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = (1.f / scale);
	Vec4 currentPos( posX * invScale, posY * invScale, posZ * invScale, posT * invScale );

	for( unsigned int octaveNum = 0; octaveNum < numOctaves; ++ octaveNum )
	{
		// Determine noise values at nearby integer "grid point" positions
		Vec4 cellMins( floorf( currentPos.x ), floorf( currentPos.y ), floorf( currentPos.z ), floorf( currentPos.w ) );
		int indexWestX   = (int) cellMins.x;
		int indexSouthY  = (int) cellMins.y;
		int indexBelowZ  = (int) cellMins.z;
		int indexBeforeT = (int) cellMins.w;
		int indexEastX  = indexWestX + 1;
		int indexNorthY = indexSouthY + 1;
		int indexAboveZ = indexBelowZ + 1;
		int indexAfterT = indexBeforeT + 1;

		// Noise grid cell has 16 "corners" in 4D
		float beforeBelowSW = Get4dNoiseZeroToOne( indexWestX, indexSouthY, indexBelowZ, indexBeforeT, seed );
		float beforeBelowSE = Get4dNoiseZeroToOne( indexEastX, indexSouthY, indexBelowZ, indexBeforeT, seed );
		float beforeBelowNW = Get4dNoiseZeroToOne( indexWestX, indexNorthY, indexBelowZ, indexBeforeT, seed );
		float beforeBelowNE = Get4dNoiseZeroToOne( indexEastX, indexNorthY, indexBelowZ, indexBeforeT, seed );
		float beforeAboveSW = Get4dNoiseZeroToOne( indexWestX, indexSouthY, indexAboveZ, indexBeforeT, seed );
		float beforeAboveSE = Get4dNoiseZeroToOne( indexEastX, indexSouthY, indexAboveZ, indexBeforeT, seed );
		float beforeAboveNW = Get4dNoiseZeroToOne( indexWestX, indexNorthY, indexAboveZ, indexBeforeT, seed );
		float beforeAboveNE = Get4dNoiseZeroToOne( indexEastX, indexNorthY, indexAboveZ, indexBeforeT, seed );

		float afterBelowSW = Get4dNoiseZeroToOne( indexWestX, indexSouthY, indexBelowZ, indexAfterT, seed );
		float afterBelowSE = Get4dNoiseZeroToOne( indexEastX, indexSouthY, indexBelowZ, indexAfterT, seed );
		float afterBelowNW = Get4dNoiseZeroToOne( indexWestX, indexNorthY, indexBelowZ, indexAfterT, seed );
		float afterBelowNE = Get4dNoiseZeroToOne( indexEastX, indexNorthY, indexBelowZ, indexAfterT, seed );
		float afterAboveSW = Get4dNoiseZeroToOne( indexWestX, indexSouthY, indexAboveZ, indexAfterT, seed );
		float afterAboveSE = Get4dNoiseZeroToOne( indexEastX, indexSouthY, indexAboveZ, indexAfterT, seed );
		float afterAboveNW = Get4dNoiseZeroToOne( indexWestX, indexNorthY, indexAboveZ, indexAfterT, seed );
		float afterAboveNE = Get4dNoiseZeroToOne( indexEastX, indexNorthY, indexAboveZ, indexAfterT, seed );

		// Do a smoothed (nonlinear) weighted average of nearby grid point values
		Vec4 displacementFromMins = currentPos - cellMins;

		float weightEast	= SmoothStep3( displacementFromMins.x );
		float weightNorth	= SmoothStep3( displacementFromMins.y );
		float weightAbove	= SmoothStep3( displacementFromMins.z );
		float weightAfter	= SmoothStep3( displacementFromMins.w );
		float weightWest	= 1.f - weightEast;
		float weightSouth	= 1.f - weightNorth;
		float weightBelow	= 1.f - weightAbove;
		float weightBefore	= 1.f - weightAfter;

		// 16-way blend (16 -> 8 -> 4 -> 2 -> 1)
		float blendBeforeBelowSouth	= (weightEast * beforeBelowSE) + (weightWest * beforeBelowSW);
		float blendBeforeBelowNorth	= (weightEast * beforeBelowNE) + (weightWest * beforeBelowNW);
		float blendBeforeAboveSouth	= (weightEast * beforeAboveSE) + (weightWest * beforeAboveSW);
		float blendBeforeAboveNorth	= (weightEast * beforeAboveNE) + (weightWest * beforeAboveNW);
		float blendAfterBelowSouth  = (weightEast * afterBelowSE)  + (weightWest * afterBelowSW);
		float blendAfterBelowNorth  = (weightEast * afterBelowNE)  + (weightWest * afterBelowNW);
		float blendAfterAboveSouth  = (weightEast * afterAboveSE)  + (weightWest * afterAboveSW);
		float blendAfterAboveNorth  = (weightEast * afterAboveNE)  + (weightWest * afterAboveNW);
		float blendBeforeBelow = (weightSouth * blendBeforeBelowSouth) + (weightNorth * blendBeforeBelowNorth);
		float blendBeforeAbove = (weightSouth * blendBeforeAboveSouth) + (weightNorth * blendBeforeAboveNorth);
		float blendAfterBelow  = (weightSouth * blendAfterBelowSouth)  + (weightNorth * blendAfterBelowNorth);
		float blendAfterAbove  = (weightSouth * blendAfterAboveSouth)  + (weightNorth * blendAfterAboveNorth);
		float blendBefore = (weightBelow * blendBeforeBelow) + (weightAbove * blendBeforeAbove);
		float blendAfter  = (weightBelow * blendAfterBelow) + (weightAbove * blendAfterAbove);
		float blendTotal = (weightBefore * blendBefore) + (weightAfter * blendAfter);
		float noiseThisOctave = 2.f * (blendTotal - 0.5f); // Map from [0,1] to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offsets to noise position components
		currentPos.y += OCTAVE_OFFSET; //	at each octave to break up their grid alignment
		currentPos.z += OCTAVE_OFFSET;
		currentPos.w += OCTAVE_OFFSET;
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to [0,1]
		totalNoise = SmoothStep3( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
// Perlin noise is fractal noise with "gradient vector smoothing" applied.
//
// In 1D, the gradients are trivial: -1.0 or 1.0, so resulting noise is boring at one octave.
//
float Compute1dPerlinNoise( float position, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave
	const float gradients[2] = { -1.f, 1.f }; // 1D unit "gradient" vectors; one back, one forward

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float currentPosition = position * (1.f / scale);

	for( unsigned int octaveNum = 0; octaveNum < numOctaves; ++ octaveNum )
	{
		// Determine random "gradient vectors" (just +1 or -1 for 1D Perlin) for surrounding corners
		float positionFloor = (float) floorf( currentPosition );
		int indexWest = (int) positionFloor;
		int indexEast = indexWest + 1;
		float gradientWest = gradients[ Get1dNoiseUint( indexWest, seed ) & 0x00000001 ];
		float gradientEast = gradients[ Get1dNoiseUint( indexEast, seed ) & 0x00000001 ];

		// Dot each point's gradient with displacement from point to position
		float displacementFromWest = currentPosition - positionFloor; // always positive
		float displacementFromEast = displacementFromWest - 1.f; // always negative
		float dotWest = gradientWest * displacementFromWest; // 1D "dot product" is... multiply
		float dotEast = gradientEast * displacementFromEast;

		// Do a smoothed (nonlinear) weighted average of dot results
		float weightEast = SmoothStep3( displacementFromWest );
		float weightWest = 1.f - weightEast;
		float blendTotal = (weightWest * dotWest) + (weightEast * dotEast);
		float noiseThisOctave = 2.f * blendTotal; // 1D Perlin is in [-.5,.5]; map to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPosition *= octaveScale;
		currentPosition += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to [0,1]
		totalNoise = SmoothStep3( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
// Perlin noise is fractal noise with "gradient vector smoothing" applied.
//
// In 2D, gradients are unit-length vectors in various directions with even angular distribution.
//
float Compute2dPerlinNoise( float posX, float posY, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave
	const Vec2 gradients[ 8 ] = // Normalized unit vectors in 8 quarter-cardinal directions
	{
		Vec2( +0.923879533f, +0.382683432f ), //  22.5 degrees (ENE)
		Vec2( +0.382683432f, +0.923879533f ), //  67.5 degrees (NNE)
		Vec2( -0.382683432f, +0.923879533f ), // 112.5 degrees (NNW)
		Vec2( -0.923879533f, +0.382683432f ), // 157.5 degrees (WNW)
		Vec2( -0.923879533f, -0.382683432f ), // 202.5 degrees (WSW)
		Vec2( -0.382683432f, -0.923879533f ), // 247.5 degrees (SSW)
		Vec2( +0.382683432f, -0.923879533f ), // 292.5 degrees (SSE)
		Vec2( +0.923879533f, -0.382683432f )	 // 337.5 degrees (ESE)
	};

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = (1.f / scale);
	Vec2 currentPos( posX * invScale, posY * invScale );

	for( unsigned int octaveNum = 0; octaveNum < numOctaves; ++ octaveNum )
	{
		// Determine random unit "gradient vectors" for surrounding corners
		Vec2 cellMins( floorf( currentPos.x ), floorf( currentPos.y ) );
		Vec2 cellMaxs( cellMins.x + 1.f, cellMins.y + 1.f );
		int indexWestX  = (int) cellMins.x;
		int indexSouthY = (int) cellMins.y;
		int indexEastX  = indexWestX  + 1;
		int indexNorthY = indexSouthY + 1;

		unsigned int noiseSW = Get2dNoiseUint( indexWestX, indexSouthY, seed );
		unsigned int noiseSE = Get2dNoiseUint( indexEastX, indexSouthY, seed );
		unsigned int noiseNW = Get2dNoiseUint( indexWestX, indexNorthY, seed );
		unsigned int noiseNE = Get2dNoiseUint( indexEastX, indexNorthY, seed );

		const Vec2& gradientSW = gradients[ noiseSW & 0x00000007 ];
		const Vec2& gradientSE = gradients[ noiseSE & 0x00000007 ];
		const Vec2& gradientNW = gradients[ noiseNW & 0x00000007 ];
		const Vec2& gradientNE = gradients[ noiseNE & 0x00000007 ];

		// Dot each corner's gradient with displacement from corner to position
		Vec2 displacementFromSW( currentPos.x - cellMins.x, currentPos.y - cellMins.y );
		Vec2 displacementFromSE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y );
		Vec2 displacementFromNW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y );
		Vec2 displacementFromNE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y );

		float dotSouthWest = GetDotProduct( gradientSW, displacementFromSW );
		float dotSouthEast = GetDotProduct( gradientSE, displacementFromSE );
		float dotNorthWest = GetDotProduct( gradientNW, displacementFromNW );
		float dotNorthEast = GetDotProduct( gradientNE, displacementFromNE );

		// Do a smoothed (nonlinear) weighted average of dot results
		float weightEast = SmoothStep3( displacementFromSW.x );
		float weightNorth = SmoothStep3( displacementFromSW.y );
		float weightWest = 1.f - weightEast;
		float weightSouth = 1.f - weightNorth;

		float blendSouth = (weightEast * dotSouthEast) + (weightWest * dotSouthWest);
		float blendNorth = (weightEast * dotNorthEast) + (weightWest * dotNorthWest);
		float blendTotal = (weightSouth * blendSouth) + (weightNorth * blendNorth);
		float noiseThisOctave = blendTotal * (1.f / 0.662578106f); // 2D Perlin is in [-.662578106,.662578106]; map to ~[-1,1]
		
		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.y += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to [0,1]
		totalNoise = SmoothStep3( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
// Perlin noise is fractal noise with "gradient vector smoothing" applied.
//
// In 3D, gradients are unit-length vectors in random (3D) directions.
//
float Compute3dPerlinNoise( float posX, float posY, float posZ, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	const Vec3 gradients[ 8 ] = // Traditional "12 edges" requires modulus and isn't any better.
	{
		Vec3( +fSQRT_3_OVER_3, +fSQRT_3_OVER_3, +fSQRT_3_OVER_3 ), // Normalized unit 3D vectors
		Vec3( -fSQRT_3_OVER_3, +fSQRT_3_OVER_3, +fSQRT_3_OVER_3 ), //  pointing toward cube
		Vec3( +fSQRT_3_OVER_3, -fSQRT_3_OVER_3, +fSQRT_3_OVER_3 ), //  corners, so components
		Vec3( -fSQRT_3_OVER_3, -fSQRT_3_OVER_3, +fSQRT_3_OVER_3 ), //  are all sqrt(3)/3, i.e.
		Vec3( +fSQRT_3_OVER_3, +fSQRT_3_OVER_3, -fSQRT_3_OVER_3 ), // 0.5773502691896257645091f.
		Vec3( -fSQRT_3_OVER_3, +fSQRT_3_OVER_3, -fSQRT_3_OVER_3 ), // These are slightly better
		Vec3( +fSQRT_3_OVER_3, -fSQRT_3_OVER_3, -fSQRT_3_OVER_3 ), // than axes (1,0,0) and much
		Vec3( -fSQRT_3_OVER_3, -fSQRT_3_OVER_3, -fSQRT_3_OVER_3 )  // faster than edges (1,1,0).
	};

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = (1.f / scale);
	Vec3 currentPos( posX * invScale, posY * invScale, posZ * invScale );

	for( unsigned int octaveNum = 0; octaveNum < numOctaves; ++ octaveNum )
	{
		// Determine random unit "gradient vectors" for surrounding corners
		Vec3 cellMins( floorf( currentPos.x ), floorf( currentPos.y ), floorf( currentPos.z ) );
		Vec3 cellMaxs( cellMins.x + 1.f, cellMins.y + 1.f, cellMins.z + 1.f );
		int indexWestX  = (int) cellMins.x;
		int indexSouthY = (int) cellMins.y;
		int indexBelowZ = (int) cellMins.z;
		int indexEastX  = indexWestX  + 1;
		int indexNorthY = indexSouthY + 1;
		int indexAboveZ = indexBelowZ + 1;

		unsigned int noiseBelowSW = Get3dNoiseUint( indexWestX, indexSouthY, indexBelowZ, seed );
		unsigned int noiseBelowSE = Get3dNoiseUint( indexEastX, indexSouthY, indexBelowZ, seed );
		unsigned int noiseBelowNW = Get3dNoiseUint( indexWestX, indexNorthY, indexBelowZ, seed );
		unsigned int noiseBelowNE = Get3dNoiseUint( indexEastX, indexNorthY, indexBelowZ, seed );
		unsigned int noiseAboveSW = Get3dNoiseUint( indexWestX, indexSouthY, indexAboveZ, seed );
		unsigned int noiseAboveSE = Get3dNoiseUint( indexEastX, indexSouthY, indexAboveZ, seed );
		unsigned int noiseAboveNW = Get3dNoiseUint( indexWestX, indexNorthY, indexAboveZ, seed );
		unsigned int noiseAboveNE = Get3dNoiseUint( indexEastX, indexNorthY, indexAboveZ, seed );

		Vec3 gradientBelowSW = gradients[ noiseBelowSW & 0x00000007 ];
		Vec3 gradientBelowSE = gradients[ noiseBelowSE & 0x00000007 ];
		Vec3 gradientBelowNW = gradients[ noiseBelowNW & 0x00000007 ];
		Vec3 gradientBelowNE = gradients[ noiseBelowNE & 0x00000007 ];
		Vec3 gradientAboveSW = gradients[ noiseAboveSW & 0x00000007 ];
		Vec3 gradientAboveSE = gradients[ noiseAboveSE & 0x00000007 ];
		Vec3 gradientAboveNW = gradients[ noiseAboveNW & 0x00000007 ];
		Vec3 gradientAboveNE = gradients[ noiseAboveNE & 0x00000007 ];

		// Dot each corner's gradient with displacement from corner to position
		Vec3 displacementFromBelowSW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z );
		Vec3 displacementFromBelowSE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z );
		Vec3 displacementFromBelowNW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z );
		Vec3 displacementFromBelowNE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z );
		Vec3 displacementFromAboveSW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z );
		Vec3 displacementFromAboveSE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z );
		Vec3 displacementFromAboveNW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z );
		Vec3 displacementFromAboveNE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z );

		float dotBelowSW = GetDotProduct( gradientBelowSW, displacementFromBelowSW );
		float dotBelowSE = GetDotProduct( gradientBelowSE, displacementFromBelowSE );
		float dotBelowNW = GetDotProduct( gradientBelowNW, displacementFromBelowNW );
		float dotBelowNE = GetDotProduct( gradientBelowNE, displacementFromBelowNE );
		float dotAboveSW = GetDotProduct( gradientAboveSW, displacementFromAboveSW );
		float dotAboveSE = GetDotProduct( gradientAboveSE, displacementFromAboveSE );
		float dotAboveNW = GetDotProduct( gradientAboveNW, displacementFromAboveNW );
		float dotAboveNE = GetDotProduct( gradientAboveNE, displacementFromAboveNE );

		// Do a smoothed (nonlinear) weighted average of dot results
		float weightEast  = SmoothStep3( displacementFromBelowSW.x );
		float weightNorth = SmoothStep3( displacementFromBelowSW.y );
		float weightAbove = SmoothStep3( displacementFromBelowSW.z );
		float weightWest  = 1.f - weightEast;
		float weightSouth = 1.f - weightNorth;
		float weightBelow = 1.f - weightAbove;

		// 8-way blend (8 -> 4 -> 2 -> 1)
		float blendBelowSouth = (weightEast * dotBelowSE) + (weightWest * dotBelowSW);
		float blendBelowNorth = (weightEast * dotBelowNE) + (weightWest * dotBelowNW);
		float blendAboveSouth = (weightEast * dotAboveSE) + (weightWest * dotAboveSW);
		float blendAboveNorth = (weightEast * dotAboveNE) + (weightWest * dotAboveNW);
		float blendBelow = (weightSouth * blendBelowSouth) + (weightNorth * blendBelowNorth);
		float blendAbove = (weightSouth * blendAboveSouth) + (weightNorth * blendAboveNorth);
		float blendTotal = (weightBelow * blendBelow) + (weightAbove * blendAbove);
		float noiseThisOctave = blendTotal * (1.f / 0.793856621f); // 3D Perlin is in [-.793856621,.793856621]; map to ~[-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.y += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.z += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to [0,1]
		totalNoise = SmoothStep3( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
// Perlin noise is fractal noise with "gradient vector smoothing" applied.
//
// In 4D, gradients are unit-length hyper-vectors in random (4D) directions.
//
float Compute4dPerlinNoise( float posX, float posY, float posZ, float posT, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	const Vec4 gradients[ 16 ] = // Hard to tell if this is any better in 4D than just having 8
	{
		Vec4( +0.5f, +0.5f, +0.5f, +0.5f ), // Normalized unit 4D vectors pointing toward each
		Vec4( -0.5f, +0.5f, +0.5f, +0.5f ), //  of the 16 hypercube corners, so components are
		Vec4( +0.5f, -0.5f, +0.5f, +0.5f ), //  all sqrt(4)/4, i.e. one-half.
		Vec4( -0.5f, -0.5f, +0.5f, +0.5f ), // 
		Vec4( +0.5f, +0.5f, -0.5f, +0.5f ), // It's hard to tell whether these are any better
		Vec4( -0.5f, +0.5f, -0.5f, +0.5f ), //  or worse than vectors facing axes (1,0,0,0) or
		Vec4( +0.5f, -0.5f, -0.5f, +0.5f ), //  3D edges (.7,.7,0,0) or 4D edges (.57,.57,.57,0)
		Vec4( -0.5f, -0.5f, -0.5f, +0.5f ), //  but less-axial gradients looked a little better
		Vec4( +0.5f, +0.5f, +0.5f, -0.5f ), //  with 2D and 3D noise so I'm assuming this is as
		Vec4( -0.5f, +0.5f, +0.5f, -0.5f ), //  good or better as any other gradient-selection
		Vec4( +0.5f, -0.5f, +0.5f, -0.5f ), //  scheme (and is crazy-fast).  *shrug*
		Vec4( -0.5f, -0.5f, +0.5f, -0.5f ), // 
		Vec4( +0.5f, +0.5f, -0.5f, -0.5f ), // Plus, we want a power-of-two number of evenly-
		Vec4( -0.5f, +0.5f, -0.5f, -0.5f ), //  distributed gradients, so we can cheaply select
		Vec4( +0.5f, -0.5f, -0.5f, -0.5f ), //  one from bit-noise (use bit-mask, not modulus).
		Vec4( -0.5f, -0.5f, -0.5f, -0.5f )  // 
	};

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = (1.f / scale);
	Vec4 currentPos( posX * invScale, posY * invScale, posZ * invScale, posT * invScale );

	for( unsigned int octaveNum = 0; octaveNum < numOctaves; ++ octaveNum )
	{
		// Determine random unit "gradient vectors" for 16 surrounding 4D (hypercube) cell corners
		Vec4 cellMins( floorf( currentPos.x ), floorf( currentPos.y ), floorf( currentPos.z ), floorf( currentPos.w ) );
		Vec4 cellMaxs( cellMins.x + 1.f, cellMins.y + 1.f, cellMins.z + 1.f, cellMins.w + 1.f );
		int indexWestX   = (int) cellMins.x;
		int indexSouthY  = (int) cellMins.y;
		int indexBelowZ  = (int) cellMins.z;
		int indexBeforeT = (int) cellMins.w;
		int indexEastX  = indexWestX   + 1;
		int indexNorthY = indexSouthY  + 1;
		int indexAboveZ = indexBelowZ  + 1;
		int indexAfterT = indexBeforeT + 1;

		// "BeforeBSW" stands for "BeforeBelowSouthWest" below (i.e. 4D hypercube mins), etc.
		unsigned int noiseBeforeBSW = Get4dNoiseUint( indexWestX, indexSouthY, indexBelowZ, indexBeforeT, seed );
		unsigned int noiseBeforeBSE = Get4dNoiseUint( indexEastX, indexSouthY, indexBelowZ, indexBeforeT, seed );
		unsigned int noiseBeforeBNW = Get4dNoiseUint( indexWestX, indexNorthY, indexBelowZ, indexBeforeT, seed );
		unsigned int noiseBeforeBNE = Get4dNoiseUint( indexEastX, indexNorthY, indexBelowZ, indexBeforeT, seed );
		unsigned int noiseBeforeASW = Get4dNoiseUint( indexWestX, indexSouthY, indexAboveZ, indexBeforeT, seed );
		unsigned int noiseBeforeASE = Get4dNoiseUint( indexEastX, indexSouthY, indexAboveZ, indexBeforeT, seed );
		unsigned int noiseBeforeANW = Get4dNoiseUint( indexWestX, indexNorthY, indexAboveZ, indexBeforeT, seed );
		unsigned int noiseBeforeANE = Get4dNoiseUint( indexEastX, indexNorthY, indexAboveZ, indexBeforeT, seed );
		unsigned int noiseAfterBSW = Get4dNoiseUint( indexWestX, indexSouthY, indexBelowZ, indexAfterT, seed );
		unsigned int noiseAfterBSE = Get4dNoiseUint( indexEastX, indexSouthY, indexBelowZ, indexAfterT, seed );
		unsigned int noiseAfterBNW = Get4dNoiseUint( indexWestX, indexNorthY, indexBelowZ, indexAfterT, seed );
		unsigned int noiseAfterBNE = Get4dNoiseUint( indexEastX, indexNorthY, indexBelowZ, indexAfterT, seed );
		unsigned int noiseAfterASW = Get4dNoiseUint( indexWestX, indexSouthY, indexAboveZ, indexAfterT, seed );
		unsigned int noiseAfterASE = Get4dNoiseUint( indexEastX, indexSouthY, indexAboveZ, indexAfterT, seed );
		unsigned int noiseAfterANW = Get4dNoiseUint( indexWestX, indexNorthY, indexAboveZ, indexAfterT, seed );
		unsigned int noiseAfterANE = Get4dNoiseUint( indexEastX, indexNorthY, indexAboveZ, indexAfterT, seed );

		// Mask with 15 (mod 16) to look up in gradients table
		Vec4 gradientBeforeBSW = gradients[ noiseBeforeBSW & 0x0000000F ];
		Vec4 gradientBeforeBSE = gradients[ noiseBeforeBSE & 0x0000000F ];
		Vec4 gradientBeforeBNW = gradients[ noiseBeforeBNW & 0x0000000F ];
		Vec4 gradientBeforeBNE = gradients[ noiseBeforeBNE & 0x0000000F ];
		Vec4 gradientBeforeASW = gradients[ noiseBeforeASW & 0x0000000F ];
		Vec4 gradientBeforeASE = gradients[ noiseBeforeASE & 0x0000000F ];
		Vec4 gradientBeforeANW = gradients[ noiseBeforeANW & 0x0000000F ];
		Vec4 gradientBeforeANE = gradients[ noiseBeforeANE & 0x0000000F ];
		Vec4 gradientAfterBSW = gradients[ noiseAfterBSW & 0x0000000F ];
		Vec4 gradientAfterBSE = gradients[ noiseAfterBSE & 0x0000000F ];
		Vec4 gradientAfterBNW = gradients[ noiseAfterBNW & 0x0000000F ];
		Vec4 gradientAfterBNE = gradients[ noiseAfterBNE & 0x0000000F ];
		Vec4 gradientAfterASW = gradients[ noiseAfterASW & 0x0000000F ];
		Vec4 gradientAfterASE = gradients[ noiseAfterASE & 0x0000000F ];
		Vec4 gradientAfterANW = gradients[ noiseAfterANW & 0x0000000F ];
		Vec4 gradientAfterANE = gradients[ noiseAfterANE & 0x0000000F ];

		// Dot each corner's gradient with displacement from corner to position
		Vec4 displacementFromBeforeBSW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z, currentPos.w - cellMins.w );
		Vec4 displacementFromBeforeBSE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z, currentPos.w - cellMins.w );
		Vec4 displacementFromBeforeBNW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z, currentPos.w - cellMins.w );
		Vec4 displacementFromBeforeBNE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z, currentPos.w - cellMins.w );
		Vec4 displacementFromBeforeASW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z, currentPos.w - cellMins.w );
		Vec4 displacementFromBeforeASE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z, currentPos.w - cellMins.w );
		Vec4 displacementFromBeforeANW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z, currentPos.w - cellMins.w );
		Vec4 displacementFromBeforeANE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z, currentPos.w - cellMins.w );
		Vec4 displacementFromAfterBSW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z, currentPos.w - cellMaxs.w );
		Vec4 displacementFromAfterBSE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z, currentPos.w - cellMaxs.w );
		Vec4 displacementFromAfterBNW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z, currentPos.w - cellMaxs.w );
		Vec4 displacementFromAfterBNE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z, currentPos.w - cellMaxs.w );
		Vec4 displacementFromAfterASW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z, currentPos.w - cellMaxs.w );
		Vec4 displacementFromAfterASE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z, currentPos.w - cellMaxs.w );
		Vec4 displacementFromAfterANW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z, currentPos.w - cellMaxs.w );
		Vec4 displacementFromAfterANE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z, currentPos.w - cellMaxs.w );

		float dotBeforeBSW = GetDotProduct( gradientBeforeBSW, displacementFromBeforeBSW );
		float dotBeforeBSE = GetDotProduct( gradientBeforeBSE, displacementFromBeforeBSE );
		float dotBeforeBNW = GetDotProduct( gradientBeforeBNW, displacementFromBeforeBNW );
		float dotBeforeBNE = GetDotProduct( gradientBeforeBNE, displacementFromBeforeBNE );
		float dotBeforeASW = GetDotProduct( gradientBeforeASW, displacementFromBeforeASW );
		float dotBeforeASE = GetDotProduct( gradientBeforeASE, displacementFromBeforeASE );
		float dotBeforeANW = GetDotProduct( gradientBeforeANW, displacementFromBeforeANW );
		float dotBeforeANE = GetDotProduct( gradientBeforeANE, displacementFromBeforeANE );
		float dotAfterBSW = GetDotProduct( gradientAfterBSW, displacementFromAfterBSW );
		float dotAfterBSE = GetDotProduct( gradientAfterBSE, displacementFromAfterBSE );
		float dotAfterBNW = GetDotProduct( gradientAfterBNW, displacementFromAfterBNW );
		float dotAfterBNE = GetDotProduct( gradientAfterBNE, displacementFromAfterBNE );
		float dotAfterASW = GetDotProduct( gradientAfterASW, displacementFromAfterASW );
		float dotAfterASE = GetDotProduct( gradientAfterASE, displacementFromAfterASE );
		float dotAfterANW = GetDotProduct( gradientAfterANW, displacementFromAfterANW );
		float dotAfterANE = GetDotProduct( gradientAfterANE, displacementFromAfterANE );

		// Do a smoothed (nonlinear) weighted average of dot results
		float weightEast  = SmoothStep3( displacementFromBeforeBSW.x );
		float weightNorth = SmoothStep3( displacementFromBeforeBSW.y );
		float weightAbove = SmoothStep3( displacementFromBeforeBSW.z );
		float weightAfter = SmoothStep3( displacementFromBeforeBSW.w );
		float weightWest   = 1.f - weightEast;
		float weightSouth  = 1.f - weightNorth;
		float weightBelow  = 1.f - weightAbove;
		float weightBefore = 1.f - weightAfter;

		// 16-way blend (16 -> 8 -> 4 -> 2 -> 1)
		float blendBeforeBelowSouth	= (weightEast * dotBeforeBSE) + (weightWest * dotBeforeBSW);
		float blendBeforeBelowNorth	= (weightEast * dotBeforeBNE) + (weightWest * dotBeforeBNW);
		float blendBeforeAboveSouth	= (weightEast * dotBeforeASE) + (weightWest * dotBeforeASW);
		float blendBeforeAboveNorth	= (weightEast * dotBeforeANE) + (weightWest * dotBeforeANW);
		float blendAfterBelowSouth  = (weightEast * dotAfterBSE)  + (weightWest * dotAfterBSW);
		float blendAfterBelowNorth  = (weightEast * dotAfterBNE)  + (weightWest * dotAfterBNW);
		float blendAfterAboveSouth  = (weightEast * dotAfterASE)  + (weightWest * dotAfterASW);
		float blendAfterAboveNorth  = (weightEast * dotAfterANE)  + (weightWest * dotAfterANW);
		float blendBeforeBelow = (weightSouth * blendBeforeBelowSouth) + (weightNorth * blendBeforeBelowNorth);
		float blendBeforeAbove = (weightSouth * blendBeforeAboveSouth) + (weightNorth * blendBeforeAboveNorth);
		float blendAfterBelow  = (weightSouth * blendAfterBelowSouth)  + (weightNorth * blendAfterBelowNorth);
		float blendAfterAbove  = (weightSouth * blendAfterAboveSouth)  + (weightNorth * blendAfterAboveNorth);
		float blendBefore = (weightBelow * blendBeforeBelow) + (weightAbove * blendBeforeAbove);
		float blendAfter  = (weightBelow * blendAfterBelow) + (weightAbove * blendAfterAbove);
		float blendTotal = (weightBefore * blendBefore) + (weightAfter * blendAfter);
		float noiseThisOctave = blendTotal * (1.f / 0.6875f); // 4D Perlin is in [-.6875,.6875]; map to ~[-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.y += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.z += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.w += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to [0,1]
		totalNoise = SmoothStep3( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}

