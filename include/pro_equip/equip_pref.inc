#if defined _pro_equip_equip_pref_included
    #endinput
#endif
#define _pro_equip_equip_pref_included


enum struct EquipPref {
    int hp;
    int armor;
    int knife;
    int hegrenades;
    int flashbangs;
    int smokegrenades;
    int nightvision;
    bool defusekit;
    bool silencers;
    
    void debug(int client) {
        LogToFileEx(EquipLogFile, "target=%i hp=%i armor=%i knife=%i he=%i flash=%i smoke=%i nv=%i defuse=%i silencers=%i", client, this.hp, this.armor, this.knife, this.hegrenades, this.flashbangs, this.smokegrenades, this.nightvision, this.defusekit, this.silencers);
    }
    
    void build_random_grenades_list(AvailableWeapons weapons) {
        weapons.num_grenades = 0;
        for(int i=0; i < this.hegrenades; i++) {
            weapons.add_choice("weapon_hegrenade", false, -1);
        }
        for(int i=0; i < this.flashbangs; i++) {
            weapons.add_choice("weapon_flashbang", false, -1);
        }
        for(int i=0; i < this.smokegrenades; i++) {
            weapons.add_choice("weapon_smokegrenade", false, -1);
        }
    }
    void reset_grenades() {
        this.hegrenades = NO_OVERRIDE;
        this.flashbangs = NO_OVERRIDE;
        this.smokegrenades = NO_OVERRIDE;
    }
    int num_infinite_grenades() {
        return view_as<int>(this.hegrenades == INFINITE_AMMO)
                + view_as<int>(this.flashbangs == INFINITE_AMMO)
                + view_as<int>(this.smokegrenades == INFINITE_AMMO);
    }
    bool has_any_infinite_grenades() {
        return this.hegrenades == INFINITE_AMMO
                || this.flashbangs == INFINITE_AMMO
                || this.smokegrenades == INFINITE_AMMO;
    }
    bool grenade_override(const char[] grenade, int num) {
        if(StrEqual(grenade, "hegrenade") || StrEqual(grenade[7], "hegrenade")) {
            this.hegrenades = num;
            return true;
        } else if(StrEqual(grenade, "flashbang") || StrEqual(grenade[7], "flashbang")) {
            this.flashbangs = num;
            return true;
        } else if(StrEqual(grenade, "smokegrenade") || StrEqual(grenade[7], "smokegrenade")) {
            this.smokegrenades = num;
            return true;
        }
        ThrowError("Incorrect grenade type: %s", grenade);
        return false;
    }
    int grenade_quantity(const char[] grenade) {
        if(StrEqual(grenade, "weapon_hegrenade") || StrEqual(grenade, "hegrenade")) {
            return this.hegrenades;
        } else if(StrEqual(grenade, "weapon_flashbang") || StrEqual(grenade, "flashbang")) {
            return this.flashbangs;
        } else if(StrEqual(grenade, "weapon_smokegrenade") || StrEqual(grenade, "smokegrenade")) {
            return this.smokegrenades;
        }
        
        return -1;
    }
    bool has_grenades() {
        return this.hegrenades > 0 || this.flashbangs > 0 || this.smokegrenades > 0;
    }
}

void copy_equip(EquipPref from, EquipPref to) {
    to.hp = from.hp;
    to.armor = from.armor;
    to.knife = from.knife;
    to.hegrenades = from.hegrenades;
    to.flashbangs = from.flashbangs;
    to.smokegrenades = from.smokegrenades;
    to.nightvision = from.nightvision;
    to.defusekit = from.defusekit;
    to.silencers = from.silencers;
}

EquipPref player_equip;
EquipPref bot_equip;
EquipPref player_equip_defaults;
EquipPref bot_equip_defaults;


