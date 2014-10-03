#pragma once

#include "custom_shapes.h"

enum draw { CURRENT, BUFFER, BACKUP, RESTORE };
enum Tools { PEN, LINE, RECTANGLE, ELLIPSE, POLY, TEXT, NONE };

#define BACKUPS 11
#define DEFAULT_ZOOM 1
#define DELTA 1.1
