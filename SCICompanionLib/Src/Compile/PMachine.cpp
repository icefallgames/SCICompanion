/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
#include "stdafx.h"
#include "PMachine.h"

Opcode RawToOpcode(const SCIVersion &version, uint8_t rawOpcode)
{
    if (version.PackageFormat >= ResourcePackageFormat::SCI2)
    {
        switch (rawOpcode)
        {
            case 0x7d:
                return Opcode::Filename;
            case 0x7e:
                return Opcode::LineNumber;
        }
    }
    return (Opcode)(rawOpcode >> 1);
}

uint8_t OpcodeToRaw(const SCIVersion &version, Opcode opcode, bool wide)
{
    if (version.PackageFormat >= ResourcePackageFormat::SCI2)
    {
        switch (opcode)
        {
            case Opcode::Filename:
                return 0x7d;
            case Opcode::LineNumber:
                return 0x7e;
        }
    }
    return (((uint8_t)opcode) << 1) | (wide ? 0 : 1);
}

/******************************************************************************/
OperandType OpArgTypes_SCI0[TOTAL_OPCODES][3] = {
	/*bnot*/     {otEMPTY,otEMPTY,otEMPTY},
	/*add*/      {otEMPTY,otEMPTY,otEMPTY},
	/*sub*/      {otEMPTY,otEMPTY,otEMPTY},
	/*mul*/      {otEMPTY,otEMPTY,otEMPTY},
	/*div*/      {otEMPTY,otEMPTY,otEMPTY},
	/*mod*/      {otEMPTY,otEMPTY,otEMPTY},
	/*shr*/      {otEMPTY,otEMPTY,otEMPTY},
	/*shl*/      {otEMPTY,otEMPTY,otEMPTY},
	/*xor*/      {otEMPTY,otEMPTY,otEMPTY},
	/*and*/      {otEMPTY,otEMPTY,otEMPTY},
/*10*/
	/*or*/       {otEMPTY,otEMPTY,otEMPTY},
	/*neg*/      {otEMPTY,otEMPTY,otEMPTY},
	/*not*/      {otEMPTY,otEMPTY,otEMPTY},
	/*eq?*/      {otEMPTY,otEMPTY,otEMPTY},
	/*ne?*/      {otEMPTY,otEMPTY,otEMPTY},
	/*gt?*/      {otEMPTY,otEMPTY,otEMPTY},
	/*ge?*/      {otEMPTY,otEMPTY,otEMPTY},
	/*lt?*/      {otEMPTY,otEMPTY,otEMPTY},
	/*le?*/      {otEMPTY,otEMPTY,otEMPTY},
	/*ugt?*/     {otEMPTY,otEMPTY,otEMPTY},
/*20*/
	/*uge?*/     {otEMPTY,otEMPTY,otEMPTY},
	/*ult?*/     {otEMPTY,otEMPTY,otEMPTY},
	/*ule?*/     {otEMPTY,otEMPTY,otEMPTY},
	/*bt*/       {otLABEL,otEMPTY,otEMPTY},
	/*bnt*/      {otLABEL,otEMPTY,otEMPTY},
	/*jmp*/      {otLABEL,otEMPTY,otEMPTY},
	/*ldi*/      {otINT,otEMPTY,otEMPTY},
	/*push*/     {otEMPTY,otEMPTY,otEMPTY},
	/*pushi*/    {otINT,otEMPTY,otEMPTY},
	/*toss*/     {otEMPTY,otEMPTY,otEMPTY},
/*30*/
	/*dup*/      {otEMPTY,otEMPTY,otEMPTY},
	/*link*/     {otUINT,otEMPTY,otEMPTY},
    /*call*/     {otLABEL,otUINT8,otEMPTY},
	/*callk*/    {otKERNEL,otUINT8,otEMPTY},
	/*callb*/    {otPUBPROC,otUINT8,otEMPTY},
	/*calle*/    {otUINT,otPUBPROC,otUINT8},
	/*ret*/      {otEMPTY,otEMPTY,otEMPTY},
	/*send*/     {otUINT8,otEMPTY,otEMPTY},
	/**/         {otEMPTY,otEMPTY,otEMPTY},
	/**/         {otEMPTY,otEMPTY,otEMPTY},
/*40*/
	/*class*/    {otCLASS,otEMPTY,otEMPTY},
	/**/         {otEMPTY,otEMPTY,otEMPTY},
    /*self*/     {otUINT8, otEMPTY, otEMPTY },
    /*super*/    {otCLASS, otUINT8, otEMPTY },
	/*rest*/     {otPVAR,otEMPTY,otEMPTY},
	/*lea*/      {otUINT,otUINT,otEMPTY},
	/*selfID*/   {otEMPTY,otEMPTY,otEMPTY},
	/**/         {otEMPTY,otEMPTY,otEMPTY},
	/*pprev*/    {otEMPTY,otEMPTY,otEMPTY},
	/*pToa*/     {otPROP,otEMPTY,otEMPTY},
/*50*/
	/*aTop*/     {otPROP,otEMPTY,otEMPTY},
	/*pTos*/     {otPROP,otEMPTY,otEMPTY},
	/*sTop*/     {otPROP,otEMPTY,otEMPTY},
	/*ipToa*/    {otPROP,otEMPTY,otEMPTY},
	/*dpToa*/    {otPROP,otEMPTY,otEMPTY},
	/*ipTos*/    {otPROP,otEMPTY,otEMPTY},
	/*dpTos*/    {otPROP,otEMPTY,otEMPTY},
	/*lofsa*/    {otOFFS,otEMPTY,otEMPTY},
	/*lofss*/    {otOFFS,otEMPTY,otEMPTY},
	/*push0*/    {otEMPTY,otEMPTY,otEMPTY},
/*60*/
	/*push1*/    {otEMPTY,otEMPTY,otEMPTY},
	/*push2*/    {otEMPTY,otEMPTY,otEMPTY},
	/*pushSelf*/ {otEMPTY,otEMPTY,otEMPTY},
    /**/         {otEMPTY,otEMPTY,otEMPTY},
	/*lag*/      {otVAR,otEMPTY,otEMPTY},
	/*lal*/      {otVAR,otEMPTY,otEMPTY},
	/*lat*/      {otVAR,otEMPTY,otEMPTY},
	/*lap*/      {otVAR,otEMPTY,otEMPTY},
	/*lsg*/      {otVAR,otEMPTY,otEMPTY},
	/*lsl*/      {otVAR,otEMPTY,otEMPTY},
/*70*/
	/*lst*/      {otVAR,otEMPTY,otEMPTY},
	/*lsp*/      {otVAR,otEMPTY,otEMPTY},
	/*lagi*/     {otVAR,otEMPTY,otEMPTY},
	/*lali*/     {otVAR,otEMPTY,otEMPTY},
	/*lati*/     {otVAR,otEMPTY,otEMPTY},
	/*lapi*/     {otVAR,otEMPTY,otEMPTY},
	/*lsgi*/     {otVAR,otEMPTY,otEMPTY},
	/*lsli*/     {otVAR,otEMPTY,otEMPTY},
	/*lsti*/     {otVAR,otEMPTY,otEMPTY},
	/*lspi*/     {otVAR,otEMPTY,otEMPTY},
/*80*/
	/*sag*/      {otVAR,otEMPTY,otEMPTY},
	/*sal*/      {otVAR,otEMPTY,otEMPTY},
	/*sat*/      {otVAR,otEMPTY,otEMPTY},
	/*sap*/      {otVAR,otEMPTY,otEMPTY},
	/*ssg*/      {otVAR,otEMPTY,otEMPTY},
	/*ssl*/      {otVAR,otEMPTY,otEMPTY},
	/*sst*/      {otVAR,otEMPTY,otEMPTY},
	/*ssp*/      {otVAR,otEMPTY,otEMPTY},
	/*sagi*/     {otVAR,otEMPTY,otEMPTY},
	/*sali*/     {otVAR,otEMPTY,otEMPTY},
/*90*/
	/*sati*/     {otVAR,otEMPTY,otEMPTY},
	/*sapi*/     {otVAR,otEMPTY,otEMPTY},
	/*ssgi*/     {otVAR,otEMPTY,otEMPTY},
	/*ssli*/     {otVAR,otEMPTY,otEMPTY},
	/*ssti*/     {otVAR,otEMPTY,otEMPTY},
	/*sspi*/     {otVAR,otEMPTY,otEMPTY},
	/*+ag*/      {otVAR,otEMPTY,otEMPTY},
	/*+al*/      {otVAR,otEMPTY,otEMPTY},
	/*+at*/      {otVAR,otEMPTY,otEMPTY},
	/*+ap*/      {otVAR,otEMPTY,otEMPTY},
/*100*/
	/*+sg*/      {otVAR,otEMPTY,otEMPTY},
	/*+sl*/      {otVAR,otEMPTY,otEMPTY},
	/*+st*/      {otVAR,otEMPTY,otEMPTY},
	/*+sp*/      {otVAR,otEMPTY,otEMPTY},
	/*+agi*/     {otVAR,otEMPTY,otEMPTY},
	/*+ali*/     {otVAR,otEMPTY,otEMPTY},
	/*+ati*/     {otVAR,otEMPTY,otEMPTY},
	/*+api*/     {otVAR,otEMPTY,otEMPTY},
	/*+sgi*/     {otVAR,otEMPTY,otEMPTY},
	/*+sli*/     {otVAR,otEMPTY,otEMPTY},
/*110*/
	/*+sti*/     {otVAR,otEMPTY,otEMPTY},
	/*+spi*/     {otVAR,otEMPTY,otEMPTY},
	/*-ag*/      {otVAR,otEMPTY,otEMPTY},
	/*-al*/      {otVAR,otEMPTY,otEMPTY},
	/*-at*/      {otVAR,otEMPTY,otEMPTY},
	/*-ap*/      {otVAR,otEMPTY,otEMPTY},
	/*-sg*/      {otVAR,otEMPTY,otEMPTY},
	/*-sl*/      {otVAR,otEMPTY,otEMPTY},
	/*-st*/      {otVAR,otEMPTY,otEMPTY},
	/*-sp*/      {otVAR,otEMPTY,otEMPTY},
/*120*/
	/*-agi*/     {otVAR,otEMPTY,otEMPTY},
	/*-ali*/     {otVAR,otEMPTY,otEMPTY},
	/*-ati*/     {otVAR,otEMPTY,otEMPTY},
	/*-api*/     {otVAR,otEMPTY,otEMPTY},
	/*-sgi*/     {otVAR,otEMPTY,otEMPTY},
	/*-sli*/     {otVAR,otEMPTY,otEMPTY},
	/*-sti*/     {otVAR,otEMPTY,otEMPTY},
	/*-spi*/     {otVAR,otEMPTY,otEMPTY},
};

OperandType OpArgTypes_SCI2[TOTAL_OPCODES][3] = {
    /*bnot*/{ otEMPTY, otEMPTY, otEMPTY },
    /*add*/{ otEMPTY, otEMPTY, otEMPTY },
    /*sub*/{ otEMPTY, otEMPTY, otEMPTY },
    /*mul*/{ otEMPTY, otEMPTY, otEMPTY },
    /*div*/{ otEMPTY, otEMPTY, otEMPTY },
    /*mod*/{ otEMPTY, otEMPTY, otEMPTY },
    /*shr*/{ otEMPTY, otEMPTY, otEMPTY },
    /*shl*/{ otEMPTY, otEMPTY, otEMPTY },
    /*xor*/{ otEMPTY, otEMPTY, otEMPTY },
    /*and*/{ otEMPTY, otEMPTY, otEMPTY },
    /*10*/
    /*or*/{ otEMPTY, otEMPTY, otEMPTY },
    /*neg*/{ otEMPTY, otEMPTY, otEMPTY },
    /*not*/{ otEMPTY, otEMPTY, otEMPTY },
    /*eq?*/{ otEMPTY, otEMPTY, otEMPTY },
    /*ne?*/{ otEMPTY, otEMPTY, otEMPTY },
    /*gt?*/{ otEMPTY, otEMPTY, otEMPTY },
    /*ge?*/{ otEMPTY, otEMPTY, otEMPTY },
    /*lt?*/{ otEMPTY, otEMPTY, otEMPTY },
    /*le?*/{ otEMPTY, otEMPTY, otEMPTY },
    /*ugt?*/{ otEMPTY, otEMPTY, otEMPTY },
    /*20*/
    /*uge?*/{ otEMPTY, otEMPTY, otEMPTY },
    /*ult?*/{ otEMPTY, otEMPTY, otEMPTY },
    /*ule?*/{ otEMPTY, otEMPTY, otEMPTY },
    /*bt*/{ otLABEL, otEMPTY, otEMPTY },
    /*bnt*/{ otLABEL, otEMPTY, otEMPTY },
    /*jmp*/{ otLABEL, otEMPTY, otEMPTY },
    /*ldi*/{ otINT, otEMPTY, otEMPTY },
    /*push*/{ otEMPTY, otEMPTY, otEMPTY },
    /*pushi*/{ otINT, otEMPTY, otEMPTY },
    /*toss*/{ otEMPTY, otEMPTY, otEMPTY },
    /*30*/
    /*dup*/{ otEMPTY, otEMPTY, otEMPTY },
    /*link*/{ otUINT, otEMPTY, otEMPTY },
    /*call*/{ otLABEL, otUINT16, otEMPTY },
    /*callk*/{ otKERNEL, otUINT16, otEMPTY },
    /*callb*/{ otPUBPROC, otUINT16, otEMPTY },
    /*calle*/{ otUINT, otPUBPROC, otUINT16 },
    /*ret*/{ otEMPTY, otEMPTY, otEMPTY },
    /*send*/{ otUINT16, otEMPTY, otEMPTY },
    /**/{ otEMPTY, otEMPTY, otEMPTY },
    /**/{ otEMPTY, otEMPTY, otEMPTY },
    /*40*/
    /*class*/{ otCLASS, otEMPTY, otEMPTY },
    /**/{ otEMPTY, otEMPTY, otEMPTY },
    /*self*/{ otUINT16, otEMPTY, otEMPTY },
    /*super*/{ otCLASS, otUINT16, otEMPTY },
    /*rest*/{ otPVAR, otEMPTY, otEMPTY },
    /*lea*/{ otUINT, otUINT, otEMPTY },
    /*selfID*/{ otEMPTY, otEMPTY, otEMPTY },
    /**/{ otEMPTY, otEMPTY, otEMPTY },
    /*pprev*/{ otEMPTY, otEMPTY, otEMPTY },
    /*pToa*/{ otPROP, otEMPTY, otEMPTY },
    /*50*/
    /*aTop*/{ otPROP, otEMPTY, otEMPTY },
    /*pTos*/{ otPROP, otEMPTY, otEMPTY },
    /*sTop*/{ otPROP, otEMPTY, otEMPTY },
    /*ipToa*/{ otPROP, otEMPTY, otEMPTY },
    /*dpToa*/{ otPROP, otEMPTY, otEMPTY },
    /*ipTos*/{ otPROP, otEMPTY, otEMPTY },
    /*dpTos*/{ otPROP, otEMPTY, otEMPTY },
    /*lofsa*/{ otOFFS, otEMPTY, otEMPTY },
    /*lofss*/{ otOFFS, otEMPTY, otEMPTY },
    /*push0*/{ otEMPTY, otEMPTY, otEMPTY },
    /*60*/
    /*push1*/{ otEMPTY, otEMPTY, otEMPTY },
    /*push2*/{ otEMPTY, otEMPTY, otEMPTY },
    /*pushSelf*/{ otEMPTY, otEMPTY, otEMPTY },
    /**/{ otEMPTY, otEMPTY, otEMPTY },
    /*lag*/{ otVAR, otEMPTY, otEMPTY },
    /*lal*/{ otVAR, otEMPTY, otEMPTY },
    /*lat*/{ otVAR, otEMPTY, otEMPTY },
    /*lap*/{ otVAR, otEMPTY, otEMPTY },
    /*lsg*/{ otVAR, otEMPTY, otEMPTY },
    /*lsl*/{ otVAR, otEMPTY, otEMPTY },
    /*70*/
    /*lst*/{ otVAR, otEMPTY, otEMPTY },
    /*lsp*/{ otVAR, otEMPTY, otEMPTY },
    /*lagi*/{ otVAR, otEMPTY, otEMPTY },
    /*lali*/{ otVAR, otEMPTY, otEMPTY },
    /*lati*/{ otVAR, otEMPTY, otEMPTY },
    /*lapi*/{ otVAR, otEMPTY, otEMPTY },
    /*lsgi*/{ otVAR, otEMPTY, otEMPTY },
    /*lsli*/{ otVAR, otEMPTY, otEMPTY },
    /*lsti*/{ otVAR, otEMPTY, otEMPTY },
    /*lspi*/{ otVAR, otEMPTY, otEMPTY },
    /*80*/
    /*sag*/{ otVAR, otEMPTY, otEMPTY },
    /*sal*/{ otVAR, otEMPTY, otEMPTY },
    /*sat*/{ otVAR, otEMPTY, otEMPTY },
    /*sap*/{ otVAR, otEMPTY, otEMPTY },
    /*ssg*/{ otVAR, otEMPTY, otEMPTY },
    /*ssl*/{ otVAR, otEMPTY, otEMPTY },
    /*sst*/{ otVAR, otEMPTY, otEMPTY },
    /*ssp*/{ otVAR, otEMPTY, otEMPTY },
    /*sagi*/{ otVAR, otEMPTY, otEMPTY },
    /*sali*/{ otVAR, otEMPTY, otEMPTY },
    /*90*/
    /*sati*/{ otVAR, otEMPTY, otEMPTY },
    /*sapi*/{ otVAR, otEMPTY, otEMPTY },
    /*ssgi*/{ otVAR, otEMPTY, otEMPTY },
    /*ssli*/{ otVAR, otEMPTY, otEMPTY },
    /*ssti*/{ otVAR, otEMPTY, otEMPTY },
    /*sspi*/{ otVAR, otEMPTY, otEMPTY },
    /*+ag*/{ otVAR, otEMPTY, otEMPTY },
    /*+al*/{ otVAR, otEMPTY, otEMPTY },
    /*+at*/{ otVAR, otEMPTY, otEMPTY },
    /*+ap*/{ otVAR, otEMPTY, otEMPTY },
    /*100*/
    /*+sg*/{ otVAR, otEMPTY, otEMPTY },
    /*+sl*/{ otVAR, otEMPTY, otEMPTY },
    /*+st*/{ otVAR, otEMPTY, otEMPTY },
    /*+sp*/{ otVAR, otEMPTY, otEMPTY },
    /*+agi*/{ otVAR, otEMPTY, otEMPTY },
    /*+ali*/{ otVAR, otEMPTY, otEMPTY },
    /*+ati*/{ otVAR, otEMPTY, otEMPTY },
    /*+api*/{ otVAR, otEMPTY, otEMPTY },
    /*+sgi*/{ otVAR, otEMPTY, otEMPTY },
    /*+sli*/{ otVAR, otEMPTY, otEMPTY },
    /*110*/
    /*+sti*/{ otVAR, otEMPTY, otEMPTY },
    /*+spi*/{ otVAR, otEMPTY, otEMPTY },
    /*-ag*/{ otVAR, otEMPTY, otEMPTY },
    /*-al*/{ otVAR, otEMPTY, otEMPTY },
    /*-at*/{ otVAR, otEMPTY, otEMPTY },
    /*-ap*/{ otVAR, otEMPTY, otEMPTY },
    /*-sg*/{ otVAR, otEMPTY, otEMPTY },
    /*-sl*/{ otVAR, otEMPTY, otEMPTY },
    /*-st*/{ otVAR, otEMPTY, otEMPTY },
    /*-sp*/{ otVAR, otEMPTY, otEMPTY },
    /*120*/
    /*-agi*/{ otVAR, otEMPTY, otEMPTY },
    /*-ali*/{ otVAR, otEMPTY, otEMPTY },
    /*-ati*/{ otVAR, otEMPTY, otEMPTY },
    /*-api*/{ otVAR, otEMPTY, otEMPTY },
    /*-sgi*/{ otVAR, otEMPTY, otEMPTY },
    /*-sli*/{ otVAR, otEMPTY, otEMPTY },
    /*-sti*/{ otVAR, otEMPTY, otEMPTY },
    /*-spi*/{ otVAR, otEMPTY, otEMPTY },
};

OperandType filenameOperands[3] = { otDEBUGSTRING, otEMPTY, otEMPTY };
OperandType lineNumberOperands[3] = { otUINT16, otEMPTY, otEMPTY };

const OperandType *GetOperandTypes(const SCIVersion &version, Opcode opcode)
{
    if (version.PackageFormat == ResourcePackageFormat::SCI2)
    {
        switch (opcode)
        {
            case Opcode::Filename:
                return filenameOperands;
            case Opcode::LineNumber:
                return lineNumberOperands;
        }
    
        return OpArgTypes_SCI2[static_cast<BYTE>(opcode)];
    }
    else
    {
        return OpArgTypes_SCI0[static_cast<BYTE>(opcode)];
    }
}

// Corresponds to Opcode enum
char *OpcodeNames[130]={
	"bnot",
	"add",
	"sub",
	"mul",
	"div",
	"mod",
	"shr",
	"shl",
	"xor",
	"and",
	"or",
	"neg",
	"not",
	"eq?",
	"ne?",
	"gt?",
	"ge?",
	"lt?",
	"le?",
	"ugt?",
	"uge?",
	"ult?",
	"ule?",
	"bt",
	"bnt",
	"jmp",
	"ldi",
	"push",
	"pushi",
	"toss",
	"dup",
	"link",
	"call",
	"callk",
	"callb",
	"calle",
	"ret",
	"send",
    "INVALID",
    "INVALID",
	"class",
    "INVALID",
	"self",
	"super",
	"&rest",
	"lea",
	"selfID",  
    "INVALID",
	"pprev",
	"pToa",
	"aTop",
	"pTos",
	"sTop",
	"ipToa",
	"dpToa",
	"ipTos",
	"dpTos",
	"lofsa",
	"lofss",
	"push0",
	"push1",
	"push2",
	"pushSelf",  
    "INVALID",
	"lag",
	"lal",
	"lat",
	"lap",
	"lsg",
	"lsl",
	"lst",
	"lsp",
	"lagi",
	"lali",
	"lati",
	"lapi",
	"lsgi",
	"lsli",
	"lsti",
	"lspi",
	"sag",
	"sal",
	"sat",
	"sap",
	"ssg",
	"ssl",
	"sst",
	"ssp",
	"sagi",
	"sali",
	"sati",
	"sapi",
	"ssgi",
	"ssli",
	"ssti",
	"sspi",
	"+ag",
	"+al",
	"+at",
	"+ap",
/*100*/
	"+sg",
	"+sl",
	"+st",
	"+sp",
	"+agi",
	"+ali",
	"+ati",
	"+api",
	"+sgi",
	"+sli",
/*110*/
	"+sti",
	"+spi",
	"-ag",
	"-al",
	"-at",
	"-ap",
	"-sg",
	"-sl",
	"-st",
	"-sp",
/*120*/
	"-agi",
	"-ali",
	"-ati",
	"-api",
	"-sgi",
	"-sli",
	"-sti",
	"-spi",
    "_file_",
    "_line_",
};

const char *OpcodeToName(Opcode opcode)
{
    return OpcodeNames[(int)opcode];
}

std::unordered_set<std::string> opcodeSet;
std::unordered_set<std::string> &GetOpcodeSet()
{
    if (opcodeSet.empty())
    {
        opcodeSet.insert(OpcodeNames, OpcodeNames + ARRAYSIZE(OpcodeNames));
    }
    return opcodeSet;
}

Opcode NameToOpcode(const std::string &opcodeName)
{
    Opcode opcode = Opcode::INDETERMINATE;
    for (int i = 0; i < ARRAYSIZE(OpcodeNames); i++)
    {
        if (OpcodeNames[i] == opcodeName)
        {
            opcode = (Opcode)i;
        }
    }
    return opcode;
}

bool IsOpcode(const std::string &theString)
{
    GetOpcodeSet();
    return opcodeSet.find(theString) != opcodeSet.end();
}

SaidToken SaidArgs[] = {
    {',', 0xF0}, /*"OR". Used to specify alternatives to words, such as "take , get".*/
    {'&', 0xF1}, /*Unknown. Probably used for debugging.*/
    {'/', 0xF2}, /*Sentence part separator. Only two of these tokens may be used, since sentences are split into a maximum of three parts.*/
    {'(', 0xF3}, /*Used together with ')' for grouping*/
    {')', 0xF4}, /*See '('*/
    {'[', 0xF5}, /*Used together with '[' for optional grouping. "[ a ]" means "either a or nothing" */
    {']', 0xF6}, /*See '['.*/
    {'#', 0xF7}, /*Unknown. Assumed to have been used exclusively for debugging, if at all.*/
    {'<', 0xF8}, /*Semantic reference operator (as in "get < up").*/
    {'>', 0xF9}  /*Instructs Said() not to claim the event passed to the previous Parse() call on a match. Used for successive matching.*/
};