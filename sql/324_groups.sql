ALTER TABLE groups
  CHANGE COLUMN isRaid groupType tinyint(1) unsigned NOT NULL;

UPDATE groups
  SET groupType = 2 WHERE groupType = 1;
