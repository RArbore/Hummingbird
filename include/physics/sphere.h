#include "body.h"

/**
 * Basic sphere object to be displayed
 *
 */

class Sphere : public Body {
public:
  int getRadius() const;

private:
  int radius_;

  void calcForceVector();

  bool collided(Body &body);
};