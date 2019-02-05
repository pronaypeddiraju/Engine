#pragma once
//------------------------------------------------------------------------------------------------------------------------------
//Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Commons/ErrorWarningAssert.hpp"

#include <functional>

typedef unsigned int uint;

class Collider2D;

struct Collision2D
{
	// for now, nothing, 
	// but next week will contain information 
	// for correction; 
};

typedef std::function<bool(Collision2D* out, Collider2D* a, Collider2D* b)> CollisionCheck2DCallback ;

// 2D arrays are [Y][X] remember
extern CollisionCheck2DCallback COLLISION_LOOKUP_TABLE[][COLLIDER2D_COUNT];

bool				CheckAABB2ByAABB2(Collision2D* out, Collider2D* a, Collider2D* b);
bool				CheckAABB2ByDisc(Collision2D* out, Collider2D* a, Collider2D* b);
bool				CheckDiscByDisc(Collision2D* out, Collider2D* a, Collider2D* b);
bool				CheckDiscByAABB2(Collision2D* out, Collider2D* a, Collider2D* b);
bool				GetCollisionInfo( Collision2D *out, Collider2D * a, Collider2D *b );