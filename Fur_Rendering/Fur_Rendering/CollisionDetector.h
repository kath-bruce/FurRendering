#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include <iostream>
#include "Model.h"

//header and cpp file adapted from games tech group project/rt3d group project

class CollisionDetector {
public:
	static bool detectCollision(Model &modA, Model &modB);
};

#endif
