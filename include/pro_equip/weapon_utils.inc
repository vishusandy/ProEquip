#if defined _pro_weapon_utils_included
    #endinput
#endif
#define _pro_weapon_utils_included

/*  weapon_utils.inc
        This file contains convenience functions to modify a player's weapons.
        These are intended to be used in Equip as well as in other plugins just 
        by including this file.
        All functions in this file should be standalone functions that DO NOT 
        require any other dependencies on Equip.
 */


// Give a specified weapon (e.g. weapon_deagle).  This ensures an existing weapon is removed before giving a new one
stock bool give_weapon(int client, int slot, const char[] weapon) {
    if(!IsClientInGame(client) || !IsPlayerAlive(client)) {
        return false;
    }
    int gun = GetPlayerWeaponSlot(client, slot);
    if(gun > 0 && IsValidEdict(gun)) {
        RemovePlayerItem(client, gun);
        RemoveEdict(gun);
    }
    bool result = GivePlayerItem(client, weapon) != -1;
    return result;
}

// Give a weapon with no ammo
stock bool give_dummy_weapon(int client, int slot, const char[] weapon) {
    if(slot < 0 || slot > 1 || !give_weapon(client, slot, weapon)) {
        return false;
    }
    if(slot == 0) {
        CreateTimer(0.1, delayed_primary_dummy_ammo, client);
    }
    if(slot == 1) {
        CreateTimer(0.1, delayed_secondary_dummy_ammo, client);
    }
    return give_ammo(client, slot, 0) & give_reserve_ammo(client, slot, 0);
}

public Action delayed_primary_dummy_ammo(Handle timer, int client) {
    give_ammo(client, 0, 0);
    give_reserve_ammo(client, 0, 0);
}

public Action delayed_secondary_dummy_ammo(Handle timer, int client) {
    give_ammo(client, 1, 0);
    give_reserve_ammo(client, 1, 0);
}

// use `GiveGrenades()` for setting grenade ammo
stock bool give_ammo(int client, int slot, int amount) {
    if(amount < 0 || slot < 0 || slot > 1) {
        return false;
    }
    int ent = GetPlayerWeaponSlot(client, slot);
    if(!IsValidEdict(ent)) {
        LogAction(-1, -1, "invalid edict for give_ammo");
        return false;
    }
    SetEntProp(ent, Prop_Data, "m_iClip1", amount);
    return true;
}

stock bool give_reserve_ammo(int client, int slot, int amount) {
    if(amount < 0 || slot < 0 || slot > 1) {
        return false;
    }
    int ent = GetPlayerWeaponSlot(client, slot);
    if(!IsValidEdict(ent)) {
        LogAction(-1, -1, "invalid edict for give_reserve_ammo");
        return false;
    }
    int offset_ammo = FindDataMapInfo(client, "m_iAmmo");
    int ammo_type = GetEntProp(ent, Prop_Data, "m_iPrimaryAmmoType");
    int offset = offset_ammo + (ammo_type * 4);
    SetEntData(client, offset, amount, 4, true);
    return true;
}

// Increase ammo in slot by one
stock bool inc_ammo(int client, int slot) {
    int ent = GetPlayerWeaponSlot(client, slot);
    int amount = GetEntProp(ent, Prop_Data, "m_iClip1") + 1;
    if(slot < 0 || !IsValidEdict(ent)) {
        return false;
    }
    SetEntProp(ent, Prop_Data, "m_iClip1", amount);
    return true;
}

stock int EquipLowestSlot(int client) {
    int ent = -1;
    for(int i=0; i < 4; i++) {
        ent = GetPlayerWeaponSlot(client, i);
        if(ent > MaxClients && IsValidEntity(ent)) {
            ClientCommand(client, "slot%i", i+1);
            return i;
        }
    }
    return -1;
}

stock Action EquipLowestSlotDelayed(Handle event, int client) {
    EquipLowestSlot(client);
}

stock bool has_weapon_slot(int client, int slot) {
    int ent = GetPlayerWeaponSlot(client, slot);
    return ent > 0 && IsValidEdict(ent);
}

stock void strip_grenades(int client) {
    for(int i=0; i < 3; i++) {
        if(!strip_slot(client, 3)) {
            break;
        }
    }
}

stock void strip_all_weapons(int client) {
    strip_slot(client, 0);
    strip_slot(client, 1);
    for(int i=0; i < 2; i++) {
        if(!strip_slot(client, 1))
            break;
    }
    strip_slot(client, 3);
}
stock bool strip_slot(int client, int slot) {
    int ent = GetPlayerWeaponSlot(client, slot);
    if(ent > 0 && IsValidEdict(ent)) {
        RemovePlayerItem(client, ent);
        RemoveEntity(ent);
        return true;
    }
    return false;
}

stock bool display_nightvision(int client, bool on) {
    if(GetEntProp(client, Prop_Send, "m_bHasNightVision") == 0) {
        return false;
    }
    if(on) {
        SetEntProp(client, Prop_Send, "m_bNightVisionOn", 1);
    } else {
         SetEntProp(client, Prop_Send, "m_bNightVisionOn", 0);
    }
    return true;
}

stock int weapon_silenced(int client, int slot) {
    int ent = GetPlayerWeaponSlot(client, slot);
    if(ent > 0 && IsValidEdict(ent)) {
        char buffer[32];
        GetEntityClassname(ent, buffer, sizeof(buffer));
        if(strcmp(buffer, "weapon_m4a1") == 0 || strcmp(buffer, "weapon_usp") == 0) {
            int silenced = GetEntProp(ent, Prop_Send, "m_bSilencerOn");
            return silenced;
        }
    } else {
        LogAction(-1, -1, "%N invalid edict", client);
    }
    return -1;
}

stock bool give_silencers(int client, int slot, bool on) {
    int ent = GetPlayerWeaponSlot(client, slot);
    
    if(ent > 0 && IsValidEdict(ent)) {
        char weapon[32];
        GetEdictClassname(ent, weapon, sizeof(weapon));
        if((slot == 0 && strcmp(weapon, "weapon_m4a1") == 0) || (slot == 1 && strcmp(weapon, "weapon_usp") == 0)) {
            SetEntProp(ent, Prop_Send, "m_bSilencerOn", on);
            SetEntProp(ent, Prop_Send, "m_weaponMode", on);
            return true;
        }
    }
    return false;
}


stock int grenade_offset(const char[] weapon) {
    int offset;
    if(StrEqual(weapon, "hegrenade") || StrEqual(weapon[7], "hegrenade")) {
        offset = 11;
    } else if(StrEqual(weapon, "flashbang") || StrEqual(weapon[7], "flashbang")) {
        offset = 12;
    } else if(StrEqual(weapon, "smokegrenade") || StrEqual(weapon[7], "smokegrenade")) {
        offset = 13;
    } else {
        ThrowError("Incorrect grenade '%s'", weapon);
    }
    return offset;
}

stock bool inc_grenade_ammo(int client, const char[] weapon) {
    int offset = grenade_offset(weapon);
    int ammo = GetEntProp(client, Prop_Send, "m_iAmmo", 4, offset);
    SetEntProp(client, Prop_Send, "m_iAmmo", ammo+1, 4, offset);
    return true;
}

stock int grenade_quantity(int client, const char[] weapon) {
    int offset = grenade_offset(weapon);
    return GetEntProp(client, Prop_Send, "m_iAmmo", 4, offset);
}


// Takes a grenade classname like 'weapon_hegrenade'
stock bool replace_grenade(int client, const char[] weapon) {
    if(grenade_quantity(client, weapon) > 0) {
        remove_specific_grenade(client, weapon);
    }
    GivePlayerItem(client, weapon);
}

// Takes an weapon classname, e.g. 'weapon_hegrenade'
stock bool give_grenades(int client, const char[] weapon, int num, bool give_item=true, bool ignore_initial_ammo=false) {
    if(num <= 0) {
        return true;
    }
    int offset = grenade_offset(weapon[7]);
    int ammo = GetEntProp(client, Prop_Send, "m_iAmmo", 4, offset);
    if((ammo == 0 || ignore_initial_ammo) && num > 0 && give_item == true) {
        GivePlayerItem(client, weapon);
    }
    SetEntProp(client, Prop_Send, "m_iAmmo", num, 4, offset);
    return true;
}

stock Handle give_grenades_delayed(int client, int grenade, int num, float delay, bool give=true) {
    DataPack pack;
    Handle h = CreateDataTimer(delay, GiveGrenadesDelayed, pack);
    pack.WriteCell(client);
    pack.WriteCell(grenade);
    pack.WriteCell(num);
    pack.WriteCell(give);
    return h;
}

stock Action GiveGrenadesDelayed(Handle event, DataPack pack) {
    int client;
    int grenade;
    int num;
    bool give;
    pack.Reset();
    client = pack.ReadCell();
    grenade = pack.ReadCell();
    num = pack.ReadCell();
    give = pack.ReadCell();
    give_grenades(client, grenades[grenade], num, give);
}


// Returns the number of items removed.  Takes a weapon classname, e.g. 'weapon_hegrenade'
stock int remove_specific_grenade(int client, const char[] weapon) {
    int removed = 0;
    int size = GetEntPropArraySize(client, Prop_Send, "m_hMyWeapons");
    char WeaponName[64];
    for (int i = 0; i < size; i++) {
        int weaponEntity = GetEntPropEnt(client, Prop_Send, "m_hMyWeapons", i);
        if(weaponEntity > 0 && IsValidEntity(weaponEntity)) {
            GetEntityClassname(weaponEntity, WeaponName, sizeof(WeaponName));
            if(StrEqual(weapon, WeaponName)) {
                RemovePlayerItem(client, weaponEntity);
                RemoveEntity(weaponEntity);
                removed++;
            }
        }
    }
    return removed;
}


stock bool bots_players_targets (int target, bool &players, bool &bots) {
    switch(target) {
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
            return false;
        }
    }
    return true;
}
