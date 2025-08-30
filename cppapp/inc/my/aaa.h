#pragma once

#include <iostream>
#include <string>

class AAA {
private:
  std::string name;
  int value;

public:
  const std::string getName();
  int getValue() const;
};
