#pragma once

#include <vector>
#include "lib/binarytools/BinaryWriter.h"

void write_aiff(std::vector<char> data, LUS::BinaryWriter& writer);