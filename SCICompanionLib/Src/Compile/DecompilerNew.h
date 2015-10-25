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
#pragma once

bool OutputNewStructure(const std::string &messagePrefix, sci::FunctionBase &func, MainNode &main, DecompileLookups &lookups);

/*
    Decompilation summary

    0) For each function or method, we get the start and end code markers.

    1) In preparation for control flow analysis, the code is partitioned into blocks based on
        branch and jump instructions. These form the base blocks for control flow analysis.
        We also do some "instruction consumption" here and produce breaks prior to if statements,
        which isn't a good thing. With a more robust control flow analysis, we could probably avoid this.

    2) Then we do "control flow analysis". We identify loops, switch statements, compound conditions and
        if/else constructs. We do a bit of patchup in some cases. We try to stick with theorical
        control flow graph paradigms as much as possible, but sometimes we do SCI-dependent things, such
        as knowing that code is always sequential (i.e. we never branch backward except in loops).
        For debugging purposes VISUALIZE_FLOW can be uncommented to produce a graph file in .dot format
        so we can visualize the graph (essential for debugging). A good .dot visualizer is here:
        https://stamm-wilbrandt.de/GraphvizFiddle/

    3) The result of control flow analysis is a tree of nodes that represent the flow structures. We
        turn this into a tree of instruction consumption. e.g. an instruction that consumes two stack
        values will have two instructions that produce stack values as children. Sometimes we need
        to look outisde our control flow nodes to find the instructions to include as children. This is
        due (presumably) to optimizations Sierra's compiler did, or possibly to language-specific features
        that reused previous results in the accumulator or stack. In some cases this means we end up
        duplicating sections of the tree. 
        Note that control structures are included in the consumption tree. They generally act as
        an accumulator generator.

    4) From the instruction consumption tree, we can directly generate the SyntaxNode tree. These correspond
        directly to high level statements. It's here we can attempt to give meaningul names to variables and such.

    5) From the SyntaxNode tree, we can produce textual output in any of the supported languages.


*/