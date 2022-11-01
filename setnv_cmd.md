# `!setnv`

## Overview

Set a player's nightivision (if [ProNightvision](https://github.com/vishusandy/ProNightvision) is loaded).

It can:

- turn on/off nightvision

- set nightvision to a custom nightvision filter

- list available custom nightvision filters 

## Syntax

```
  !setnv <target> on|normal|off|<filter_name>
  !setnv list
```

- "on" is the same as "normal", and will both turn a player's nightvision on using normal nightvision

- `list` will list available custom filters (normal nightvision is always available)

## Examples

```
  !setnv help
  !setnv <target> normal
  !setnv <target> off
  !setnv <target> strong
  !setnv list
```
