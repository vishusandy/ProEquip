#if defined _pro_equip_weapons_included
    #endinput
#endif
#define _pro_equip_weapons_included

#include <pro_equip/vars.inc>
#include <pro_equip/weapon_utils.inc>

#undef REQUIRE_PLUGIN
#include <ProNightvision.inc>
#define REQUIRE_PLUGIN


/**
 * This file contains functions that retrieve/manipulate weapon preferences
 */


bool give_primary_pref(int client) {
    if(client <= 0 || !IsClientInGame(client) || !give_weapons) {
        return false;
    }
    if(!IsFakeClient(client)) {
        bool result =  give_weapon_index(client, primaries, sizeof(primaries), player_weapons.primary_choices, player_weapons.num_primaries, 0, player_data[client].guns.rifle);
        if(player_data[client].guns.rifle > WEAPON_NONE) {
            player_data[client].guns.give_rifle_ammo(client);
        }
        return result;
    } else {
        bool result = give_weapon_index(client, primaries, sizeof(primaries), bot_weapons.primary_choices, bot_weapons.num_primaries, 0, player_data[client].guns.rifle);
        if(player_data[client].guns.rifle > WEAPON_NONE) {
            player_data[client].guns.give_rifle_ammo(client);
        }
        return result;
    }
}

bool give_secondary_pref(int client) {
    if(client <= 0 || !IsClientInGame(client) || !give_weapons) {
        return false;
    }
    if(!IsFakeClient(client)) {
        bool result = give_weapon_index(client, secondaries, sizeof(secondaries), player_weapons.secondary_choices, player_weapons.num_secondaries, 1, player_data[client].guns.pistol);
        if(player_data[client].guns.pistol > WEAPON_NONE) {
            player_data[client].guns.give_pistol_ammo(client);
        }
        return result;
    } else {
        bool result = give_weapon_index(client, secondaries, sizeof(secondaries), bot_weapons.secondary_choices, bot_weapons.num_secondaries, 1, player_data[client].guns.pistol);
        if(player_data[client].guns.pistol > WEAPON_NONE) {
            player_data[client].guns.give_pistol_ammo(client);
        }
        return result;
    }
}

bool give_guns_pref(int client) {
    if(client <= 0 || !IsClientInGame(client) || !give_weapons) {
        return false;
    }
    bool guns = give_primary_pref(client) & give_secondary_pref(client);
    bool is_bot = IsFakeClient(client);
    bool knife = view_as<bool>((player_data[client].equip.knife != NO_OVERRIDE)? player_data[client].equip.knife: (is_bot)? bot_equip.knife: player_equip.knife);
    bool nades = (has_grenade_overrides(client))? player_data[client].equip.has_grenades(): (is_bot)? bot_equip.has_grenades(): player_equip.has_grenades();
    // if(!guns && !knife && !nades) {} else { reset_infinite_grenades(client); }
    if(guns || knife || nades) {
        reset_infinite_grenades(client);
    }
    return guns;
}



// Give a weapon by its index into the associated weapon array; enables giving random weapons as well
bool give_weapon_index(int client, char[][] weapon_list, int weapon_size, int[] rand_list, int rand_size, int slot, int index) {
    if(index == WEAPON_NONE || index == WEAPON_RESET || !give_weapons || weapon_size <= 0 || slot == 2) {
        return false;
    }
    if(index >= 0 && index < weapon_size) { // a specified weapon
        give_weapon(client, slot, weapon_list[index]);
        return true;
    }
    if(index == RANDOM_WEAPON_ALL) {    // random from weapon_list (all possible weapons)
        if(slot == 3) {
            give_weapon(client, slot, grenades[player_data[client].random_grenade(true)]);
        } else {
            give_weapon(client, slot, weapon_list[GetURandomInt() % weapon_size]);
        }
        return true;
    }
    if(index == RANDOM_WEAPON_AVAILABLE && rand_size > 0) {   // random from rand_list
        if(slot == 3) {
            give_weapon(client, slot, grenades[player_data[client].random_grenade(false)]);
        } else {
            int rand = rand_list[GetURandomInt() % rand_size];
            give_weapon(client, slot, weapon_list[rand]);
        }
        return true;
    }
    return false;
}


bool reset_infinite_grenades(int client) {
    EquipPref player;
    player = player_data[client].equip;
    
    EquipPref equip;
    equip = (IsFakeClient(client))? bot_equip: player_equip;
    
    if(player.hegrenades == INFINITE_AMMO) {
        player.hegrenades = equip.hegrenades;
    }
    if(player.flashbangs == INFINITE_AMMO) {
        player.flashbangs = equip.flashbangs;
    }
    if(player.smokegrenades == INFINITE_AMMO) {
        player.smokegrenades = equip.smokegrenades;
    }
}

bool has_grenade_overrides(int client) {
    EquipPref equip;
    equip = (IsFakeClient(client))? bot_equip: player_equip;
    return (player_data[client].equip.hegrenades != NO_OVERRIDE || equip.hegrenades > 0)
            || (player_data[client].equip.flashbangs != NO_OVERRIDE || equip.flashbangs > 0)
            || (player_data[client].equip.smokegrenades != NO_OVERRIDE || equip.smokegrenades > 0);
}

bool should_have_knife(int client, EquipPref equip) {
    if(player_data[client].equip.knife == NO_OVERRIDE) {
        return equip.knife == 1;
    } else {
        return player_data[client].equip.knife == 1;
    }
}

bool set_knife(int client, bool on) {
    if(client == EQUIP_PLAYERS) {
        player_equip.knife = on;
        set_slot_pref(client, 2, on);
        return true;
    } else if(client == EQUIP_BOTS) {
        bot_equip.knife = on;
        set_slot_pref(client, 2, on);
        return true;
    } else if(client == EQUIP_BOTH) {
        player_equip.knife = on;
        bot_equip.knife = on;
        set_slot_pref(client, 2, on);
        return true;
    }
    player_data[client].equip.knife = on;
    if(on) {
        return give_weapon(client, 2, "weapon_knife");
    } else {
        int ent = GetPlayerWeaponSlot(client, 2);
        if(ent > 0 && IsValidEdict(ent)) {
            RemovePlayerItem(client, ent);
            RemoveEdict(ent);
            return true;
        }
    }
    return false;
}

int first_available_weapon(bool player, int slot) {
    if(player) {
        switch(slot) {
            case 0: {
                return (player_weapons.num_primaries > 0)? player_weapons.primary_choices[0]: WEAPON_RESET;
            }
            case 1: {
                return (player_weapons.num_secondaries > 0)? player_weapons.secondary_choices[0]: WEAPON_RESET;
            }
            default: {
                return WEAPON_RESET;
            }
        }
    } else {
        switch(slot) {
            case 0: {
                return (bot_weapons.num_primaries > 0)? bot_weapons.primary_choices[0]: WEAPON_RESET;
            }
            case 1: {
                return (bot_weapons.num_secondaries > 0)? bot_weapons.secondary_choices[0]: WEAPON_RESET;
            }
            default: {
                return WEAPON_RESET;
            }
        }
    }
}


int remove_player_weapon_from_all(int idx, int slot, bool players, bool bots) {
    bool is_bot;
    int player_reset_index = WEAPON_RESET;
    int bot_reset_index = WEAPON_RESET;
    if(slot ==0 ) {
        if(idx == 0 && bot_weapons.num_primaries < 2) {
            bot_reset_index = WEAPON_RESET;
        } else {
            // bot_reset_index = bot_weapons.primary_choices[0];
            bot_reset_index = RANDOM_WEAPON_AVAILABLE;
        }
        if(idx == 0 && player_weapons.num_primaries < 2) {
            player_reset_index = WEAPON_RESET;
        } else {
            player_reset_index = player_weapons.primary_choices[0];
        }
    } else {
        if(idx == 0 && bot_weapons.num_secondaries < 2) {
            bot_reset_index = WEAPON_RESET;
        } else {
            // bot_reset_index = bot_weapons.secondary_choices[0];
            bot_reset_index = RANDOM_WEAPON_AVAILABLE;
        }
        if(idx == 0 && player_weapons.num_secondaries < 2) {
                player_reset_index = WEAPON_RESET;
        } else {
            player_reset_index = player_weapons.secondary_choices[0];
        }
    }
    int reset_idx;
    int removed = 0;
    char weapon[32];
    for(int i=1; i < MaxClients; i++) {
        if(!IsClientInGame(i)) {
            continue;
        }
        is_bot = IsFakeClient(i);
        reset_idx = (!is_bot)? player_reset_index: bot_reset_index;
        if((!is_bot && players) || (is_bot && bots)) {
            char buff[32];
            if(slot == 0) {
                if(player_data[i].guns.rifle == idx) {
                    player_data[i].guns.rifle = reset_idx;
                    int ent = GetPlayerWeaponSlot(i, slot);
                    GetClientWeapon(i, weapon, sizeof(weapon));
                    if(ent > 0 && IsValidEdict(ent)) {
                        GetEntityClassname(ent, buff, sizeof(buff));
                        if(strcmp(buff, primaries[idx]) == 0) {
                            RemovePlayerItem(i, ent);
                            RemoveEdict(ent);
                            removed++;
                            if(StrEqual(weapon, buff)) {
                                EquipLowestSlot(i);
                            }
                        }
                    }
                }
            } else {
                if(player_data[i].guns.pistol == idx) {
                    player_data[i].guns.pistol = reset_idx;
                    int ent = GetPlayerWeaponSlot(i, slot);
                    GetClientWeapon(i, weapon, sizeof(weapon));
                    if(ent > 0 && IsValidEdict(ent)) {
                        GetEntityClassname(ent, buff, sizeof(buff));
                        if(strcmp(buff, secondaries[idx]) == 0) {
                            RemovePlayerItem(i, ent);
                            RemoveEdict(ent);
                            removed++;
                            if(StrEqual(weapon, buff)) {
                                EquipLowestSlot(i);
                            }
                        }
                    }
                }
            }
        }
    }
    return removed;
}

int remove_player_weapon_slot(bool players, bool bots, int slot, bool remove_knife=false) {
    int is_bot;
    int removed = 0;
    char weapon[32];
    char slot_buff[32];
    for(int i=1; i < MaxClients; i++) {
        if(!IsClientInGame(i)) {
            continue;
        }
        is_bot = IsFakeClient(i);
        if((!bots && is_bot) || (!players && !is_bot)) {
            continue;
        }
        for(int s=0; s < 4; s++) {
            if(s==2 && !remove_knife) {
                s++;
                continue;
            }
            if(slot == -1 || slot == s) {
                int ent = GetPlayerWeaponSlot(i, s);
                if(ent > 0 && IsValidEdict(ent)) {
                    GetClientWeapon(i, weapon, sizeof(weapon));
                    GetEntityClassname(ent, slot_buff, sizeof(slot_buff));
                    RemovePlayerItem(i, ent);
                    RemoveEdict(ent);
                    removed++;
                    if(StrEqual(weapon, slot_buff)) {
                        EquipLowestSlot(i);
                    }
                }
                if(s == 0) {
                    player_data[i].guns.rifle = WEAPON_RESET;
                } else if(s == 1) {
                    player_data[i].guns.pistol = WEAPON_RESET;
                }
            }
        }
    }
    return removed;
}

// for knife slot (slot 2) index is whether to turn it on or off (0=off, 1=on)
bool set_slot_pref(int client, int slot, int index) {
    bool players = false;
    bool bots = false;
    switch(client) {
        case EQUIP_PLAYERS: {
            players = true;
        }
        case EQUIP_BOTS: {
            bots = true;
        }
        case EQUIP_BOTH: {
            players = true;
            bots = true;
        }
        default: {
            switch(slot) {
                case 0: {
                    player_data[client].guns.rifle = index;
                    return true;
                }
                case 1: {
                    player_data[client].guns.pistol = index;
                    return true;
                }
                case 2: {
                    player_data[client].equip.knife = index;
                }
                default: {
                    return false;
                }
            }
            return false
        }
    }
    for(int i=1; i < MaxClients; i++) {
        if(!IsClientInGame(i)) { continue; }
        bool is_bot = IsFakeClient(i);
        if((is_bot && bots) || (!is_bot && players)) {
            switch(slot) {
                case 0: {
                    player_data[i].guns.rifle = index;
                }
                case 1: {
                    player_data[i].guns.pistol = index;
                }
                case 2: {
                    player_data[i].equip.knife = index;
                }
            }
            return false
        }
    }
    return true;
}

int slot_size(int slot, bool bots=false) {
    AvailableWeapons w;
    w = (!bots)? player_weapons: bot_weapons;
    switch(slot) {
        case 0: {
            return w.num_primaries;
        }
        case 1: {
            return w.num_secondaries;
        }
        case 2: {
            EquipPref e;
            e = (!bots)? player_equip: bot_equip;
            return e.knife == 1;
        }
        case 3: {
            return w.num_grenades;
        }
        default: {
            return -1;
        }
    }
}

void reset_invalid_weapon_prefs(int client, int players_reset_to=RANDOM_WEAPON_AVAILABLE, int bots_reset_to=RANDOM_WEAPON_AVAILABLE) {
    if(client >= 0) {
        return;
    }
    bool players, bots;
    bots_players_targets(client, players, bots);
    for(int i=1; i < MaxClients; i++) {
        if(!IsClientInGame(i)) {
            continue;
        }
        bool is_bot = IsFakeClient(i);
        int reset_to = (is_bot)? bots_reset_to: players_reset_to;
        if((is_bot && bots) || (!is_bot && players)) {
            if(player_data[i].guns.rifle == WEAPON_RESET) {
                player_data[i].guns.rifle = reset_to;
            }
            if(player_data[i].guns.pistol == WEAPON_RESET) {
                player_data[i].guns.pistol = reset_to;
            }
        }
    }
}

void initialize_weapon_array() {
    for(int i; i < sizeof(primaries); i++) {
        gun_menu_order.add_choice(primaries[i], true);
    }
    for(int i; i < sizeof(secondaries); i++) {
        gun_menu_order.add_choice(secondaries[i], true);
    }
    for(int i; i < sizeof(grenades); i++) {
        gun_menu_order.add_choice(grenades[i], true);
    }
}

void initialize_weapon_hashmaps() {
    for(int i=0; i < sizeof(primaries); i++) {
        weapon_indexes.SetValue(primaries[i], i);
        weapon_slots.SetValue(primaries[i], 0);
    }
    for(int i=0; i < sizeof(secondaries); i++) {
        weapon_indexes.SetValue(secondaries[i], i);
        weapon_slots.SetValue(secondaries[i], 1);
    }
    weapon_indexes.SetValue("weapon_knife", 0);
    weapon_slots.SetValue("weapon_knife", 2);
    for(int i=0; i < sizeof(grenades); i++) {
        weapon_indexes.SetValue(grenades[i], i);
        weapon_slots.SetValue(grenades[i], 3);
    }
}

void initialize_equip_defaults() {
    player_equip.hp = DEFAULT_HP;
    player_equip.armor = DEFAULT_ARMOR;
    player_equip.knife = DEFAULT_KNIFE;
    player_equip.hegrenades = DEFAULT_HEGRENADES;
    player_equip.flashbangs = DEFAULT_FLASHBANGS;
    player_equip.smokegrenades = DEFAULT_SMOKEGRENADES;
    player_equip.nightvision = DEFAULT_NIGHTVISION;
    player_equip.defusekit = DEFAULT_DEFUSEKIT;
    player_equip.silencers = DEFAULT_SILENCERS;
    
    bot_equip.hp = DEFAULT_HP;
    bot_equip.armor = DEFAULT_ARMOR;
    bot_equip.knife = DEFAULT_KNIFE;
    bot_equip.hegrenades = DEFAULT_HEGRENADES;
    bot_equip.flashbangs = DEFAULT_FLASHBANGS;
    bot_equip.smokegrenades = DEFAULT_SMOKEGRENADES;
    bot_equip.nightvision = DEFAULT_NIGHTVISION;
    bot_equip.defusekit = DEFAULT_DEFUSEKIT;
    bot_equip.silencers = DEFAULT_SILENCERS;
}

public void initial_weapon_prefs() {
    GunPref players;
    GunPref bots;
    first_weapons(players, bots);
    reset_weapon_prefs(players, bots);
    reset_invalid_weapon_prefs(EQUIP_BOTH, RANDOM_WEAPON_AVAILABLE, RANDOM_WEAPON_AVAILABLE);
}

public void first_weapons(GunPref players, GunPref bots) {
    players.rifle = first_available_weapon(true, 0);
    players.pistol = first_available_weapon(true, 1);
    bots.rifle = first_available_weapon(false, 0);
    bots.pistol = first_available_weapon(false, 1);
}

public void reset_weapon_prefs(GunPref players, GunPref bots) {
    bool is_bot;
    for(int i = 1; i < MaxClients; i++) {
        is_bot = !IsClientInGame(i) || IsFakeClient(i);
        player_data[i].guns.rifle = (is_bot)? bots.rifle: players.rifle;
        player_data[i].guns.pistol = (is_bot)? bots.pistol: players.pistol;
    }
}


bool show_nightvision(int client) {
    return display_nightvision(client, player_data[client].equip.nightvision > 0);
}

public Action TimerShowNightVision(Handle timer, int client) {
    show_nightvision(client);
}

bool equip_client(int client) {
    if(client <= 0 || !IsClientInGame(client) || GetClientTeam(client) <= CS_TEAM_SPECTATOR) {
        return false;
    }
    EquipPref e;
    bool is_bot = IsFakeClient(client);
    if(is_bot) {
        e = bot_equip;
    } else {
        e = player_equip;
    }
    int hp = (player_data[client].equip.hp != NO_OVERRIDE)? player_data[client].equip.hp: e.hp;
    int armor = (player_data[client].equip.armor != NO_OVERRIDE)? player_data[client].equip.armor: e.armor;
    bool knife = (player_data[client].equip.knife != NO_OVERRIDE)? player_data[client].equip.knife == 1: e.knife == 1;
    int nightvision = e.nightvision > 0 || player_data[client].equip.nightvision > 0;
    if(hp > 0) {
        SetEntProp(client, Prop_Send, "m_iHealth", hp);
    }
    if(armor > 0) {
        GivePlayerItem(client, "item_kevlar");
        // GivePlayerItem(client, "item_kevlar"); // does this need to be in here twice????
        SetEntProp(client, Prop_Send, "m_ArmorValue", armor);
    }
    if(nightvision) {
        SetEntProp(client, Prop_Send, "m_bHasNightVision", true);
    }
    if(player_equip.defusekit && GetClientTeam(client) == CS_TEAM_CT) {
        GivePlayerItem(client, "item_defuser");
    }
    if(give_weapons) {
        if(!knife){
            strip_slot(client, 2);
        } else {
            if(player_data[client].guns.pistol == WEAPON_NONE && player_data[client].guns.rifle == WEAPON_NONE) {
                ClientCommand(client, "slot3");
            }
        }
        
        int he = (player_data[client].equip.hegrenades != NO_OVERRIDE || player_data[client].equip.hegrenades == INFINITE_AMMO)? ((player_data[client].equip.hegrenades == INFINITE_AMMO)? 1: player_data[client].equip.hegrenades): e.hegrenades;
        int flash = (player_data[client].equip.flashbangs != NO_OVERRIDE || player_data[client].equip.flashbangs == INFINITE_AMMO)? ((player_data[client].equip.flashbangs == INFINITE_AMMO)? 1: player_data[client].equip.flashbangs): e.flashbangs;
        int smoke = (player_data[client].equip.smokegrenades != NO_OVERRIDE || player_data[client].equip.smokegrenades == INFINITE_AMMO)? ((player_data[client].equip.smokegrenades == INFINITE_AMMO)? 1: player_data[client].equip.smokegrenades): e.smokegrenades;
        if(!knife && player_data[client].guns.rifle == WEAPON_NONE && player_data[client].guns.pistol == WEAPON_NONE) {
            if(he > 1 && flash == 0 && smoke == 0) {
                he = 1;
            } else if(he == 0 && flash > 1 && smoke == 0) {
                flash = 1;
            } else if(he == 0 && flash == 0 && smoke > 1) {
                smoke = 1;
            }
        }
        
        give_grenades(client, "weapon_smokegrenade", smoke, true, true);
        give_grenades(client, "weapon_flashbang", flash, true, true);
        give_grenades(client, "weapon_hegrenade", he, true, true);
        if(player_data[client].guns.pistol == WEAPON_NONE && player_data[client].guns.rifle == WEAPON_NONE && (he > 0 || flash > 0 || smoke > 0)) {
            ClientCommand(client, "slot4");
        }
    }
    return true;
}

