#ifndef A2S_RULE_PARSER_H
#define A2S_RULE_PARSER_H

#include <stdint.h>
#include <stddef.h>

#include "dayz_mod.h"


bool parse_a2s_rules(
        uint8_t* buffer, size_t size, 
        struct dayz_mod* mods, uint8_t* num_mods);




#endif
