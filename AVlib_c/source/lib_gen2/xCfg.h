#pragma once
#ifndef _xCfgH_
#define _xCfgH_

#include "xLicense.h"
#include "xCommon.h"
#include "xCmp.h"

namespace AVlib {

class xCfgParam
{
protected:
  std::string              m_Name;
  std::vector<std::string> m_Args;
  std::string              m_Comment;

public:
  void                      setName(std::string Name) { m_Name = Name; }
  std::string               getName() { return m_Name; }

  void                      addArg(std::string Arg) { m_Args.push_back(Arg); }
  std::vector<std::string>& getArgs() { return m_Args; }
  std::string               getArg(int32 ArgIdx, std::string Default) { return m_Args.size()>ArgIdx ? m_Args[0] : Default; }
  std::string               get1stArg(std::string Default) { return getArg(0, Default); }
  double                    get1stArg(double      Default);
  int32                     get1stArg(int32       Default);
  bool                      get1stArg(bool        Default);

  void                      setComment(std::string Comment) { m_Comment = Comment; }
  std::string               getComment() { return m_Comment; }
};

//=============================================================================================

class xCfgSection
{
protected:
  std::string                        m_Name;
  std::map<std::string, xCfgParam>   m_Params;
  std::map<std::string, xCfgSection> m_SubSections;
  std::string                        m_Comment;

public:
  void                                setName(std::string Name) { m_Name = Name; }
  std::string                         getName() { return m_Name; }

  void                                addParam   (xCfgParam Param) { m_Params[Param.getName()] = Param; }
  std::map<std::string, xCfgParam>&   getParams  () { return m_Params; }
  bool                                findParam  (std::string ParamName) { return (m_Params.find(ParamName) != m_Params.end()); }
  xCfgParam&                          getParam   (std::string ParamName) { return m_Params[ParamName]; }
  std::string                         getParamArg(std::string ParamName, std::string Default) { return findParam(ParamName) ? getParam(ParamName).get1stArg(Default) : Default; } //operates on current section
  double                              getParamArg(std::string ParamName, double      Default) { return findParam(ParamName) ? getParam(ParamName).get1stArg(Default) : Default; } //operates on current section
  int32                               getParamArg(std::string ParamName, int32       Default) { return findParam(ParamName) ? getParam(ParamName).get1stArg(Default) : Default; } //operates on current section
  bool                                getParamArg(std::string ParamName, bool        Default) { return findParam(ParamName) ? getParam(ParamName).get1stArg(Default) : Default; } //operates on current section

  void                                addSection (xCfgSection Section) { m_SubSections[Section.getName()] = Section; }
  std::map<std::string, xCfgSection>& getSections() { return m_SubSections; }
  bool                                findSection(std::string SectionName) { return (m_SubSections.find(SectionName) != m_SubSections.end()); }
  xCfgSection&                        getSection (std::string SectionName) { return m_SubSections[SectionName]; }

  void                                setComment(std::string Comment) { m_Comment = Comment; }
  std::string                         getComment() { return m_Comment; }
};

//=============================================================================================

class xCfgParser
{
protected:
  xCfgSection       m_RootSection;
  std::stringstream m_Cfg;

protected:
  static inline bool xIsAlpha     (char c) { return ((c>='a' && c<='z') || (c>='A' && c<='Z')); }  
  static inline bool xIsNumeric   (char c) { return ( c>='0' && c <='9'); }
  static inline bool xIsAlphaNum  (char c) { return ((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c <='9')); }
  static inline bool xIsBlank     (char c) { return (c==' '  || c=='\a' || c=='\b' || c=='\t' || c=='\f'); }
  static inline bool xIsEndl      (char c) { return (c=='\n' || c=='\r' || c=='\v'); }
  static inline bool xIsSectName  (char c) { return (c=='['  || c==']'); }
  static inline bool xIsSection   (char c) { return (c=='{'  || c=='}'); }
  static inline bool xIsSeparator (char c) { return (c=='='  || c==':'); }
  static inline bool xIsComment   (char c) { return (c=='#'); }
  static inline bool xIsName      (char c) { return (!xIsBlank(c) && !xIsEndl(c) && !xIsSeparator(c) && !xIsComment(c)) && !xIsSectName(c) && !xIsSection(c); }

public:
  bool          loadFromCommandline(int argc, char* argv[], std::string CfgFileToken);
  void          loadFromFile       (std::string FileName);
  void          load               (std::string& buffer);
  void          storeToFile        (std::string FileName);
  void          store              (std::string& buffer);

protected:
  int32         xLoadSection       (xCfgSection& ParrentSection);
  int32         xLoadParam         (xCfgSection& ParrentSection, bool OneArgOnly);
  void          xStoreSection      (xCfgSection& Section);
  void          xStoreParam        (xCfgParam&   Param);

  int32         xSkipBlank         ();
  int32         xSkipBlankAndEndl  ();
  int32         xSkipAnyTillEndl   ();
  int32         xSkipAnyAndEndl    ();
  int32         xReadSectionName   (std::string &SectionName);
  int32         xReadParamName     (std::string &ParamName);
  int32         xReadParamArg      (std::string &ParamArg);
  int32         xReadComment       (std::string &Comment);

public:
  xCfgSection&  getRootSection() { return m_RootSection; }
  bool          findParam  (std::string ParamName) { return m_RootSection.findParam(ParamName); }; //operates on root section
  xCfgParam     getParam   (std::string ParamName) { return m_RootSection.getParam(ParamName); } //operates on root section
  std::string   getParamArg(std::string ParamName, std::string Default) { return m_RootSection.getParamArg(ParamName, Default); } //operates on root section
  double        getParamArg(std::string ParamName, double      Default) { return m_RootSection.getParamArg(ParamName, Default); } //operates on root section
  int32         getParamArg(std::string ParamName, int32       Default) { return m_RootSection.getParamArg(ParamName, Default); } //operates on root section
  bool          getParamArg(std::string ParamName, bool        Default) { return m_RootSection.getParamArg(ParamName, Default); } //operates on root section

public:
  static std::string replaceFirst(std::string& Source, const std::string& Token, const std::string& ReplaceTo);
  static std::string replaceLast (std::string& Source, const std::string& Token, const std::string& ReplaceTo);
  static std::string replaceAll  (std::string& Source, const std::string& Token, const std::string& ReplaceTo);
  static std::string formatString(const std::string &Fmt, ...);
};

} //end of namespace AVLib

#endif //_xCfgH_