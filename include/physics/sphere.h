#include "body.h"

/**
 * Basic sphere object to be displayed
 *
 */

class Sphere : public Body {
public:
  int getRadius() const;

  bool update();

private:
  int radius_;
};