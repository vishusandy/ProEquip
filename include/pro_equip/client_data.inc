#if defined _pro_equip_client_data_included
    #endinput
#endif
#define _pro_equip_client_data_included

#include <pro_equip/constants.inc>
#include <pro_equip/equip_pref.inc>
#include <pro_equip/weapon_utils.inc>

enum struct PlayerData {
    WeaponMenu menu;
    MsgFlags msgs;
    MenuState menu_state;
    int last_silencer_slot;
    GunPref guns;
    EquipPref equip;
    
    void debug(int client) {
        LogToFileEx(EquipLogFile, "Client %i: Rifle=%i (ammo=%i), Pistol=%i (ammo=%i)", client, this.guns.rifle, this.guns.primary_ammo, this.guns.pistol, this.guns.secondary_ammo);
        this.equip.debug(client);
    }
    
    void reset_grenades() {
        this.equip.reset_grenades();
    }
    void reset_admin_overrides(bool &weapon_prefs_changed=false) {
        this.menu.weapon_menu_enabled = true;
        this.menu.primary_menu_enabled = true;
        this.menu.secondary_menu_enabled = true;
        this.equip.knife = NO_OVERRIDE; // This is reset to proper value for bots on team change
        this.equip.hp = NO_OVERRIDE;
        this.equip.armor = NO_OVERRIDE;
        this.reset_grenades();
        this.guns.reset_ammo();
        
        bool affected_rifles = false;
        bool affected_pistols = false;
        if(this.guns.pistol == WEAPON_NONE || this.guns.pistol == WEAPON_RESET) {
            this.guns.pistol = RANDOM_WEAPON_AVAILABLE;
            this.menu_state.show_menu = ShowMenu_Secondary;
            affected_pistols = true;
        }
        if(this.guns.rifle == WEAPON_NONE || this.guns.rifle == WEAPON_RESET) {
            this.guns.rifle = RANDOM_WEAPON_AVAILABLE;
            this.menu_state.show_menu = ShowMenu_Primary;
            affected_rifles = true;
        }
        if(affected_rifles && affected_pistols) {
            this.menu_state.next_menu = ShowMenu_Secondary;
        }
        if(affected_rifles || affected_pistols) {
            weapon_prefs_changed = true;
        }
    }
    void reset(int rifle=0, int pistol=0) {
        this.reset_admin_overrides();
        this.set_pistol(pistol);
        this.set_rifle(rifle);
        this.equip.nightvision = 0;
        this.equip.silencers = DEFAULT_SILENCERS;
        this.menu_state.reset();
        this.msgs.reset();
        this.guns.reset_ammo();
    }
    void set_rifle(int gun) {
        this.guns.set_rifle(gun);
    }
    void set_pistol(int gun) {
        this.guns.set_pistol(gun);
    }
    int random_grenade(bool all=false) {
        if(all) {
            return GetURandomInt() % sizeof(grenades);
        }
        int choices[sizeof(grenades)];
        int count = 0;
        for(int i=0; i < sizeof(grenades); i++) {
            if(player_weapons.grenade_choices[i]) {
                choices[count++] = i;
            }
        }
        if(count == 0) {
            return -1;
        }
        return choices[GetURandomInt() % count];
    }
    // This will set gun preferences to none (no guns on respawn) and the specified grenade ammo to infinite
    void set_infinite_grenade_ammo(const char[] grenade) {
        this.set_rifle(WEAPON_NONE);
        this.set_pistol(WEAPON_NONE);
        this.equip.grenade_override(grenade, INFINITE_AMMO);
    }
    bool has_any_infinite_ammo() {
        return this.equip.has_any_infinite_grenades() || this.guns.has_any_infinite_guns();
    }
    // takes a weapon classname like 'weapon_hegrenade'
    bool fire_infinite_ammo(int client, int slot, const char[] weapon) {
        switch(slot) {
            case 0, 1: {
                return inc_ammo(client, slot);
            }
            case 3: {
                if (give_weapons) {
                    replace_grenade(client, weapon);
                    ClientCommand(client, "slot4");
                    CreateTimer(0.1, TimerSlot4, client);
                    return true;
                } else {
                    return false;
                }
            }
            default: {
                return false;
            }
        }
    }
}

public Action TimerSlot4(Handle timer, int client) {
    ClientCommand(client, "slot4");
}

PlayerData player_data[MAXPLAYERS+1];






void reset_client_data(int client=EQUIP_BOTH) {
    if(client < 0) {
        bool bots = (client == EQUIP_BOTH) || (client == EQUIP_BOTS);
        bool players = (client == EQUIP_BOTH) || (client == EQUIP_PLAYERS);
        bool is_bot;
        for(int i=1; i < MaxClients; i++) {
            is_bot = !IsClientInGame(i) || IsFakeClient(i);
            if(is_bot && bots) {
                player_data[i].reset(RANDOM_WEAPON_AVAILABLE, RANDOM_WEAPON_AVAILABLE);
            } else if(!is_bot && players) {
                player_data[i].reset(player_weapons.primary_choices[0], player_weapons.secondary_choices[0]);
            }
        }
    } else {
        LogToFileEx(EquipLogFile, "Invalid client specified to reset_client_data() - client=%i", client);
    }
}
