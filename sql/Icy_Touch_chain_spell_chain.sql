/* Restore some Spell chain */

/* Icy Touch */
DELETE FROM spell_chain WHERE first_spell = 45477
INSERT INTO spell_chain VALUES(45477,0,45477,1,0);
INSERT INTO spell_chain VALUES(49896,45477,45477,2,0);
INSERT INTO spell_chain VALUES(49903,49896,45477,3,0);
INSERT INTO spell_chain VALUES(49904,49903,45477,4,0);
INSERT INTO spell_chain VALUES(49909,49904,45477,5,0);

