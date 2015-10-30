#pragma once

class DocScript;

// Returns core the filename generated (e.g. "View")
void OutputScriptRST(DocScript &docScript, const std::string &rstfolder, std::vector<std::string> &generatedFiles);
void OutputClassRST(DocScript &docScript, const std::string &rstFolder, std::vector<std::string> &generatedFiles);
void OutputProceduresRST(DocScript &docScript, const std::string &rstFolder, std::vector<std::string> &generatedFiles);