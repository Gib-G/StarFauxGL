#pragma once
#include "Types.h"

enum class EAxis : uint8_t { X, Y, Z, EnumCount };
static constexpr uint8_t Dim = uint8_t(EAxis::EnumCount);

// Directions related to the axes in EAxis, in the same order.
enum class EDirection : uint8_t { Right, Up, Forward, EnumCount };