#if defined _pro_equip_snapshots_included
    #endinput
#endif
#define _pro_equip_snapshots_included

#include <pro_equip/vars.inc>
#include <pro_equip/available_weapons.inc>
#include <pro_equip/equip_pref.inc>
#include <pro_equip/client_data.inc>

/*
    Snapshots
        Snapshots allow plugins to save the current state and restore it later.
        The purpose is to allow creating things like "fun rounds" where only 
        certain weapons are allowed, and afterwards preferences will be restored,
        
    Status: todo
        snapshots have not been properly tested or debugged yet
 */

enum struct Snapshot {
    AvailableWeapons player_weapons;
    AvailableWeapons bot_weapons;
    EquipPref player_equip;
    EquipPref bot_equip;
    ArrayList player_data;
}

void snapshots_plugin_start() {
    snapshots = new StringMap();
}


public bool delete_snapshot(const char[] name) {
    free_player_data(name);
    return snapshots.Remove(name);
}

public void create_snapshot(const char[] name) {
    free_player_data(name);
    Snapshot snapshot;
    copy_available_weapons(player_weapons, snapshot.player_weapons);
    copy_available_weapons(bot_weapons, snapshot.bot_weapons);
    copy_equip(player_equip, snapshot.player_equip);
    copy_equip(bot_equip, snapshot.bot_equip);
    snapshot.player_data = new ArrayList(sizeof(PlayerData));
    for(int i=0; i < MAXPLAYERS+1; i++) {
        snapshot.player_data.PushArray(player_data[i], sizeof(PlayerData));
    }
    snapshots.SetArray(name, snapshot, sizeof(Snapshot), true);
}

public bool restore_snapshot(const char[] name) {
    Snapshot snapshot;
    if(!snapshots.GetArray(name, snapshot, sizeof(snapshot))) {
        return false;
    }
    copy_available_weapons(snapshot.player_weapons, player_weapons);
    copy_available_weapons(snapshot.bot_weapons, bot_weapons);
    copy_equip(snapshot.player_equip, player_equip);
    copy_equip(snapshot.bot_equip, bot_equip);
    
    PlayerData data;
    for(int i=0; i <= MAXPLAYERS; i++) {
        if(snapshot.player_data.GetArray(i, data, sizeof(PlayerData))) {
            player_data[i] = data;
        }
    }
    return true;
}

public bool free_player_data(const char[] name) {
    Snapshot snapshot;
    if(snapshots.GetArray(name, snapshot, sizeof(Snapshot))) {
        delete snapshot.player_data;
        return true;
    }
    return false;
}



