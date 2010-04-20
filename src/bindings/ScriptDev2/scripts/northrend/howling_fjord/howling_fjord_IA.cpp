#include "precompiled.h"


void AddSC_proto_egg_creature()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "proto_egg";
    newscript->GetAI = &GetAI_proto_egg;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "web_spid_AI";
    newscript->GetAI = &GetAI_web;
    newscript->RegisterSelf();
}