#pragma once
/**
 * The abstract class from which all bodies in simulation derive from
 *
 */

class Body {
public:
  int getXPos() const;
  int getYPos() const;
  int getZPos() const;

  double getMass() const;

  virtual bool update() = 0;

private:
  int xPos_, yPos_, zPos_;

  double mass_;
};