#if defined _pro_equip_db_included
    #endinput
#endif
#define _pro_equip_db_included

public void DbConnCallback(Database db, const char[] error, any data) {
    if(strlen(error) > 0 || db == null) {
        LogToFile(EquipLogFile, "Database not connected: %s", error);
        return;
    }
    DB = db;
    Setup();
}

public void Setup() {
    char db_ident[16];
    
    // Find out which database driver is being used
    SQL_ReadDriver(DB, db_ident, sizeof(db_ident));
    
    SQL_LockDatabase(DB); // this is already running in a thread (from Database.Connect()) so it should be fine to use blocking queries here as long as the other queries are async
    
    // Auto create the table based on which driver is being used
    if(StrEqual(db_ident, "mysql", false)) {
        SQL_FastQuery(DB, "CREATE TABLE IF NOT EXISTS `pro_menu` (`id` int(11) NOT NULL, `ordering` int(11) NOT NULL, `value` varchar(32) NOT NULL, `title` varchar(32) NOT NULL, `cmd` varchar(32) NOT NULL, `msg` varchar(256) DEFAULT NULL) ENGINE=InnoDB DEFAULT CHARSET=latin1;");
        SQL_FastQuery(DB, "ALTER TABLE `pro_menu` ADD PRIMARY KEY (`id`), ADD UNIQUE KEY `pro_menu_value_unique` (`value`);");
        SQL_FastQuery(DB, "ALTER TABLE `pro_menu` MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;");
    } else if (StrEqual(db_ident, "sqlite")) {
        SQL_FastQuery(DB, "CREATE TABLE IF NOT EXISTS 'pro_menu' ('id'	INTEGER NOT NULL, 'ordering' INTEGER NOT NULL, 'value'	TEXT NOT NULL UNIQUE, 'title'	TEXT NOT NULL, 'cmd'	TEXT NOT NULL, 'msg'	TEXT, PRIMARY KEY('id' AUTOINCREMENT));");
    } else if (StrEqual(db_ident, "pgsql")) {
        SQL_FastQuery(DB, "CREATE TABLE IF NOT EXISTS 'pro_menu' (id integer NOT NULL, ordering integer NOT NULL, value character varying(32) NOT NULL, title character varying(32) NOT NULL, cmd character varying(32) NOT NULL, msg character varying(256));");
        SQL_FastQuery(DB, "CREATE SEQUENCE IF NOT EXISTS pro_menu_id_seq  START WITH 1  INCREMENT BY 1  NO MINVALUE  NO MAXVALUE  CACHE 1;");
        SQL_FastQuery(DB, "ALTER TABLE ONLY pro_menu ALTER COLUMN id SET DEFAULT nextval('pro_menu_id_seq'::regclass);");
        SQL_FastQuery(DB, "ALTER TABLE ONLY pro_menu ADD CONSTRAINT pro_menu_pkey PRIMARY KEY (id);");
        SQL_FastQuery(DB, "ALTER TABLE ONLY pro_menu ADD CONSTRAINT pro_menu_value_key UNIQUE (value);");
    } else {
        ThrowError("Invalid database driver specified: creating tables automatically for '%s' is not supported.  Please create the table manually.", db_ident);
        DB = null;
    }
    SQL_UnlockDatabase(DB);
    
    
    load_menu_extras();
    
    late_loaded = false;
}
