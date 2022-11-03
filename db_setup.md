# Database Setup

## Setup

1. To add custom menu entries add the following to`cstrike/addons/sourcemod/configs/databases.cfg`:

```
  "pro_menu"
    {
        "driver"    "default"
        "host"      "<hostname>"
        "database"  "<database>"
        "user"      "<username>"
        "pass"      "<password>"
    }
```

2. The table will be created automatically on first run.  If you have issues see [Manual Setup](db_setup.md#manual-setup)
   
   > Note: automatically creating the tables has only been tested with MySQL, but should work for Postgres and SQLite as well.

## Manual Setup

If you have problems you can manually create the table. MySQL code:

```
CREATE TABLE `pro_menu` (
  `id` int(11) NOT NULL,
  `ordering` int(11) NOT NULL,
  `value` varchar(32) NOT NULL,
  `title` varchar(32) NOT NULL,
  `cmd` varchar(32) NOT NULL,
  `msg` varchar(256) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

ALTER TABLE `pro_menu`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `pro_menu_value_unique` (`value`);

ALTER TABLE `pro_menu`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;
```
