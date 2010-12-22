/* Ebon Hold Telporter fix */

UPDATE `creature` SET `phaseMask`=69 WHERE `guid` IN (128753,128754);
DELETE FROM `creature` WHERE `guid` IN (129960,130543,129963,130544);
DELETE FROM `creature_model_info` WHERE `modelid`=24719;
INSERT INTO `creature_model_info` (`modelid`,`bounding_radius`,`combat_reach`) VALUES (24719,0.075,0.15);