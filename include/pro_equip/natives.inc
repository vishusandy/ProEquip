#if defined _pro_equip_natives_included
    #endinput
#endif
#define _pro_equip_natives_included

#include <pro_equip/arrays.inc>
#include <pro_equip/constants.inc>
#include <pro_equip/weapons.inc>


public int Native_ResetEquip(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    if(client == EQUIP_PLAYERS) {
        copy_equip(player_equip_defaults, player_equip);
        copy_available_weapons(player_weapons_defaults, player_weapons);
        player_equip.build_random_grenades_list(player_weapons);
        // reset_client_data(client);
    } else if(client == EQUIP_BOTS) {
        copy_equip(bot_equip_defaults, bot_equip);
        copy_available_weapons(bot_weapons_defaults, bot_weapons);
        bot_equip.build_random_grenades_list(bot_weapons);
        // reset_client_data(client);
    } else if(client == EQUIP_BOTH) {
        copy_equip(player_equip_defaults, player_equip);
        copy_available_weapons(player_weapons_defaults, player_weapons);
        player_equip.build_random_grenades_list(player_weapons);
        copy_equip(bot_equip_defaults, bot_equip);
        copy_available_weapons(bot_weapons_defaults, bot_weapons);
        bot_equip.build_random_grenades_list(bot_weapons);
        // reset_client_data(client);
    } else {
        bool weapons_changed = false;
        player_data[client].reset_admin_overrides(weapons_changed);
        if(weapons_changed) {
            display_menu(client);
        }
        return 1;
    }
    bool is_bot;
    int processed = 0;
    for(int i=1; i < MaxClients; i++) {
        is_bot = (!IsClientInGame(i) || IsFakeClient(i))? true: false;
        if(client == EQUIP_BOTH || (client == EQUIP_BOTS && is_bot) || (client == EQUIP_PLAYERS && !is_bot)) {
            player_data[i].reset_admin_overrides();
            processed++;
        }
    }
    return processed;
}
public int Native_ResetEquipAll(Handle plugin, int numParams) {
    for(int i=1; i < MaxClients; i++) {
        player_data[i].reset_admin_overrides();
    }
}
public int Native_ResetWeaponPrefs(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    int reset_to = (numParams < 2)? RANDOM_WEAPON_ALL: GetNativeCell(2);
    reset_invalid_weapon_prefs(client, reset_to);
}
public int Native_ResetGrenadeOverrides(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    if(client == EQUIP_PLAYERS) {
        player_equip.hegrenades = player_equip_defaults.hegrenades;
        player_equip.flashbangs = player_equip_defaults.flashbangs;
        player_equip.smokegrenades = player_equip_defaults.smokegrenades;
        // player_equip.build_random_grenades_list(player_weapons);
    } else if(client == EQUIP_BOTS) {
        bot_equip.hegrenades = bot_equip_defaults.hegrenades;
        bot_equip.flashbangs = bot_equip_defaults.flashbangs;
        bot_equip.smokegrenades = bot_equip_defaults.smokegrenades;
        // bot_equip.build_random_grenades_list(bot_weapons);
    } else if(client == EQUIP_BOTH) {
        player_equip.hegrenades = player_equip_defaults.hegrenades;
        player_equip.flashbangs = player_equip_defaults.flashbangs;
        player_equip.smokegrenades = player_equip_defaults.smokegrenades;
        bot_equip.hegrenades = bot_equip_defaults.hegrenades;
        bot_equip.flashbangs = bot_equip_defaults.flashbangs;
        bot_equip.smokegrenades = bot_equip_defaults.smokegrenades;
    } else {
        player_data[client].reset_grenades();
    }
}
public int Native_OverrideGrenades(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    int len;
    GetNativeStringLength(2, len);
    if(len <= 0)
        return 0;
    char[] grenade = new char[len+1+7];
    strcopy(grenade, len+1+7, "weapon_");
    GetNativeString(2, grenade[7], len+1);
    int num = GetNativeCell(3);
    #if DEBUGGING
        LogToFileEx(EquipLogFile, "Native_OverrideGrenades() - client=%i grenade='%s' amount=%i", client, grenade, num);
    #endif
    int bot_num = num;
    int player_num = num;
    if(num < 0) {
        #if DEBUGGING
            LogToFileEx(EquipLogFile, "Native_OverrideGrenades() - setting default grenade quantity");
        #endif
        bot_num = bot_equip.grenade_quantity(grenade);
        player_num = player_equip.grenade_quantity(grenade);
    }
    if(client == EQUIP_PLAYERS) {
        player_equip.grenade_override(grenade[7], player_num);
        player_equip.build_random_grenades_list(player_weapons);
        return true;
    }
    if(client == EQUIP_BOTS) {
        bot_equip.grenade_override(grenade[7], bot_num);
        bot_equip.build_random_grenades_list(bot_weapons);
        return true;
    }
    if(client == EQUIP_BOTH) {
        player_equip.grenade_override(grenade[7], player_num);
        player_equip.build_random_grenades_list(player_weapons);
        bot_equip.grenade_override(grenade[7], bot_num);
        bot_equip.build_random_grenades_list(bot_weapons);
        return true;
    }
    if(player_data[client].equip.grenade_override(grenade[7], num)) {
        #if DEBUGGING
            LogToFileEx(EquipLogFile, "Native_OverrideGrenades() - give_grenades(%N, %s, %i)", client, grenade, num);
        #endif
        return give_grenades(client, grenade, num);
    } else {
        LogToFileEx(EquipLogFile, "Grenade override failed");
    }
    return false;
}

public int Native_SetGrenade(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    int len;
    GetNativeStringLength(2, len);
    if(len <= 0)
        return 0;
    char[] grenade = new char[len+1+7];
    strcopy(grenade, len+1+7, "weapon_");
    GetNativeString(2, grenade[7], len+1);
    int num = GetNativeCell(3);
    if (num > MAX_GRENADES) {
        num = MAX_GRENADES;
    } else if(num < INFINITE_AMMO) {
        num = 0;
    }
    switch(client) {
        case EQUIP_PLAYERS: {
            if(num == NO_OVERRIDE) {
                num = player_equip_defaults.grenade_quantity(grenade);
            }
            player_equip.grenade_override(grenade[7], num);
            player_equip.build_random_grenades_list(player_weapons);
        }
        case EQUIP_BOTS: {
            if(num == NO_OVERRIDE) {
                num = bot_equip_defaults.grenade_quantity(grenade);
            }
            bot_equip.grenade_override(grenade[7], num);
            bot_equip.build_random_grenades_list(bot_weapons);
        }
        case EQUIP_BOTH: {
            if(num == NO_OVERRIDE) {
                player_equip.grenade_override(grenade[7], player_equip_defaults.grenade_quantity(grenade));
                player_equip.build_random_grenades_list(player_weapons);
                bot_equip.grenade_override(grenade[7], bot_equip_defaults.grenade_quantity(grenade));
                bot_equip.build_random_grenades_list(bot_weapons);
            } else {
                player_equip.grenade_override(grenade[7], num);
                player_equip.build_random_grenades_list(player_weapons);
                bot_equip.grenade_override(grenade[7], num);
                bot_equip.build_random_grenades_list(bot_weapons);
            }
        }
        default: {
            return player_data[client].equip.grenade_override(grenade[7], num);
        }
    }
    return true;
}

public int Native_GetCurGrenadeQuantity(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    int len;
    GetNativeStringLength(2, len);
    if(len <= 0)
        return 0;
    char[] grenade = new char[len+1+7];
    strcopy(grenade, len+1+7, "weapon_");
    GetNativeString(2, grenade[7], len+1);
    int offset = 0;
    
    if(StrEqual(grenade, "hegrenade", false)) {
        offset = 11;
    } else if(StrEqual(grenade, "flashbang", false)) {
        offset = 12;
    } else if(StrEqual(grenade, "smokegrenade", false)) {
        offset = 13;
    } else {
        return -1;
    }
    return GetEntProp(client, Prop_Send, "m_iAmmo", 4, offset);
}

public int Native_GetGrenadeQuantity(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    int len;
    GetNativeStringLength(2, len);
    if(len <= 0)
        return 0;
    char[] grenade = new char[len+1+7];
    strcopy(grenade, len+1+7, "weapon_");
    GetNativeString(2, grenade[7], len+1);
    switch(client) {
        case EQUIP_PLAYERS: {
            return player_equip.grenade_quantity(grenade);
        }
        case EQUIP_BOTS: {
            return bot_equip.grenade_quantity(grenade);
        }
        default: {
            int g = player_data[client].equip.grenade_quantity(grenade);
            if(g == NO_OVERRIDE) {
                if(client <= 0 || !IsClientInGame(client) || IsFakeClient(client)) {
                    return bot_equip.grenade_quantity(grenade);
                } else {
                    return player_equip.grenade_quantity(grenade);
                }
            } else {
                return g;
            }
        }
    }
}

public int Native_GiveGrenade(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    int num = GetNativeCell(3);
    int len;
    GetNativeStringLength(2, len);
    if(len <= 0)
        return 0;
    char[] grenade = new char[len+1+7];
    strcopy(grenade, len+1+7, "weapon_");
    GetNativeString(2, grenade[7], len+1);
    if(num == -1) {
        num = (IsClientInGame(client) && !IsFakeClient(client))? player_equip.grenade_quantity(grenade): bot_equip.grenade_quantity(grenade);
    }
    return give_grenades(client, grenade, num);
}



public int Native_SetKnife(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    bool on = GetNativeCell(2);
    return set_knife(client, on);
}

public int Native_SetEquipHP(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    int hp = GetNativeCell(2);
    if(client == EQUIP_PLAYERS) {
        player_equip.hp = hp;
    } else if(client == EQUIP_BOTS) {
        bot_equip.hp = hp;
    } else if(client == EQUIP_BOTH) {
        player_equip.hp = hp;
        bot_equip.hp = hp;
    } else if(client == 0 || !IsClientInGame(client)) {
        return false;
    } else {
        player_data[client].equip.hp = hp;
    }
    return true;
}
public int Native_SetEquipArmor(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    int armor = GetNativeCell(2);
    if(client == EQUIP_PLAYERS) {
        player_equip.armor = armor;
    } else if(client == EQUIP_BOTS) {
        bot_equip.armor = armor;
    } else if(client == EQUIP_BOTH) {
        player_equip.armor = armor;
        bot_equip.armor = armor;
    } else if(client == 0 || !IsClientInGame(client)) {
        return false;
    } else {
        player_data[client].equip.armor = armor;
    }
    return true;
}
public int Native_SetEquipNightvision(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    bool nv = GetNativeCell(2);
    if(client == EQUIP_PLAYERS) {
        player_equip.nightvision = nv;
    } else if(client == EQUIP_BOTS) {
        bot_equip.nightvision = nv;
    } else if(client == EQUIP_BOTH) {
        player_equip.nightvision = nv;
        bot_equip.nightvision = nv;
    } else {
        player_data[client].equip.nightvision = nv;
    }
}
public int Native_SetEquipDefuseKit(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    bool defuse = GetNativeCell(2);
    if(client == EQUIP_PLAYERS) {
        player_equip.defusekit = defuse;
    } else if(client == EQUIP_BOTS) {
        bot_equip.defusekit = defuse;
    } else if(client == EQUIP_BOTH) {
        player_equip.defusekit = defuse;
        bot_equip.defusekit = defuse;
    } else {
        player_data[client].equip.defusekit = defuse;
    }
}


public int Native_SetPlayerNightvisionOn(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    bool nv = GetNativeCell(2);
    if(client == EQUIP_PLAYERS) {
        player_equip.nightvision = nv;
    } else if(client == EQUIP_BOTS) {
        bot_equip.nightvision = nv;
    } else if(client == EQUIP_BOTH) {
        player_equip.nightvision = nv;
        bot_equip.nightvision = nv;
    } else if(IsClientInGame(client) && !IsFakeClient(client)) {
        player_data[client].equip.nightvision = nv;
        show_nightvision(client);
    }
}
public int Native_DisplayPlayerNightvision(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    bool nv = GetNativeCell(2);
    display_nightvision(client, nv);
}


public int Native_EnablePrimaryMenu(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    if(client == EQUIP_PLAYERS) {
        weapon_menu.primary_menu_enabled = true;
    } else {
        player_data[client].menu.primary_menu_enabled = true;
    }
}
public int Native_DisablePrimaryMenu(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    if(client == EQUIP_PLAYERS) {
        weapon_menu.primary_menu_enabled = false;
    } else {
        player_data[client].menu.primary_menu_enabled = false;
    }
}
public int Native_EnableSecondaryMenu(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    if(client == EQUIP_PLAYERS) {
        weapon_menu.secondary_menu_enabled = true;
    } else {
        player_data[client].menu.secondary_menu_enabled = true;
    }
}
public int Native_DisableSecondaryMenu(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    if(client == EQUIP_PLAYERS) {
        weapon_menu.secondary_menu_enabled = false;
    } else {
        player_data[client].menu.secondary_menu_enabled = false;
    }
}
public int Native_SetMenuState(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    int state = GetNativeCell(2)
    player_data[client].menu_state.show_menu = view_as<showMenu>(state);
}
public int Native_SetNextMenuState(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    int state = GetNativeCell(2)
    player_data[client].menu_state.next_menu = view_as<showMenu>(state);
}
public int Native_SetPrevMenuState(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    int state = GetNativeCell(2)
    player_data[client].menu_state.prev_menu = view_as<showMenu>(state);
}

public int Native_SetAvailableWeapon(Handle plugin, int numParams) {
    int len;
    GetNativeStringLength(1, len);
    if(len <= 0)
        return false;
    char[] gun = new char[len+1+7];
    strcopy(gun, len+1+7, "weapon_");
    GetNativeString(1, gun[7], len+1);
    bool players = GetNativeCell(2);
    bool bots = GetNativeCell(3);
    bool remove = GetNativeCell(4);
    int slot = WEAPON_LOOKUP_FAIL;
    int idx;
    bool result = false;
    if(!weapon_slots.GetValue(gun, slot)) {
        return false;
    }
    if(slot == 2) {
        int target = (players)? EQUIP_PLAYERS: 0;
        target += (bots)? EQUIP_BOTS: 0;
        set_knife(target, !remove);
    } else if((slot == 0 || slot == 1 || slot == 3) && weapon_indexes.GetValue(gun, idx)) {
        if(!remove) {
            // ADD WEAPON
            if(players) {
                int w = player_weapons_defaults.weapon_index(idx, slot); // find original position
                if(w == -1) {
                    w = slot_size(slot, false); // add to end
                }
                #if DEBUGGING
                    LogToFileEx(EquipLogFile, "players w=%i", w);
                #endif
                int pos = -1;
                switch(slot) {
                    case 0: 
                        pos = add_position(player_weapons_defaults.primary_choices, player_weapons_defaults.num_primaries, player_weapons.primary_choices, player_weapons.num_primaries, w);
                    case 1: 
                        pos = add_position(player_weapons_defaults.secondary_choices, player_weapons_defaults.num_secondaries, player_weapons.secondary_choices, player_weapons.num_secondaries, w);
                    case 3: 
                        pos = add_position(player_weapons_defaults.grenade_choices, player_weapons_defaults.num_grenades, player_weapons.grenade_choices, player_weapons.num_grenades, w);
                }
                #if DEBUGGING
                    LogToFileEx(EquipLogFile, "pos=%i", pos)
                #endif
                result = player_weapons.add_choice(gun, true, pos);
            }
            if(bots) {
                int w = bot_weapons_defaults.weapon_index(idx, slot);
                if(w == -1) {
                    w = slot_size(slot, true);
                }
                #if DEBUGGING
                    LogToFileEx(EquipLogFile, "bots w=%i", w);
                #endif
                int pos = -1;
                switch(slot) {
                    case 0: 
                        pos = add_position(bot_weapons_defaults.primary_choices, bot_weapons_defaults.num_primaries, bot_weapons.primary_choices, bot_weapons.num_primaries, w);
                    case 1: 
                        pos = add_position(bot_weapons_defaults.secondary_choices, bot_weapons_defaults.num_secondaries, bot_weapons.secondary_choices, bot_weapons.num_secondaries, w);
                    case 3: 
                        pos = add_position(bot_weapons_defaults.grenade_choices, bot_weapons_defaults.num_grenades, bot_weapons.grenade_choices, bot_weapons.num_grenades, w);
                }
                result = result | bot_weapons.add_choice(gun, true, pos); // while bot weapons can have duplicates this would be confusing for users so only allow unique values to be added, -1
            }
        } else {
            // REMOVE WEAPON
            if(players) {
                result = player_weapons.remove_choice(gun);
            }
            if(bots) {
                result = result | bot_weapons.remove_choice(gun);
            }
            if(!result) {
                return false;
            }
            remove_player_weapon_from_all(idx, slot, players, bots);
        }
    }
    return result;
}


public int Native_EnableWeapons(Handle plugin, int numParams) {
    give_weapons = true;
}
public int Native_DisableWeapons(Handle plugin, int numParams) {
    give_weapons = false;
}
public int Native_WeaponsEnabled(Handle plugin, int numParams) {
    return give_weapons;
}

public int Native_ReloadConfigs(Handle plugin, int numParams) {
    return load_map_configs();
}

/**
 * For both pistol and rifle slots use `slot = -1`
 */
public int Native_ResetSlotPreference(Handle plugin, int numParams) {
    bool players = GetNativeCell(1);
    bool bots = GetNativeCell(2);
    int slot = GetNativeCell(3);
    bool remove = GetNativeCell(4);
    if(remove) {
        remove_player_weapon_slot(players, bots, slot, false);
    }
    for(int i=1; i < MaxClients; i++) {
        if(!IsClientInGame(i)) {
            continue;
        }
        switch(slot) {
            case -1: {
                player_data[i].guns.rifle = WEAPON_RESET;
                player_data[i].guns.pistol = WEAPON_RESET;
                player_data[i].menu_state.show_menu = ShowMenu_Primary;
                player_data[i].menu_state.next_menu = ShowMenu_Secondary;
            }
            case 0: {
                player_data[i].guns.rifle = WEAPON_RESET;
                player_data[i].menu_state.show_menu = ShowMenu_Primary;
            }
            case 1: {
                player_data[i].guns.pistol = WEAPON_RESET;
                player_data[i].menu_state.show_menu = ShowMenu_Secondary;
            }
        }
    }
    if(slot == -1) {
        return (players && player_weapons.truncate_slot(0))
                | (bots && bot_weapons.truncate_slot(0))
                | (players && player_weapons.truncate_slot(1))
                | (bots && bot_weapons.truncate_slot(1));
    }
    return (players && player_weapons.truncate_slot(slot)) || (bots && bot_weapons.truncate_slot(slot));
}

public int Native_ResetWeaponSlotAvailability(Handle plugin, int numParams) {
    bool players = GetNativeCell(1);
    bool bots = GetNativeCell(2);
    int slot = GetNativeCell(3);
    switch(slot) {
        case 0: {
            if(players) {
                player_weapons.num_primaries = player_weapons_defaults.num_primaries;
                for(int i=0; i < player_weapons_defaults.num_primaries; i++) {
                    player_weapons.primary_choices[i] = player_weapons_defaults.primary_choices[i];
                }
            }
            if(bots) {
                bot_weapons.num_primaries = bot_weapons_defaults.num_primaries;
                for(int i=0; i < bot_weapons_defaults.num_primaries; i++) {
                    bot_weapons.primary_choices[i] = bot_weapons_defaults.primary_choices[i];
                }
            }
            return true;
        }
        case 1: {
            if(players) {
                player_weapons.num_secondaries = player_weapons_defaults.num_secondaries;
                for(int i=0; i < player_weapons_defaults.num_secondaries; i++) {
                    player_weapons.secondary_choices[i] = player_weapons_defaults.secondary_choices[i];
                }
            }
            if(bots) {
                bot_weapons.num_secondaries = bot_weapons_defaults.num_secondaries;
                for(int i=0; i < bot_weapons_defaults.num_secondaries; i++) {
                    bot_weapons.secondary_choices[i] = bot_weapons_defaults.secondary_choices[i];
                }
            }
            return true;
        }
        case 3: {
            if(players) {
                player_weapons.num_grenades = player_weapons_defaults.num_grenades;
                for(int i=0; i < player_weapons_defaults.num_grenades; i++) {
                    player_weapons.grenade_choices[i] = player_weapons_defaults.grenade_choices[i];
                }
            }
            if(bots) {
                bot_weapons.num_grenades = bot_weapons_defaults.num_grenades;
                for(int i=0; i < bot_weapons_defaults.num_grenades; i++) {
                    bot_weapons.grenade_choices[i] = bot_weapons_defaults.grenade_choices[i];
                }
            }
            return true;
        }
    }
    return false;
}

public int Native_SetGunPreference(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    int slot = WEAPON_LOOKUP_FAIL;
    int idx = WEAPON_PREF_FAIL;
    int len;
    GetNativeStringLength(2, len);
    if(len <= 0)
        return false;
    char[] gun = new char[len+1+7];
    strcopy(gun, len+1+7, "weapon_");
    GetNativeString(2, gun[7], len+1);
    
    if(!(weapon_slots.GetValue(gun, slot) && (slot == 0 || slot == 1) && weapon_indexes.GetValue(gun, idx))) {
        idx = WEAPON_PREF_FAIL;
    }
    
    if(idx != WEAPON_PREF_FAIL) {
        if(client >= 0) {
            if(slot == 0) {
                player_data[client].guns.rifle = idx;
            } else {
                player_data[client].guns.pistol = idx;
            }
            return true;
        } else {
            bool bots = (client == EQUIP_BOTH) | (client == EQUIP_BOTS);
            bool players = (client == EQUIP_BOTH) | (client == EQUIP_PLAYERS);
            if(!bots && !players) {
                return false;
            }
            bool is_bot;
            for(int i=0; i < MaxClients; i++) {
                if(!IsClientInGame(i)) {
                    continue;
                }
                is_bot = IsFakeClient(i);
                if((is_bot && bots) || (!is_bot && players)) {
                    if(slot == 0) {
                        player_data[i].guns.rifle = idx;
                    } else {
                        player_data[i].guns.pistol = idx;
                    }
                }
            }
            return true;
        }
    }
    return false;
}

public int Native_SetSlotPreference(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    int slot = GetNativeCell(2);
    int index = GetNativeCell(3);
    return set_slot_pref(client, slot, index);
}

public int Native_GetSlotPreference(Handle plugin, int numParams) {
    bool client = GetNativeCell(1);
    int slot = GetNativeCell(2);
    switch(slot) {
        case 0: {
            return player_data[client].guns.rifle;
        }
        case 1: {
            return player_data[client].guns.pistol;
        }
        case 2: {
            
            if(player_data[client].equip.knife == NO_OVERRIDE) {
                return (!IsClientInGame(client) || IsFakeClient(client))? bot_equip.knife: player_equip.knife;
            } else {
                return player_data[client].equip.knife;
            }
        }
    }
    return WEAPON_PREF_FAIL;
}


public int Native_GetWeaponIndex(Handle plugin, int numParams) {
    int idx = WEAPON_LOOKUP_FAIL;
    int len;
    GetNativeStringLength(1, len);
    if(len <= 0)
        return false;
    char[] gun = new char[len+1+7];
    strcopy(gun, len+1+7, "weapon_");
    GetNativeString(1, gun[7], len+1);
    if(weapon_indexes.GetValue(gun, idx)) {
        return idx;
    }
    return WEAPON_LOOKUP_FAIL;
}

public int Native_GetWeaponSlot(Handle plugin, int numParams) {
    int slot = WEAPON_LOOKUP_FAIL;
    int len;
    GetNativeStringLength(1, len);
    if(len <= 0)
        return false;
    char[] gun = new char[len+1+7];
    strcopy(gun, len+1+7, "weapon_");
    GetNativeString(1, gun[7], len+1);
    if(weapon_slots.GetValue(gun, slot)) {
        return slot;
    }
    return WEAPON_LOOKUP_FAIL;
}

public int Native_FirstAvailableWeapon(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    int slot = GetNativeCell(2);
    bool player;
    switch(client) {
        case EQUIP_PLAYERS: {
            player = true;
        }
        case EQUIP_BOTS: {
            player = false;
        }
        default: {
            if(!IsClientInGame(client)) {
                return WEAPON_NONE;
            }
            player = !IsFakeClient(client);
        }
    }
    return first_available_weapon(player, slot);
}

public int Native_DisplayMainMenu(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    display_main_menu(client, 0);
}
public int Native_DisplayPrimaryMenu(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    display_primary_menu(client, 0);
}
public int Native_DisplaySecondaryMenu(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    display_secondary_menu(client, 0);
}
public int Native_UpdateUserWeapons(Handle plugin, int numParams) {
    bool is_bot;
    bool equip_lowest;
    int ent;
    bool removed[2];
    char buffer[32];
    char cur_weapon[32];
    for(int i=1; i < MaxClients; i++) {
        if(!IsClientInGame(i) || !IsPlayerAlive(i)) {
            continue;
        }
        
        // Update primary and secondary weapons
        equip_lowest = false;
        is_bot = IsFakeClient(i);
        GetClientWeapon(i, cur_weapon, sizeof(cur_weapon));
        
        for(int slot=0; slot < 2; slot++) {
            ent = GetPlayerWeaponSlot(i, slot);
            if(ent > 0 && IsValidEdict(ent)) {
                GetEdictClassname(ent, buffer, sizeof(buffer));
                
                if((is_bot && !bot_weapons.has_weapon(buffer)) || (!is_bot && !player_weapons.has_weapon(buffer))) {
                    removed[slot] = true;
                    RemovePlayerItem(i, ent);
                    RemoveEdict(ent);
                    if(StrEqual(cur_weapon, buffer)) {
                        equip_lowest = true;
                    }
                }
            }
        }
        
        // Update knife
        bool knife;
        if(!is_bot) {
            knife = should_have_knife(i, player_equip);
        } else {
            knife = should_have_knife(i, bot_equip);
        }
        ent = GetPlayerWeaponSlot(i, 2);
        if(ent > 0 && IsValidEdict(ent)) {
            if(!knife) {
                #if DEBUGGING
                    if(!is_bot) LogToFileEx(EquipLogFile, "%N has a knife but shouldn't - removing knife (should_have_knife=%i)", i, knife);
                #endif
                RemovePlayerItem(i, ent);
                RemoveEdict(ent);
                if(StrEqual(cur_weapon, "weapon_knife")) {
                    equip_lowest = true;
                }
            }
        } else if(knife) {
            #if DEBUGGING
                if(!is_bot) LogToFileEx(EquipLogFile, "%N doesn't have knife but should - giving knife (should_have_knife=%i)", i, knife);
            #endif
            GivePlayerItem(i, "weapon_knife");
            if(strlen(cur_weapon) == 0) {
                equip_lowest = true;
            }
        }
        
        // Update grenades
        
        int quantity;
        int expected;
        int ovr_quantity;
        int size = GetEntPropArraySize(i, Prop_Send, "m_hMyWeapons");
        for(int j=0; j < size; j++) {
            ent = GetEntPropEnt(i, Prop_Send, "m_hMyWeapons", j);
            if(ent > 0 && IsValidEntity(ent)) {
                GetEntityClassname(ent, buffer, sizeof(buffer));
                for(int g=0; g < sizeof(grenades); g++) {
                    if(StrEqual(buffer, grenades[g])) {
                        quantity = GetEntProp(i, Prop_Send, "m_iAmmo", 4, 11+g);
                        ovr_quantity = player_data[i].equip.grenade_quantity(grenades[g]);
                        expected = (!is_bot)? player_equip.grenade_quantity(grenades[g]): bot_equip.grenade_quantity(grenades[g]);
                        if(ovr_quantity != NO_OVERRIDE) {
                            expected = ovr_quantity;
                        }
                        if(expected == 0) {
                            RemovePlayerItem(i, ent);
                            RemoveEdict(ent);
                            if(StrEqual(cur_weapon, buffer)) {
                                equip_lowest = true;
                            }
                        } else if(quantity > expected) {
                            #if DEBUGGING
                                LogToFileEx(EquipLogFile, "Resetting offset=%i for %N to %i", 11+g, i, expected);
                            #endif
                            SetEntProp(i, Prop_Send, "m_iAmmo", expected, 4, 11+g);
                        }
                    }
                }
            }
        }
        if(equip_lowest) {
            EquipLowestSlot(i);
        }
        if(!is_bot) {
            bool show_primary = (removed[0] && player_weapons.num_primaries > 0);
            bool show_secondary = (removed[1] && player_weapons.num_secondaries > 0);
            if(show_primary && show_secondary) {
                player_data[i].menu_state.show_menu = ShowMenu_Primary;
                player_data[i].menu_state.next_menu = ShowMenu_Secondary;
            } else if(show_primary) {
                player_data[i].menu_state.show_menu = ShowMenu_Primary;
                player_data[i].menu_state.next_menu = ShowMenu_None;
            } else if(show_secondary) {
                player_data[i].menu_state.show_menu = ShowMenu_Secondary;
                player_data[i].menu_state.next_menu = ShowMenu_None;
            }
        }
        display_menu(i);
    }
}



// Sets reserve ammo amount for a gun.  Use SetGrenade() for grenades
public int Native_SetSlotAmmo(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    int slot = GetNativeCell(2);
    int amount = GetNativeCell(3);
    
    if(client == 0) {
        return 0;
    }
    if(client > 0) {
        return view_as<int>(player_data[client].guns.set_slot_ammo(client, slot, amount));
    }
    
    bool bots = client == EQUIP_BOTS || client == EQUIP_BOTH;
    bool players = client == EQUIP_PLAYERS || client == EQUIP_BOTH;
    bool is_bot;
    int affected = 0;
    for(int i=1; i < MaxClients; i++) {
        if(!IsClientInGame(i)) {
            continue;
        }
        is_bot = IsFakeClient(i);
        if((is_bot && bots) || (!is_bot && players)) {
            affected += view_as<int>(player_data[i].guns.set_slot_ammo(i, slot, amount));
        }
    }
    return affected;
}

public int Native_HasAnyInfiniteGrenades(Handle plugin, int numParams) {
    int client = GetNativeCell(1);
    return player_data[client].equip.has_any_infinite_grenades();
}

public int Native_CreateSnapshot(Handle plugin, int numParams) {
    int len;
    GetNativeStringLength(1, len);
    if(len <= 0)
        return 0;
    char[] name = new char[len+1];
    GetNativeString(1, name, len+1);
    create_snapshot(name);
    return 0;
}
public int Native_DeleteSnapshot(Handle plugin, int numParams) {
    int len;
    GetNativeStringLength(1, len);
    if(len <= 0)
        return 0;
    char[] name = new char[len+1];
    GetNativeString(1, name, len+1);
    return delete_snapshot(name);
}

public int Native_RestoreSnapshot(Handle plugin, int numParams) {
    int len;
    GetNativeStringLength(1, len);
    if(len <= 0)
        return 0;
    char[] name = new char[len+1];
    GetNativeString(1, name, len+1);
    return restore_snapshot(name);
}
// restores a snapshot and, if successful, deletes the snapshot
public int Native_RestoreDeleteSnapshot(Handle plugin, int numParams) {
    int len;
    GetNativeStringLength(1, len);
    if(len <= 0)
        return 0;
    char[] name = new char[len+1];
    GetNativeString(1, name, len+1);
    return restore_snapshot(name) && delete_snapshot(name);
}

