
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


#include "a2s_parser.h"
#include "dzlauncher.h"



//#define DEBUG


/*
static uint16_t read_u16(uint8_t* buf) {
    return buf[0] | (buf[1] << 8);
}

static uint32_t read_u32(uint8_t* buf) {
    return buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
}

static int count_bits(uint16_t i) {
    int count = 0;
    while(i) {
        count += i&1;
        i >>= 1;
    }
    return count;
}
*/

static uint32_t read_u32(uint8_t* buf) {
    return buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
}




bool parse_a2s_rules(
        uint8_t* buffer, size_t size,
        struct dayz_mod* mods, uint8_t* num_mods
){

    if(size < 6) {
        fprintf(stderr, "%s: Too small A2S response buffer to be valid.\n",
                __func__);
        return false;
    }

    size_t offset = 0;


    //uint32_t dlc_hash = 0;
    uint8_t mods_count = 0;

    // ID length (byte)
    // Mod steam ID (uint32)
    // Mod name (max 256)


    // Skip protocol version and overflow flags.
    offset += 2;
    //uint8_t protocol_version = buffer[offset++];
    //uint8_t overflow_flags = buffer[offset++];
    //printf(" \033[35mProtocol Version = %i\033[0m\n", protocol_version);


    // Skip DLC flags.
    offset += sizeof(uint32_t); 
    //uint32_t dlc_hash = read_u32(buffer + offset);
    //offset += sizeof(uint32_t);

    mods_count = buffer[offset++];
   
    *num_mods = mods_count;
   
    /*
    printf("Protocol version: %i\n", protocol_version);
    printf("Overflow flags: %i\n", overflow_flags);
    printf("DLC flags: %i | dlc: %s\n", dlc_flags, (count_bits(dlc_flags) == 1) ? "yes" : "no");
    printf("Mods count: %i\n", mods_count);
    */
    // note: there is 10 bytes before mod name.
    
    for(int mi = 0; mi < mods_count; mi++) {
        struct dayz_mod* mod = &mods[mi];

   
#ifdef DEBUG
        uint32_t mod_hash = read_u32(buffer + offset); 
        printf("\n\033[36m(DEBUG): (4:u32) mod hash = %u\033[0m\n", mod_hash);
#endif
        offset += sizeof(uint32_t);

        uint8_t modid_len = buffer[offset++] & 0x0F;

#ifdef DEBUG
        printf("\033[36m(DEBUG): (1:u8) mod id length = %u\033[0m\n", modid_len);
#endif

        /*
        // For now assume all mod id lengths are the same. 4 bytes.
        if(modid_len > 4) {

            fprintf(stderr,
                    "\033[31m\n"
                    "=================================================================\n"
                    " [ WARNING ]: Mod id length of %i bytes are not supported\n"
                    "              by the current version of this software.\n"
                    "=================================================================\n"
                    "\033[0m\n", modid_len);



            return false;
        }
        */


        // TODO: Add something which will tell user that
        //       this failed to get the workshop id but we must be able to parse the rest.
        //       so try to look for bytes which could be the mod name length (if everything fails)


        mod->workshop_id = read_u32(buffer + offset);

        // Sometimes there may be invalid workshop id
        // for some reason its not always 4 bytes long and its bit of a mess...
        // TODO: Add explain from notes if this works..

        if(mod->workshop_id < 0x3b9aca00) {
            
            uint8_t bytes[8] = { 0 };
            memmove(bytes, buffer + offset, 8);

            bytes[3] = bytes[7];
            memset(bytes + 4, 0, 8-4);

            mod->workshop_id = read_u32(bytes);

            offset += 8;
        }
        else {
            offset += modid_len; 
        }

#ifdef DEBUG
        printf("\033[36m(DEBUG): (4:u32) workshop id = %u\033[0m\n", mod->workshop_id);
#endif

        mod->name_length = buffer[offset++];
        memset(mod->name, 0, DAYZ_MODNAME_MAX);


        // Read only printable ASCII characters in the name.
        for(uint8_t k = 0; k < mod->name_length; k++) {
            char ch = (char)buffer[offset];
            if((ch >= 0x20) && (ch <= 0x7E)) {
                mod->name[k] = ch;
            } 
            else
            if(k > 0) {
                k--;
            }
            offset++;
        }

#ifdef DEBUG
        printf("\033[36m(DEBUG): (%i) mod name = '%s'\033[0m\n", mod->name_length, mod->name);
#endif
    }

    return true;
}



