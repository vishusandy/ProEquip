#if defined _pro_equip_dm_config_included
    #endinput
#endif
#define _pro_equip_dm_config_included

#include <pro_equip/equip_pref.inc>

bool kv_bool(KeyValues kv, const char[] key, bool defaultvalue) {
    char value[8];
    KvGetString(kv, key, value, sizeof(value), (defaultvalue)? "yes": "no");
    return strcmp(value, "yes") == 0;
}

int kv_int_or_bool(KeyValues kv, const char[] key, int defaultvalue) {
    char defaultstring[8];
    IntToString(defaultvalue, defaultstring, sizeof(defaultstring));
    char value[8];
    KvGetString(kv, key, value, sizeof(value), defaultstring);
    if(StrEqual(value, "yes", false)) {
        return 1;
    } else if(StrEqual(value, "no", false)) {
        return 0;
    }
    int v = StringToInt(value);
    return v;
}

// loads default and map config
bool load_map_configs() {
    bool result = load_config("cfg/cssdm/cssdm.equip.txt");
    char map[64];
    char buffer[256];
    GetCurrentMap(map, sizeof(map));
    Format(buffer, sizeof(buffer), "cfg/cssdm/maps/%s.equip.txt", map);
    return result | load_config(buffer);
}

// loads a specified config file
bool load_config(const char[] config) {
    KeyValues kv = new KeyValues("Equipment");
    if(!kv.ImportFromFile(config)) {
        LogToFileEx(EquipLogFile, "Could not load %s", config);
        delete kv;
        return false;
    }
    LogToFileEx(EquipLogFile, "Processing config: %s", config);
    
    if(kv.JumpToKey("Settings")) {
        weapon_menu.weapon_menu_enabled = kv_bool(kv, "guns_command", weapon_menu.weapon_menu_enabled);
        kv.GoBack();
    }
    
    load_equipment(kv, player_equip);
    player_equip.build_random_grenades_list(player_weapons);
    copy_equip(player_equip, player_equip_defaults);
    
    load_bot_items(kv);
    bot_equip.build_random_grenades_list(bot_weapons);
    copy_equip(bot_equip, bot_equip_defaults);
    
    player_weapons.truncate_slot(0);
    player_weapons.truncate_slot(1);
    load_player_weapons(kv, "PrimaryMenu");
    load_player_weapons(kv, "SecondaryMenu");
    
    copy_available_weapons(player_weapons, player_weapons_defaults);
    copy_available_weapons(bot_weapons, bot_weapons_defaults);
    
    delete kv;
    return true;
}

bool load_equipment(KeyValues kv, EquipPref equip) {
    if(!kv.JumpToKey("AutoItems")) {
        return false;
    }
    equip.hp = kv.GetNum("health", equip.hp);
    equip.armor = kv.GetNum("armor", equip.armor);
    equip.hegrenades = kv_int_or_bool(kv, "hegrenade", equip.hegrenades);
    equip.flashbangs = kv.GetNum("flashbangs", equip.flashbangs);
    equip.smokegrenades = kv_int_or_bool(kv, "smokegrenade", equip.smokegrenades);
    equip.nightvision = (kv_bool(kv, "nightvision", (equip.nightvision == 0)? false: true))? 1: 0;
    equip.defusekit = kv_bool(kv, "defusekits", equip.defusekit);
    kv.GoBack();
    return true;
}

bool load_bot_items(KeyValues kv) {
    if(!kv.JumpToKey("BotItems")) {
        LogToFileEx(EquipLogFile, "No bot weapons/equipment found, loading defaults");
        copy_available_weapons(gun_menu_order, bot_weapons);
        return false;
    }
    if(!kv.GotoFirstSubKey(false)) {
        LogToFileEx(EquipLogFile, "BotItems first subkey not found, loading defaults");
        copy_available_weapons(gun_menu_order, bot_weapons);
        kv.GoBack();
        return false;
    }
    char buffer[255];
    bot_weapons.truncate_slot(0);
    bot_weapons.truncate_slot(1);
    do {
        kv.GetSectionName(buffer, sizeof(buffer));
        if(strcmp(buffer, "weapon") == 0) {
            buffer = "weapon_";
            kv.GetString(NULL_STRING, buffer[7], sizeof(buffer)-7);
            if(!bot_weapons.add_choice(buffer, false, -1)) {
                LogToFileEx(EquipLogFile, "Could not add BotItem choice %s", buffer);
            }
        } else if(strcmp(buffer, "health") == 0) {
            bot_equip.hp = kv.GetNum(NULL_STRING, bot_equip.hp);
        } else if(strcmp(buffer, "armor") == 0) {
            bot_equip.armor = kv.GetNum(NULL_STRING, bot_equip.armor);
        } else if(strcmp(buffer, "hegrenade") == 0) {
            bot_equip.hegrenades = kv.GetNum(NULL_STRING, bot_equip.hegrenades);
        } else if(strcmp(buffer, "flashbangs") == 0) {
            bot_equip.flashbangs = kv_bool(kv, NULL_STRING, bot_equip.flashbangs > 0);
        } else if(strcmp(buffer, "smokegrenade") == 0) {
            bot_equip.smokegrenades = kv_bool(kv, NULL_STRING, bot_equip.smokegrenades > 0);
        } else if(strcmp(buffer, "nightvision") == 0) {
            bot_equip.nightvision = kv_bool(kv, NULL_STRING, (bot_equip.nightvision == 0)? false: true);
        } else if(strcmp(buffer, "defusekits") == 0) {
            bot_equip.defusekit = kv_bool(kv, NULL_STRING, bot_equip.defusekit);
        }
    } while(kv.GotoNextKey(false));
    kv.GoBack();
    kv.GoBack();
    return true;
}


bool load_player_weapons(KeyValues kv, const char[] key) {
    if(!kv.JumpToKey(key)) {
        LogToFileEx(EquipLogFile, "Failed to find key %s, continuing", key);
        // LogToFileEx(EquipLogFile, "Failed to find key %s - loading defaults", key);
        // copy_available_weapons(gun_menu_order, player_weapons);
        return false;
    }
    if(!kv.GotoFirstSubKey(false)) {
        LogToFileEx(EquipLogFile, "Failed to find subkey %s, continuing", key);
        // LogToFileEx(EquipLogFile, "Failed to find subkey %s - loading defaults", key);
        // copy_available_weapons(gun_menu_order, player_weapons);
        kv.GoBack();
        return false;
    }
    char buffer[255];
    do {
        kv.GetSectionName(buffer, sizeof(buffer));
        if(strcmp(buffer, "weapon") != 0)
            continue;
        buffer = "weapon_"
        kv.GetString(NULL_STRING, buffer[7], sizeof(buffer)-7);
        if(!player_weapons.add_choice(buffer, true, -1)) {
            LogToFileEx(EquipLogFile, "Could not add weapon: %s", buffer);
        }
    } while(kv.GotoNextKey(false) && player_weapons.num_primaries < 32 && player_weapons.num_secondaries < 32);
    
    kv.GoBack();
    kv.GoBack();
    return true;
}
