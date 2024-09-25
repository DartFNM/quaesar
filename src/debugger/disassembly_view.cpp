#include "disassembly_view.h"
#include "debugger.h"
#include <dear_imgui/imgui.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <capstone/capstone.h>
// clang-format off
#include "sysconfig.h"
#include "sysdeps.h"
#include "options.h"
#include "memory.h"
#include "newcpu.h"
// clang-format on

struct DisassemblyView {
    int foo;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DisassemblyView* DisassemblyView_create() {
    DisassemblyView* view = new DisassemblyView();
    return view;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DisassemblyView_destroy(DisassemblyView* self) {
    delete self;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DisassemblyView_update(DisassemblyView* self, Debugger* debugger, const char* name) {
    bool open = true;
    if (ImGui::Begin(name, &open, ImGuiWindowFlags_NoScrollbar)) {
        uae_u32 pc = M68K_GETPC;
        uae_u32 offset = 40;
        uae_u32 count_bytes = 80;
        uae_u8* pc_addr = memory_get_real_address(pc);
        uae_u32 start_disasm = pc - offset;

        cs_insn* instructions = nullptr;
        int instructionCount =
            cs_disasm(debugger->capstone, pc_addr - offset, count_bytes, start_disasm, 0, &instructions);

        if (ImGui::BeginTable("##disassembly", 3,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                                  ImGuiTableFlags_Reorderable | ImGuiTableFlags_SizingFixedFit |
                                  ImGuiTableFlags_ScrollY,
                              ImVec2(0, 300))) {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("address");
            ImGui::TableSetupColumn("bytes");
            ImGui::TableSetupColumn("instruction");
            ImGui::TableHeadersRow();

            eastl::string op_str;

            for (int i = 0; i < instructionCount; ++i) {
                const cs_insn& entry = instructions[i];
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("0x%08X", entry.address);
                ImGui::TableNextColumn();
                op_str.clear();
                for (int b = 0; b < entry.size; ++b) {
                    char bytes_str[4];
                    ImSnprintf(bytes_str, 3, "%02X", entry.bytes[b]);
                    op_str.append(bytes_str, 2);
                }
                ImGui::TextUnformatted(op_str.c_str());
                ImGui::TableNextColumn();

                op_str = entry.mnemonic;
                op_str += ' ';
                op_str += entry.op_str;
                ImGui::TextUnformatted(op_str.c_str());
            }

            ImGui::EndTable();
        }

        cs_free(instructions, instructionCount);

        ImGui::End();
    }
}

