#if defined _pro_equip_hp_cmd_included
    #endinput
#endif
#define _pro_equip_hp_cmd_included

#include <pro_equip/ProEquip.inc>

void hp_plugin_start() {
    prop_iHealth = FindSendPropInfo("CCSPlayer", "m_iHealth");
    RegAdminCmd("sm_hp", Command_HP, ADM_LEVEL, "Set a player's hp");
    RegAdminCmd("sm_sethp", Command_HP, ADM_LEVEL, "Set a player's hp");
}

public Action Command_HP(client, args) {
    if(args < 2) {
        ReplyToCommand(client, "Usage: sm_hp <target> [+|-]<amount> [--save]");
        return Plugin_Handled;
    }
    
    char pattern[MAX_NAME_LENGTH];
    GetCmdArg(1, pattern, sizeof(pattern));
    
    char hp_buffer[10];
    char hp_buffer2[10];
    GetCmdArg(2, hp_buffer, sizeof(hp_buffer));
    bool add = false;
    if(hp_buffer[0] == '+' || hp_buffer[0] == '-') {
        add = true;
        strcopy(hp_buffer2, sizeof(hp_buffer2), hp_buffer[1]);
    } else {
        strcopy(hp_buffer2, sizeof(hp_buffer2), hp_buffer[0]);
    }
    int hp = StringToInt(hp_buffer2);
    if(hp_buffer[0] == '-') {
        hp = hp * -1;
    }
    if(hp == 0) {
        ReplyToCommand(client, "You must specify a valid amount");
        return Plugin_Continue;
    }
    bool save = false;
    if(args >= 3) {
        GetCmdArg(3, hp_buffer, sizeof(hp_buffer));
        if(StrEqual(hp_buffer, "--save", false)) {
            save = true;
            if(add) {
                ReplyToCommand(client, "Adding an hp amount with '--save' will not save hp changes");
            } else if(hp == 0) {
                ReplyToCommand(client, "Setting hp to 0 with '--save' will not save hp changes");
            }
        } else {
            LogToFileEx(EquipLogFile, "not saving - buffer=%s", hp_buffer);
        }
    }
    
    
    char buffer[MAX_NAME_LENGTH];
    int[] targets = new int[MaxClients];
    bool ml = false;
    int count = ProcessTargetString(pattern, client, targets, MaxClients, COMMAND_FILTER_ALIVE, buffer, sizeof(buffer), ml);
    
    int processed = 0;
    if(count <= 0) {
        ReplyToCommand(client, (count < 0)? "Bad target": "No matching targets found");
        return Plugin_Continue;
    } else {
        for(int i=0; i<count; i++) {
            if(!IsPlayerAlive(targets[i])) {
                if(!add && save) {
                    if(ProEquip_SetEquipHP(targets[i], hp)) {
                        processed += 1;
                    }
                }
                continue;
            }
            int client_hp;
            if(add) {
                client_hp = GetEntData(targets[i],prop_iHealth,4) + hp;
            } else {
                client_hp = hp;
                if(save && hp > 0) {
                    ProEquip_SetEquipHP(targets[i], hp);
                }
            }
            if(client_hp > 0) {
                SetEntData(targets[i], prop_iHealth, client_hp);
            } else {
                ForcePlayerSuicide(targets[i]);
            }
            processed += 1;
        }
    }
    ReplyToCommand(client, "Affected %i users", processed);
    return Plugin_Continue;
}

