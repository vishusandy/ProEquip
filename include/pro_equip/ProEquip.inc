#if defined _pro_equip_included
    #endinput
#endif
#define _pro_equip_included

#include <pro_equip/constants.inc>

public SharedPlugin __pl_pro_equip = 
{
    name = "pro_equip",
    file = "ProEquip.smx",
#if defined REQUIRE_PLUGIN
    required = 1,
#else
    required = 0,
#endif
};

#if !defined REQUIRE_PLUGIN
public void __pl_pro_equip_SetNTVOptional()
{
    MarkNativeAsOptional("ProEquip_ResetEquip");
    MarkNativeAsOptional("ProEquip_ResetEquipAll");
    MarkNativeAsOptional("ProEquip_ResetWeaponPrefs");
    MarkNativeAsOptional("ProEquip_UpdateUserWeapons");
    MarkNativeAsOptional("ProEquip_ResetGrenadeOverrides");
    MarkNativeAsOptional("ProEquip_OverrideGrenades");
    MarkNativeAsOptional("ProEquip_SetGrenade");
    MarkNativeAsOptional("ProEquip_GetGrenadeQuantity");
    MarkNativeAsOptional("ProEquip_GetCurGrenadeQuantity");
    MarkNativeAsOptional("ProEquip_GiveGrenade");
    MarkNativeAsOptional("ProEquip_SetKnife");
    MarkNativeAsOptional("ProEquip_SetEquipHP");
    MarkNativeAsOptional("ProEquip_SetEquipArmor");
    MarkNativeAsOptional("ProEquip_SetEquipNightvision");
    MarkNativeAsOptional("ProEquip_SetPlayerNightvisionOn");
    MarkNativeAsOptional("ProEquip_DisplayPlayerNightvision");
    MarkNativeAsOptional("ProEquip_SetEquipDefuseKit");
    MarkNativeAsOptional("ProEquip_EnablePrimaryMenu");
    MarkNativeAsOptional("ProEquip_DisablePrimaryMenu");
    MarkNativeAsOptional("ProEquip_EnableSecondaryMenu");
    MarkNativeAsOptional("ProEquip_DisableSecondaryMenu");
    MarkNativeAsOptional("ProEquip_SetAvailableWeapon");
    MarkNativeAsOptional("ProEquip_EnableWeapons");
    MarkNativeAsOptional("ProEquip_DisableWeapons");
    MarkNativeAsOptional("ProEquip_WeaponsEnabled");
    MarkNativeAsOptional("ProEquip_ReloadConfigs");
    MarkNativeAsOptional("ProEquip_SetMenuState");
    MarkNativeAsOptional("ProEquip_SetNextMenuState");
    MarkNativeAsOptional("ProEquip_SetPrevMenuState");
    MarkNativeAsOptional("ProEquip_ResetSlotPreference");
    MarkNativeAsOptional("ProEquip_ResetWeaponSlotAvailability");
    MarkNativeAsOptional("ProEquip_SetGunPreference");
    MarkNativeAsOptional("ProEquip_SetSlotPreference");
    MarkNativeAsOptional("ProEquip_GetSlotPreference");
    MarkNativeAsOptional("ProEquip_GetWeaponIndex");
    MarkNativeAsOptional("ProEquip_GetWeaponSlot");
    MarkNativeAsOptional("ProEquip_FirstAvailableWeapon");
    MarkNativeAsOptional("ProEquip_DisplayMainMenu");
    MarkNativeAsOptional("ProEquip_DisplayPrimaryMenu");
    MarkNativeAsOptional("ProEquip_DisplaySecondaryMenu");
    MarkNativeAsOptional("ProEquip_SetSlotAmmo");
    MarkNativeAsOptional("ProEquip_HasAnyInfiniteGrenades");
    MarkNativeAsOptional("ProEquip_CreateSnapshot");
    MarkNativeAsOptional("ProEquip_DeleteSnapshot");
    MarkNativeAsOptional("ProEquip_RestoreSnapshot");
    MarkNativeAsOptional("ProEquip_RestoreDeleteSnapshot");
}
#endif

// See constants.inc for the values of the constants used

/**
 * Reset equipment for the specified players(s).
 * Can specify a specific player or all humans and/or bots.
 * Use -1 for all humans, -2 for all bots, -3 for all bots and humans.
 * 
 * 
 * @param client    Either the client index or: -1 for all humans, -2 for all bots, -3 for all players and bots
 * @return          Returns the number of players reset
 */
native int ProEquip_ResetEquip(int client); // calls `reset_admin_overrides()` on the client's player_data


/**
 * Reset equipment for all humans and bots.
 */
native void ProEquip_ResetEquipAll(); // calls `reset_admin_overrides()` on the all clients' player_data


/**
 * Updates weapon preferences after weapon availability has changed.
 * This will only change their weapon preferences; they will keep their weapons.
 * 
 * @param target    Use -1 for humans, -2 for bots, and -3 for humans and bots
 * @param reset_to  How to choose a random weapon.  Use -1 for a random available weapon, -2 for any random weapon, or -3 for none.
 */
native void ProEquip_ResetWeaponPrefs(int target, int reset_to); // calls `reset_admin_overrides()` on the all clients' player_data


/**
 * Remove weapons that the user should no longer have.
 * Use after modifying weapon availability.
 */
//.Remove weapons that players shouldn't have and add knife if needed.  Also removes extra grenades.
native void ProEquip_UpdateUserWeapons();


/**
 * Reset grenades preferences to default, without updating or removing any quantities players may have.
 * 
 * @param client    The client index or -1 for all humans, -2 for all bots, or -3 for all bots and humans
 */
native void ProEquip_ResetGrenadeOverrides(int client);


/**
 * Override the grenade quantity available for specific player(s).
 * If specifying a specific client, their current grenade quantity will be set immediately.
 * 
 * @param client    Client index or -1 for all humans, -2 for all bots, -3 for all humans and bots
 * @param grenade   The grenade type ("hegrenade","flashbang", or "smokegrenade")
 * @param num       The quantity that should be available
 * @return          Returns whether the override was successful
 */
native bool ProEquip_OverrideGrenades(int client, const char[] grenade, int num);


/**
 * Override the grenade quantity available for specified player(s).
 * If a specific client is specified, they will not recieve the updated quantity until respawn.
 * 
 * @param client    Client index or -1 for all humans, -2 for all bots, -3 for all humans and bots
 * @param grenade   The grenade type ("hegrenade","flashbang", or "smokegrenade")
 * @param num       The quantity that should be available
 * @return          Returns whether the override was successful
 */
native bool ProEquip_SetGrenade(int client, const char[] grenade, int num);


/**
 * Returns the number of a specified type of grenades the specified player gets every respawn.
 * 
 * @param client    Client index
 * @param grenade   The grenade type ("hegrenade", "flashbang", or "smokegrenade")
 * @return          The number of grenades the user will get on repsawn
 */
native int ProEquip_GetGrenadeQuantity(int client, const char[] grenade);


/**
 * Find out how many grenades of the specified type the specified player is carrying.
 * 
 * @param client    Client index
 * @param grenade   The grenade type ("hegrenade", "flashbang", or "smokegrenade")
 * @return          The number of grenades the player has
 */
native int ProEquip_GetCurGrenadeQuantity(int client, const char[] grenade);


/**
 * Give a number of grenades of the specified type to a specific player.
 * @param client    Client index
 * @param grenade   The grenade type ("hegrenade", "flashbang", or "smokegrenade")
 * @param num       The number of grenades to give
 * @return          Whether giving the grenades was successful or not
 */
native bool ProEquip_GiveGrenade(int client, char[] grenade, int num);


/**
 * Change knife equipment preferences.
 * If a client index is specified the knife will be given/removed immediately as well, otherwise it will take affect on respawn.
 * 
 * @param client    Client index, or use -1 for all humans, -2 for all bots, or -3 for all humans and bots.
 * @param knife     Whether to respawn with a knife or not
 * @param           Returns whether it was successful or not
 */
native bool ProEquip_SetKnife(int client, bool knife);


/**
 * Change the default HP for the specified player(s).  This will change the amount of HP a player spawns with; it will not change their current HP.
 * 
 * @param client    Client index, or use -1 for all humans, -2 for all bots, or -3 for all humans and bots.
 * @param hp        Amount of HP
 * @return          Returns whether it was successful or not
 */
native bool ProEquip_SetEquipHP(int client, int hp);


/**
 * Change the default armor for the specified player(s).  This will change the amount of armor recieved on spawn.
 * 
 * @param client    Client index, or use -1 for all humans, -2 for all bots, or -3 for all humans and bots.
 * @param armor     Amount of armor
 * @return          Returns whether it was successful or not
 */
native bool ProEquip_SetEquipArmor(int client, int armor);


/**
 * Change whether nightvision goggles are given
 * 
 * @param client        Client index, or use -1 for all humans, -2 for all bots, or -3 for all humans and bots.
 * @param nightvision   Whether to give nightvision goggles or not
 */
native void ProEquip_SetEquipNightvision(int client, bool nightvision);


/**
 * Turns nightvision on or off for a specified player and sets equipment preference for giving nightvision goggles.
 * 
 * @param client        Client index
 * @param nightvision   Whether to turn nightvision goggles on or off
 */
native void ProEquip_SetPlayerNightvisionOn(int client, bool nightvision);


/**
 * Turns nightvision on or off without modifying equipment preferences.
 * 
 * @param client        Client index
 * @param nightvision   Whether to turn nightvision goggles on or off
 */
native void ProEquip_DisplayPlayerNightvision(int client, bool nightvision);


/**
 * Whether to give defusal kits on spawn
 * 
 * @param client        Client index, or use -1 for all humans, -2 for all bots, or -3 for all humans and bots.
 * @param nightvision   Whether to give defusal kits or not
 */
native void ProEquip_SetEquipDefuseKit(int client, bool defusal_kit);


/**
 * Enable the primary gun menu for the player(s)
 * 
 * @param client    Client index, or use -1 for all humans
 */
native void ProEquip_EnablePrimaryMenu(int client);


/**
 * Disable the primary gun menu for the player(s)
 * 
 * @param client    Client index, or use -1 for all humans
 */

native void ProEquip_DisablePrimaryMenu(int client);


/**
 * Enable the secondary gun menu for the player(s)
 * 
 * @param client    Client index, or use -1 for all humans
 */
native void ProEquip_EnableSecondaryMenu(int client);


/**
 * Disable the secondary gun menu for the player(s)
 * 
 * @param client    Client index, or use -1 for all humans
 */
native void ProEquip_DisableSecondaryMenu(int client);


/**
 * Sets weapon availablity for humans/bots
 * 
 * @param weapon    The weapon availability to change (e.g. "scout")
 * @param humans    Whether to change the availability for human players
 * @param bots      Whether to change the availability for bots
 * @param remove    Whether the weapon should be made available or removed
 * @return          Whether weapon availability was changed or not
 */
native bool ProEquip_SetAvailableWeapon(const char[] weapon, bool humans, bool bots, bool remove);


/**
 * Changes if a menu is displayed on spawn and which menu
 * 
 * @param client    Client index
 * @param state     The `showMenu` state to use
 */
native void ProEquip_SetMenuState(int client, showMenu state);


/**
 * Changes the next menu to be displayed after the first menu is closed
 * 
 * @param client    Client index
 * @param state     The `showMenu` state to use
 */
native void ProEquip_SetNextMenuState(int client, showMenu state);


/**
 * Sets which menu was previously accessed.  This is more useful internally.
 * 
 * @param client    Client index
 * @param state     The `showMenu` state to use
 */
native void ProEquip_SetPrevMenuState(int client, showMenu state);


/**
 * Enables giving out weapons globally.  Weapons will be given out on spawn to everyone.
 */
native void ProEquip_EnableWeapons();


/**
 * Disables giving out weapons globally.  No weapons will be given on spawn to anyone.
 */
native void ProEquip_DisableWeapons();


/**
 * Returns whether weapons are given out on spawn or if it has been disabled.
 * 
 * @return      Whether giving weapons is enabled or not
 */
native bool ProEquip_WeaponsEnabled();


/**
 * Resets equipment preferences and weapon availability to the defaults.
 * Loads both the default config file and the map config.
 * Returns true if at least one of the config files was loaded successfully.
 * 
 * @return      Returns whether the reload was successful or not.
 */
native bool ProEquip_ReloadConfigs();


/**
 * Resets weapon preferences of a specified gun slot for all humans and/or bots
 * Optionally removes the weapon from players with a weapon in the specified gun slot.
 * 
 * @param players   Whether humans will be affected
 * @param bots      Whether bots will be affected
 * @param slot      Use 0 for primary slot, 1 for secondary slot, and -1 for both.
 * @param remove    Whether to remove existing weapons in the gun slot
 * @return          Returns whether it was successful or not
 */
native bool ProEquip_ResetSlotPreference(bool humans, bool bots, int slot, bool remove);


/**
 * Resets weapon preferences to the default for the specified slot.
 * 
 * @param players   Whether humans will be affected
 * @param bots      Whether bots will be affected
 * @param slot      The slot to reset.  0 for primary weapon, 1 for secondary weapon, or 3 for grenades.
 * @return          Returns whether successful or not
 */
native bool ProEquip_ResetWeaponSlotAvailability(bool humans, bool bots, int slot);


/**
 * Sets the given weapon to be given on respawn for the specified player(s).
 * 
 * @param client    Client index, or use -1 for all humans, -2 for all bots, or -3 for all humans and bots
 * @param weapon    The weapon (e.g. "scout" or "deagle")
 * @return          Returns whether successful or not
 */
native bool ProEquip_SetGunPreference(int client, const char[] weapon);


/**
 * Sets a slot preference directly to the given slot and weapon index
 * Prefer using `ProEquip_SetGunPreference` instead.
 * 
 * @param client    Client index, or use -1 for all humans, -2 for all bots, or -3 for all humans and bots
 * @return          Returns whether successful or not
 */
native bool ProEquip_SetSlotPreference(int client, int slot, int index); // directly sets a player's pistol or rifle preference - does not do any safety checks on weapon index


/**
 * Get the weapon preference for a given slot for the specified client.
 * This returns the weapon index that the client will recieve on spawn.
 * 
 * @param client    Client index
 * @param slot      Weapon slot.  Use 0 for primary slot, 1 for secondary slot, or 2 for knife slot.
 * @return          Returns the weapon index
 */
native int ProEquip_GetSlotPreference(int client, int slot);


/**
 * Get the weapon index for the specified weapon.
 * 
 * @param weapon    The weapon (e.g. "scout" or "deagle")
 * @return          Returns the index for the weapon in its associated weapon slot
 */
native int ProEquip_GetWeaponIndex(const char[] weapon);


/**
 * Get the slot numer that a specified weapon belongs to.
 * 
 * @param weapon    The weapon (e.g. "scout" or "deagle")
 * @return          Returns the slot number (starting at 0)
 */
native int ProEquip_GetWeaponSlot(const char[] weapon);


/**
 * Returns the weapon the would be given to the user by default when joining the server.
 * 
 * @param client    The client index
 * @param slot      The gun slot.  Use 0 for primary, 1 for secondary
 * @return          Returns the weapon index
 */
native int ProEquip_FirstAvailableWeapon(int client, int slot);


/**
 * Show the main menu to the specified client
 * 
 * @param client    The client index
 */
native void ProEquip_DisplayMainMenu(int client);


/**
 * Show the primary gun menu to the specified client
 * 
 * @param client    The client index
 */
native void ProEquip_DisplayPrimaryMenu(int client);


/**
 * Show the secondary gun menu to the specified client
 * 
 * @param client    The client index
 */
native void ProEquip_DisplaySecondaryMenu(int client);


/**
 * Sets the ammo of the given gun slot for the specified player
 * 
 * @param client    The client index, or -1 for humans, -2 for bots, or -3 for all humans and bots
 * @param slot      The gun slot.  Use 0 for primary, 1 for secondary
 * @param amount    The amount of reserve ammo to give, or -2 for infinite ammo
 * @return          Returns the number of clients affected
 */
native int ProEquip_SetSlotAmmo(int client, int slot, int amount);


/**
 * Returns whether any of the grenades are set to infinite
 * 
 * @param client    Client index
 * @return          Returns true if any grenades for the client have infinite ammo
 */
native bool ProEquip_HasAnyInfiniteGrenades(int client);



// Snapshots are a WIP

// Replaces any existing snapshot with the same name.  Delete the snapshot when done, or on plugin end to prevent excess memory usage.
/**
 * WIP
 */
native void ProEquip_CreateSnapshot(const char[] name);

/**
 * WIP
 */
native void ProEquip_DeleteSnapshot(const char[] name);

/**
 * WIP
 */
native void ProEquip_RestoreSnapshot(const char[] name);

/**
 * WIP
 */
native void ProEquip_RestoreDeleteSnapshot(const char[] name);


