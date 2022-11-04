#if defined _pro_equip_available_weapons_included
    #endinput
#endif
#define _pro_equip_available_weapons_included

#include <pro_equip/arrays.inc>
#include <pro_equip/vars.inc>
#include <pro_equip/prefs.inc>

enum struct AvailableWeapons {
    int primary_choices[32]; // Warning: the ordering of the indexes can change.  Do not store indexes of this array - instead store its value
    int secondary_choices[32]; // Warning: the ordering of the indexes can change.  Do not store indexes of this array - instead store its value
    int grenade_choices[32]; // only used for random grenades - Warning: the ordering of the indexes can change.  Do not store indexes of this array - instead store its value
    int num_primaries;
    int num_secondaries;
    int num_grenades;
    int slots[4];
    
    void debug() {
        #if DEBUGGING
            for(int i=0; i < this.num_primaries; i++) {
                LogToFileEx(EquipLogFile, "Primary[%i] = %s", i, primaries[this.primary_choices[i]]);
            }
            for(int i=0; i < this.num_secondaries; i++) {
                LogToFileEx(EquipLogFile, "Secondary[%i] = %s", i, secondaries[this.secondary_choices[i]]);
            }
            for(int i=0; i < this.num_grenades; i++) {
                LogToFileEx(EquipLogFile, "Grenade[%i] = %s", i, grenades[this.grenade_choices[i]]);
            }
            for(int i=0; i < 4; i++) {
                LogToFileEx(EquipLogFile, "slot[%i] = %b", i, this.slots)
            }
        #endif
    }
    
    int weapon_bit(const char[] weapon) {
        int idx = -1;
        weapon_indexes.GetValue(weapon, idx);
        return this.index_bit(idx);
    }
    int index_bit(int idx) {
        return 1 << idx;
    }
    bool is_weapon_available(const char[] weapon) {
        int slot = -1;
        if(!weapon_slots.GetValue(weapon, slot)) {
            return false;
        }
        return (this.slots[slot] & this.weapon_bit(weapon)) != 0;
    }
    bool is_weapon_idx_available(int slot, int idx) {
        return (this.slots[slot] & this.index_bit(idx)) != 0;
    }
    void reset_weapons_available(int slot) {
        this.slots[slot] = 0x0;
    }
    bool weapon_idx_available(int idx, int slot) {
        return (this.slots[slot] & this.index_bit(idx)) != 0;
    }
    void set_weapon_available(int idx, int slot) {
        this.slots[slot] = this.slots[slot] | this.index_bit(idx);
    }
    void clear_weapon_available(int idx, int slot) {
        this.slots[slot] = this.slots[slot] & ~this.index_bit(idx);
    }
    
    
    // set unique to true for player weapons and false for bot weapons
    bool add_choice(const char[] weapon, bool unique, int at=-1) {
        int slot = -1;
        int index = -1;
        char buffer[32];
        buffer = "weapon_"
        if(weapon[0] == 'w' && weapon[1] == 'e' && weapon[2] == 'a' && weapon[3] == 'p' && weapon[4] == 'o' && weapon[5] == 'n' && weapon[6] == '_') {
            strcopy(buffer, sizeof(buffer), weapon);
        } else {
            // Format(buffer, sizeof(buffer), "weapon_%s", weapon);
            strcopy(buffer[7], sizeof(buffer)-7, weapon);
        }
        if(weapon_slots.GetValue(buffer, slot) && weapon_indexes.GetValue(buffer, index)) {
            bool result;
            switch(slot) {
                case 0: {
                    if(this.num_primaries >= 32) {
                        return false;
                    }
                    if(at == -1) {
                        result = add_item(this.primary_choices, this.num_primaries, index, unique);
                        if(result) {
                            this.set_weapon_available(index, slot);
                        }
                        return result;
                    } else {
                        result = add_item_at(this.primary_choices, this.num_primaries, index, unique, at);
                        if(result) {
                            this.set_weapon_available(index, slot);
                        }
                        return result;
                    }
                }
                case 1: {
                    if(this.num_secondaries >= 32) {
                        return false;
                    }
                    if(at == -1) {
                        result = add_item(this.secondary_choices, this.num_secondaries, index, unique);
                        if(result) {
                            this.set_weapon_available(index, slot);
                        }
                        return result;
                    } else {
                        result = add_item_at(this.secondary_choices, this.num_secondaries, index, unique, at);
                        if(result) {
                            this.set_weapon_available(index, slot);
                        }
                        return result;
                    }
                }
                case 3: {
                    if(this.num_grenades >= 32) {
                        return false;
                    }
                    if(at == -1) {
                        result = add_item(this.grenade_choices, this.num_grenades, index, unique);
                        if(result) {
                            this.set_weapon_available(index, slot);
                        }
                        return result;
                    } else {
                        result = add_item_at(this.grenade_choices, this.num_grenades, index, unique, at);
                        if(result) {
                            this.set_weapon_available(index, slot);
                        }
                        return result;
                    }
                }
            }
        }
        return false;
    }
    
    // expects a weapon's entity name, e.g. weapon_deagle
    bool remove_choice(const char[] weapon) {
        int slot = -1;
        int index = -1;
        bool result;
        if(weapon_slots.GetValue(weapon, slot) && weapon_indexes.GetValue(weapon, index)) {
            switch(slot) {
                case 0: {
                    result = remove_item_all(this.primary_choices, this.num_primaries, index) > 0;
                    this.clear_weapon_available(index, slot);
                    return result;
                }
                case 1: {
                    result = remove_item_all(this.secondary_choices, this.num_secondaries, index) > 0;
                    this.clear_weapon_available(index, slot);
                    return result;
                }
                case 3: {
                    result = remove_item_all(this.grenade_choices, this.num_grenades, index) > 0;
                    this.clear_weapon_available(index, slot);
                    return result;
                }
            }
        }
        return false;
    }
    void debug_list_items() {
        LogToFileEx(EquipLogFile, "Primary items - %i", this.num_primaries);
        for(int i=0; i < this.num_primaries; i++) {
            LogToFileEx(EquipLogFile, "%i: %s", i, primaries[this.primary_choices[i]]);
        }
        LogToFileEx(EquipLogFile, "Secondary items - %i", this.num_secondaries);
        for(int i=0; i < this.num_secondaries; i++) {
            LogToFileEx(EquipLogFile, "%i: %s", i, secondaries[this.secondary_choices[i]]);
        }
        LogToFileEx(EquipLogFile, "Grenades - %i", this.num_grenades);
        for(int i=0; i < this.num_grenades; i++) {
            LogToFileEx(EquipLogFile, "%i: %s", i, grenades[this.grenade_choices[i]]);
        }
    }
    int weapon_index(int idx, int slot) {
        switch(slot) {
            case 0: {
                for(int i=0; i < this.num_primaries; i++) {
                    if(this.primary_choices[i] == idx) {
                        return i;
                    }
                }
            }
            case 1: {
                for(int i=0; i < this.num_secondaries; i++) {
                    if(this.secondary_choices[i] == idx) {
                        return i;
                    }
                }
            }
            case 3: {
                for(int i=0; i < this.num_grenades; i++) {
                    if(this.grenade_choices[i] == idx) {
                        return i;
                    }
                }
            }
        }
        return -1;
    }
    bool has_weapon(char[] weapon) {
        int slot;
        if(weapon_slots.GetValue(weapon, slot)) {
            switch(slot) {
                case 0: {
                    for(int i=0; i < this.num_primaries; i++) {
                        if(StrEqual(weapon, primaries[this.primary_choices[i]])) {
                            return true;
                        }
                    }
                }
                case 1: {
                    for(int i=0; i < this.num_secondaries; i++) {
                        if(StrEqual(weapon, secondaries[this.secondary_choices[i]])) {
                            return true;
                        }
                    }
                }
                case 3: {
                    for(int i=0; i < this.num_grenades; i++) {
                        if(StrEqual(weapon, grenades[this.grenade_choices[i]])) {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
        
    }
    bool truncate_slot(int slot) {
        this.reset_weapons_available(slot);
        switch(slot) {
            case 0: {
                this.num_primaries = 0;
            }
            case 1: {
                this.num_secondaries = 0;
            }
            case 3: {
                this.num_grenades = 0;
            }
            default: {
                return false;
            }
        }
        return true;
    }
}


void copy_available_weapons(AvailableWeapons from, AvailableWeapons to) {
    for(int i=0; i < sizeof(from.primary_choices); i++) {
        to.primary_choices[i] = from.primary_choices[i];
    }
    for(int i=0; i < sizeof(from.secondary_choices); i++) {
        to.secondary_choices[i] = from.secondary_choices[i];
    }
    for(int i=0; i < sizeof(from.grenade_choices); i++) {
        to.grenade_choices[i] = from.grenade_choices[i];
    }
    to.num_primaries = from.num_primaries;
    to.num_secondaries = from.num_secondaries;
    to.num_grenades = from.num_grenades;
    for(int i=0; i < 4; i++) {
        to.slots[i] = from.slots[i];
    }
}


AvailableWeapons player_weapons; // what weapons are available to the players - maps or admins can customize this
AvailableWeapons player_weapons_defaults;
AvailableWeapons bot_weapons; // what weapons bots are allowed to have - maps or admins can customize this
AvailableWeapons bot_weapons_defaults;

AvailableWeapons gun_menu_order;


