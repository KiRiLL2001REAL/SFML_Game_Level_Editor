#pragma once

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <list>
#include <map>

#include "sfml/Graphics.hpp"
#include "huffman.h"
#include "json.hpp"
#include "other.h"

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)
#define CLAMP(x, bord_l, bord_r) (MAX(MIN(x, bord_r), bord_l))