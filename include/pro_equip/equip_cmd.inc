#if defined _pro_equip_equip_cmd_included
    #endinput
#endif
#define _pro_equip_equip_cmd_included

#include <pro_equip/utils.inc>
#include <pro_equip/weapon_utils.inc>
#include <pro_equip/ProEquip.inc>

void equip_plugin_start() {
    RegAdminCmd("equip", Command_Equip, ADM_LEVEL, "Set equipment and weapons");
}


enum equipTarget{
    Equip_Players = 0,
    Equip_Bots,
    Equip_Both,
    Equip_Targets,
}
enum equipStats {
    Equip_HP = 0,
    Equip_Armor,
    Equip_Hegrenades,
    Equip_Flashbangs,
    Equip_Smokegrenades,
}

enum errorType {
    Error_None = 0,
    Error_ActionFailed,
}

enum equipActionType {
    EquipAction_None,
    EquipAction_Equip,
    EquipAction_WeaponAdd,
    EquipAction_WeaponRemove,
    EquipAction_Reset,
    // Todo: future options?
    // EquipAction_PrefRifle,
    // EquipAction_PrefPistol,
    // EquipAction_MenuRifle,
    // EquipAction_MenuPistol,
    // EquipAction_ShowMain,
}
enum struct EquipAction {
    equipActionType type;
    char target[32];
    char param[32];
    int amount;
    void reset() {
        this.type = EquipAction_None;
        // this.target = ""; // DON'T RESET TARGET - it won't change
        this.param = "";
        this.amount = 0;
    }
    bool process(int client) {
        if(StrEqual(this.target, "all", false) || StrEqual(this.target, "both", false)) {
            return this.process_client(client, EQUIP_BOTH);
        } else if(StrEqual(this.target, "players", false)) {
            return this.process_client(client, EQUIP_PLAYERS);
        } else if(StrEqual(this.target, "bots", false)) {
            return this.process_client(client, EQUIP_BOTS);
        } else {
            bool result = true;
            char name_buffer[MAX_NAME_LENGTH];
            int targets[MAXPLAYERS+1];
            bool ml = false;
            int count = ProcessTargetString(this.target, client, targets, sizeof(targets), COMMAND_FILTER_CONNECTED, name_buffer, sizeof(name_buffer), ml);
            if(count == 0) {
                return false;
            }
            int processed = 0;
            for(int i=0; i < count; i++) {
                bool res = this.process_client(client, targets[i]);
                result = result & res;
                processed += (res)? 1: 0;
            }
            return result;
        }
    }
    bool process_client(int client, int target) {
        switch(this.type) {
            case EquipAction_Equip: {
                return this.modify_equipment(client, target);
            }
            case EquipAction_WeaponAdd, EquipAction_WeaponRemove: {
                bool remove = (this.type == EquipAction_WeaponRemove)? true: false;
                // todo: if this.param has an '=' in it and the first part is a grenade then add/subtract that quantity
                return this.modify_weapons(client, target, remove);
            }
            case EquipAction_Reset: {
                return this.reset_overrides(client, target);
            }
        }
        return false;
    }
    bool modify_equipment(int client, int target) {
        if(StrEqual(this.param, "hp", false)) {
            ProEquip_SetEquipHP(target, clamp(this.amount, 0, 250));
            return true;
        } else if(StrEqual(this.param, "armor", false)) {
            ProEquip_SetEquipArmor(target, clamp(this.amount, 0, 250));
            return true;
        } else if(StrEqual(this.param, "he", false) || StrEqual(this.param, "hegrenade", false)) {
            int amount = clamp(this.amount, NO_OVERRIDE, MAX_GRENADES);
            return ProEquip_OverrideGrenades(target, "hegrenade", amount);
        } else if(StrEqual(this.param, "flash", false) || StrEqual(this.param, "flashbang", false)) {
            int amount = clamp(this.amount, NO_OVERRIDE, MAX_GRENADES);
            return ProEquip_OverrideGrenades(target, "flashbang", amount);
        } else if(StrEqual(this.param, "smoke", false) || StrEqual(this.param, "smokegrenade", false)) {
            int amount = clamp(this.amount, NO_OVERRIDE, MAX_GRENADES);
            return ProEquip_OverrideGrenades(target, "smokegrenade", amount);
        } else {
            ReplyToCommand(client, "Equip: '%s' is not a recognized attribute", this.param);
            return false;
        }
    }
    bool modify_weapons(int client, int target, bool remove) {
        if(StrEqual(this.param, "all", false)) {
            return this.set_all(client, target, remove);
        } else if(StrEqual(this.param, "grenades", false)) {
            return this.set_grenade_slot(client, target, remove);
        } else if(StrEqual(this.param, "rifles", false)) {
            return this.set_gun_slot(client, target, remove, 0);
        } else if(StrEqual(this.param, "pistols", false)) {
            return this.set_gun_slot(client, target, remove, 1);
        } else if(StrEqual(this.param, "knife", false)) {
            return this.set_knife(client, target, remove);
        } else {
            return this.set_weapon_availability(client, target, remove);
        }
    }
    bool reset_overrides(int client, int target) {
        if(client < 0) {
            bool players, bots;
            bots_players_targets(target, players, bots);
            this.reset_weapons(client, players, bots);
        }
        ProEquip_ResetEquip(target);
        return true;
    }
    bool set_weapon_availability(int client, int target, bool remove) {
        int slot = ProEquip_GetWeaponSlot(this.param);
        if(slot == WEAPON_LOOKUP_FAIL) {
            ReplyToCommand(client, "Invalid weapon '%s'", this.param);
            return false;
        } else if(slot == 3) {
            int add_num = (target < 0)? NO_OVERRIDE: NO_OVERRIDE;
            return ProEquip_SetGrenade(target, this.param, (remove)? 0: add_num);
        } else {
            if(target >= 0) {
                ReplyToCommand(client, "Cannot set per-player weapon availabilty");
            }
            bool players = (target == EQUIP_BOTH || target == EQUIP_PLAYERS)? true: false;
            bool bots = (target == EQUIP_BOTH || target == EQUIP_BOTS)? true: false;
            return ProEquip_SetAvailableWeapon(this.param, players, bots, remove);
        }
    }
    bool set_knife(int client, int target, bool remove) {
        return ProEquip_SetKnife(target, !remove);
    }
    bool set_gun_slot(int client, int target, bool remove, int slot) {
        if(target >= 0) {
            ReplyToCommand(client, "Cannot set per-player weapon availability");
        }
        bool players = (target == EQUIP_BOTH || target == EQUIP_PLAYERS)? true: false;
        bool bots = (target == EQUIP_BOTH || target == EQUIP_BOTS)? true: false;
        if(!remove) {
            return ProEquip_ResetWeaponSlotAvailability(players, bots, slot);
        } else {
            return ProEquip_ResetSlotPreference(players, bots, slot, false);
        }
    }
    bool set_grenade_slot(int client, int target, bool remove) {
        if(!remove) {
            ProEquip_ResetGrenadeOverrides(target);
            return true;
        } else {
            return this.reset_grenades(target);
        }
    }
    bool set_all(int client, int target, bool remove) {
        if(target >= 0) {
            ReplyToCommand(client, "Cannot set per-player weapon availability");
        }
        bool players = (target == EQUIP_BOTH || target == EQUIP_PLAYERS)? true: false;
        bool bots = (target == EQUIP_BOTH || target == EQUIP_BOTS)? true: false;
        if(!remove) {
            return this.reset_weapons(target, players, bots);
        } else {
            return this.remove_weapons(target, players, bots);
        }
    }
    bool reset_weapons(int client, bool players, bool bots) {
                ProEquip_ResetGrenadeOverrides(client);
                return  ProEquip_ResetWeaponSlotAvailability(players, bots, 0) &
                        ProEquip_ResetWeaponSlotAvailability(players, bots, 1);
    }
    bool remove_weapons(int client, bool players, bool bots) {
        return  ProEquip_ResetSlotPreference(players, bots, 0, false) &
                ProEquip_ResetSlotPreference(players, bots, 1, false) &
                ProEquip_SetGrenade(client, "hegrenade", 0) &
                ProEquip_SetGrenade(client, "flashbang", 0) &
                ProEquip_SetGrenade(client, "smokegrenade", 0);
    }
    bool reset_grenades(int client) {
        return  ProEquip_SetGrenade(client, "hegrenade", 0) &
                ProEquip_SetGrenade(client, "flashbang", 0) &
                ProEquip_SetGrenade(client, "smokegrenade", 0)
    }
    void debug() {
        #if DEBUGGING
            LogToFileEx(EquipLogFile, "Action: type=%i target='%s'\nparam='%s' amount=%i", this.type, this.target, this.param, this.amount);
        #endif
    }
}


public Action Command_Equip(int client, int args) {
    int cur_arg = 1;
    
    if(args == 0) {
        equip_usage(client);
        return Plugin_Continue;
    } else if (args > MAX_ARGS) {
        args = MAX_ARGS;
        ReplyToCommand(client, "Maximum number of arguments (%i) reached", MAX_ARGS);
    }
    
    errorType error;
    bool reset_needed = false; // reset client preferences for invalid weapons
    bool update_needed = true; // remove any invalid weapons (also give knife if needed)
    char buffer[32];
    GetCmdArg(cur_arg, buffer, sizeof(buffer));
    
    if(StrEqual(buffer, "help", false)) {
        cur_arg++;
        equip_help(client, cur_arg, args);
        return Plugin_Continue;
    } else if(StrEqual(buffer, "examples", false)) {
        cur_arg++;
        equip_examples(client, cur_arg, args);
        return Plugin_Continue;
    } else if(args == 1 && StrEqual(buffer, "reload", false)) {
        cur_arg++;
        if(ProEquip_ReloadConfigs()) {
            ReplyToCommand(client, "Equipment configs reloaded");
        } else {
            ReplyToCommand(client, "Failed to reload equipment configs");
        }
        return Plugin_Continue;
    } else if(args == 1 && StrEqual(buffer, "reset", false)) {
        cur_arg++;
        // todo: this needs some luvin'
        ProEquip_ResetEquipAll();
        ProEquip_SetSlotPreference(EQUIP_BOTS, 0, RANDOM_WEAPON_AVAILABLE);
        ProEquip_SetSlotPreference(EQUIP_BOTS, 1, RANDOM_WEAPON_AVAILABLE);
        ProEquip_ResetEquip(EQUIP_BOTH);
        ReplyToCommand(client, "All equipment reset to map defaults");
        return Plugin_Continue;
    } else {
        EquipAction action;
        
        // get target
        GetCmdArg(cur_arg, buffer, sizeof(buffer));
        cur_arg++;
        
        // PROCESS EQUIPMENT TARGET
        if(StrEqual(buffer, "all", false) || StrEqual(buffer, "both", false) || StrEqual(buffer, "players", false) || StrEqual(buffer, "bots", false)) {
            strcopy(action.target, 32, buffer);
        } else if(StrEqual(buffer, "override", false)) {
            GetCmdArg(cur_arg, buffer, sizeof(buffer));
            cur_arg++;
            strcopy(action.target, 32, buffer);
        } else {
            action.target = "all";
            cur_arg--;
        }
        
        // ARGUMENT LOOP
        int pos = -1;
        bool arg_reset_prefs;
        bool arg_update;
        while(cur_arg <= args) {
            arg_reset_prefs = false;
            arg_update = false
            GetCmdArg(cur_arg, buffer, sizeof(buffer));
            cur_arg++;
            
            if((buffer[0] == '+' && buffer[1] != '+') || (buffer[0] == '-' && buffer[1] != '-')) {
                // SET EQUIPMENT WEAPON MODIFICATIONS
                switch(buffer[0]) {
                    case '-': {
                        action.type = EquipAction_WeaponRemove;
                        arg_update = true;
                        arg_reset_prefs = true;
                    }
                    case '+': {
                        action.type = EquipAction_WeaponAdd;
                    }
                }
                strcopy(action.param, 32, buffer[1]);
            } else if((pos = FindCharInString(buffer, '=')) && pos != -1) {
                // SET EQUIPMENT QUANTITIES
                action.type = EquipAction_Equip;
                char stats[2][16];
                ExplodeString(buffer, "=", stats, sizeof(stats), 16);
                if(StrEqual(stats[1], "on", false) || StrEqual(stats[1], "true", false)) {
                    action.amount = -1;
                } else if(StrEqual(stats[1], "off", false) || StrEqual(stats[1], "false", false)) {
                    action.amount = 0;
                } else {
                    action.amount = StringToInt(stats[1]);
                }
                strcopy(action.param, 32, stats[0]);
            } else if(StrEqual(buffer, "reset", false)) {
                action.type = EquipAction_Reset;
                arg_reset_prefs = true;
                arg_update = true;
            } else if(StrEqual(buffer, "--respawn", false)) {
                update_needed = false;
                continue;
            } else if(strlen(buffer) == 0) {
                continue;
            } else {
                ReplyToCommand(client, "Could not process argument '%s'", buffer);
                action.reset();
                continue;
            }
            
            if(!action.process(client)) {
                LogToFileEx(EquipLogFile, "Action failed: cur_arg=%i", cur_arg);
                action.debug();
                error = Error_ActionFailed;
            } else {
                if(arg_reset_prefs) {
                    reset_needed = true;
                }
                if(arg_update) {
                    update_needed = true;
                }
            }
            action.reset();
        }
    }
    // END OF ARGUMENT LOOP
    
    if(reset_needed) {
        reset_invalid_weapon_prefs(EQUIP_BOTH, RANDOM_WEAPON_AVAILABLE);
    }
    if(update_needed) {
        ProEquip_UpdateUserWeapons();
    }
    
    error_reply(client, error);
    return Plugin_Continue;
}

public void error_reply(int client, errorType error) {
    switch(error) {
        case Error_None: {
            ReplyToCommand(client, "Equipment updated");
        }
        case Error_ActionFailed: {
            ReplyToCommand(client, "An error occurred that may have prevented equip from completing.");
        }
    }
}


public void equip_usage(int client) {
    ReplyToCommand(client, "!equip [bots|players|all|override <player>] [+|-all|rifles|pistols|knife] [hp|armor|he|flash|smoke=<num>]");
    ReplyToCommand(client, "!equip override <player> reset");
    ReplyToCommand(client, "!equip reset");
    ReplyToCommand(client, "!equip examples");
}
public bool equip_help(int client, int &cur_arg, int args) {
    // could possibly add flags/args to display help for specific sections
    equip_usage(client);
    return false;
}
public bool equip_examples(int client, int &cur_arg, int args) {
    ReplyToCommand(client, "!equip he=4 smoke=2 flash=0 armor=100 hp=250");
    ReplyToCommand(client, "!equip -rifles +scout -pistols +glock +usp -grenades");
    ReplyToCommand(client, "!equip all -all +scout armor=0 hp=100");
    ReplyToCommand(client, "!equip players -flashbang +hegrenade");
    ReplyToCommand(client, "!equip override Vishus reset");
    return false;
}



