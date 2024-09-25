#pragma once

struct DisassemblyView;
struct Debugger;

DisassemblyView* DisassemblyView_create();
void DisassemblyView_update(DisassemblyView* self, Debugger* debugger, const char* name);
void DisassemblyView_destroy(DisassemblyView* self);
