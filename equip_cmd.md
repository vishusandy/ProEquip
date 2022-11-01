# `!equip`

## Overview

The `!equip` command modifies grenade quantities, armor, and hp as well as what weapons are available for bots and/or players (all bots/all players, not individual players). It can also be used to reset equipment to defaults.

## Syntax

```
  !equip [bots|players|all|override <player>] [+|-all|rifles|pistols|knife] [hp|armor|he|flash|smoke=<num>]
  !equip [override <target>] reset
  !equip reload
  !equip examples
  !equip help
```

## Examples

```
# For all humans and bots (if bots or players are not specified it defaults to all)
!equip he=4 smoke=2 flash=0 armor=100 hp=120

# For just bots
!equip bots he=0 hp=80 

# For just humans
!equip players hp=120 armor=100

# Remove all rifles, pistols and grenades, then enable scout, glock, and usp
!equip -rifles +scout -pistols +glock +usp -grenades

# remove flashbangs and add hegrenades for players
!equip players -flashbang +hegrenade

# Remove all weapons and add just a scout.  Also modify armor and hp
!equip -all +scout armor=0 hp=100

# Target equipment settings for a specific player
!equip override <target> he=1 smoke=1 flash=1 hp=140 armor=0

# Reset equipment overrides for a specific player
!equip override <target> reset

# Reset all equipment settings
```
