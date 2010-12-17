/* Correct decrease aura stacking when healing for Bone shield and Earth Shield*/

UPDATE spell_proc_event SET procFlags = 1048576 WHERE entry = 974;
UPDATE spell_proc_event SET procFlags = 1048576 WHERE entry = 49222;
