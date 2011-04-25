#ifndef EMO_TYPES_H
#define EMO_TYPES_H

#include <string>
#include <hash_map>
#include "Drawable.h"

typedef std::hash_map <std::string, std::string> kvs_t;
typedef std::hash_map <std::string, emo::Drawable *> drawables_t;

#endif
