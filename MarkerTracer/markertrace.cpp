/*
 * Copyright (C) 2004-2021 Intel Corporation.
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include <fstream>
#include "pin.H"
#include <map>

using std::cerr;
using std::endl;
using std::ios;
using std::ofstream;
using std::string;

// Only trace branches if this variable is marked true
BOOL enable_branch_tracing = false;

// This represents the output file
ofstream OutFile;
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "markertrace.out", "specify output file name");
KNOB<INT64> KnobMagic(KNOB_MODE_WRITEONCE, "pintool", "m", "7788", "specify the magic marker number");

// This function is called for every marker
VOID printMarker(UINT64 rcx, INT64 arg, UINT64 expected)
{
        // Only print if RCX has the magic value
        if(rcx != expected)
                return;

        // -1 marks the start of trace
        if(arg == -1)
        {
                OutFile << arg << " ";
                enable_branch_tracing = true;
        }
        // -2 marks the end of trace
        else if(arg == -2)
        {
                OutFile << arg << endl;
                enable_branch_tracing = false;
        }
        // Any other value is used for regular markers
        else
                OutFile << arg << " ";
}

// This function is called for every branch instruction
VOID printBranch(BOOL taken)
{
        if(enable_branch_tracing)
                OutFile << "B" << taken << " ";
}

// Instrument the code to insert print instructions whenever a branch or a marker is observed
VOID Instruction(INS ins, VOID* v)
{
        // Insert a call to track branches
        if(INS_IsBranch(ins) && INS_HasFallThrough(ins))
                INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)printBranch, IARG_BRANCH_TAKEN, IARG_END);
        
        // Insert a call to track markers
        if(INS_IsXchg(ins) && INS_OperandReg(ins, 0) == REG_BX && INS_OperandReg(ins, 1) == REG_BX)
        {
                INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)printMarker,
                                IARG_REG_VALUE, REG_GCX,        /* RCX value */
                                IARG_REG_VALUE, REG_GDX,        /* argument */
                                IARG_UINT64, (UINT64)v,         /* expected value in RCX */
                                IARG_END);
        }
}

// Close the output file
VOID Fini(INT32 code, VOID* v)
{
        OutFile.close();
}

// Print Help Message
INT32 Usage()
{
        cerr << "This tool tracks the markers and branches in the code" << endl;
        cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
        return -1;
}

int main(int argc, char* argv[])
{
        // Initialize pin
        if (PIN_Init(argc, argv)) return Usage();

        // Open the output file
        OutFile.open(KnobOutputFile.Value().c_str());
        
        // Register the function to be called to instrument instructions
        INT64 magic = KnobMagic.Value();
        INS_AddInstrumentFunction(Instruction, (VOID*)magic);

        // Register Fini to be called when the application exits
        PIN_AddFiniFunction(Fini, 0);

        // Start the program, never returns
        PIN_StartProgram();

        return 0;
}
