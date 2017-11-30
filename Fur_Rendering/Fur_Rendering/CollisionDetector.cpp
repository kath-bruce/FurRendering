#include "CollisionDetector.h"

//header and cpp file adapted from games tech group project/rt3d group project

bool CollisionDetector::detectCollision(Model &modA, Model &modB)
{
	if (modA.getPos().x + modA.getScale().x > modB.getPos().x - modB.getScale().x
		&& modA.getPos().x - modA.getScale().x < modB.getPos().x + modB.getScale().x)
	{
		if (modA.getPos().z + modA.getScale().z > modB.getPos().z - modB.getScale().z
			&& modA.getPos().z - modA.getScale().z < modB.getPos().z + modB.getScale().z)
		{
			if (modA.getPos().y + modA.getScale().y > modB.getPos().y - modB.getScale().y
				&& modA.getPos().y - modA.getScale().y < modB.getPos().y + modB.getScale().y)
			{
				return true;
			}
		}
	}
	return false;
}
