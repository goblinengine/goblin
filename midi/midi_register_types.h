/**************************************************************************/
/*  midi_register_types.h                                                 */
/**************************************************************************/
/*                         This file is part of:                          */
/*                            GOBLIN ENGINE                               */
/**************************************************************************/

#ifndef MIDI_REGISTER_TYPES_H
#define MIDI_REGISTER_TYPES_H

#include "modules/register_module_types.h"

void initialize_midi_module(ModuleInitializationLevel p_level);
void uninitialize_midi_module(ModuleInitializationLevel p_level);

#endif // MIDI_REGISTER_TYPES_H
