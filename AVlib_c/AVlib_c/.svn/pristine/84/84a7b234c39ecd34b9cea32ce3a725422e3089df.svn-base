#include "xCfg.h"

namespace AVlib {

using std::string;
using std::vector;
using std::map;
using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::istringstream;
using std::ostringstream;

//=============================================================================================

double xCfgParam::get1stArg(double Default)
{
  string Str = get1stArg(string(""));
  std::istringstream IStrStream(Str, istringstream::in);
  if(Str.length() > 0) IStrStream >> Default;
  return Default;
}
int32 xCfgParam::get1stArg(int32 Default)
{
  string Str = get1stArg(string(""));
  std::istringstream IStrStream(Str, istringstream::in);
  if(Str.length() > 0) IStrStream >> Default;
  return Default;
}
bool xCfgParam::get1stArg(bool Default)
{
  string Str = get1stArg(string(""));
  std::istringstream IStrStream(Str, istringstream::in);
  if(Str.length() > 0) IStrStream >> Default;
  return Default;
}

//=============================================================================================

bool xCfgParser::loadFromCommandline(int argc, char* argv[], string CfgFileToken)
{
  if(argc == 1)
  {
    fprintf(stderr,"cfg: no commandline arguments!\n");
    return false;
  }

  for(int i=1; i<argc; i++)
  {
    string OneArg = argv[i];
    string CfgFileName;
    if(OneArg.find(CfgFileToken) != string::npos)
    {
      m_Cfg.str("");
      m_Cfg.clear();
      m_Cfg << OneArg;
      m_Cfg.ignore(1024,'=');
      xReadParamArg(CfgFileName);
      loadFromFile(CfgFileName);
    }
  } 

  for(int i=1; i<argc; i++)
  {
    string OneArg = argv[i];
    if(xIsAlpha(OneArg[0]) && OneArg.find(CfgFileToken) == string::npos)
    {
      m_Cfg.str("");   
      m_Cfg.clear();
      m_Cfg << OneArg;
      xLoadParam(m_RootSection, true);
    }
  }  
  return true;
}
void xCfgParser::loadFromFile (string FileName)
{
  ifstream File (FileName.c_str(), ifstream::in);
  string Buffer ((std::istreambuf_iterator<char>(File)), std::istreambuf_iterator<char>());
  File.close();
  load(Buffer);  
}
void xCfgParser::load(string& Buffer)
{
  m_Cfg.str("");
  m_Cfg.clear();
  m_Cfg << Buffer;
  int32 RetVal = 0;

  m_RootSection.setName("Root");
  //fprintf(stdout, "V xCfg: Loading cfg\n");
  while (!m_Cfg.eof())
  {
    char Token = m_Cfg.peek();
    if(xIsBlank(Token) || xIsEndl(Token) || Token==EOF) //whitespace
    {
      m_Cfg.ignore(1); //skip one char
    }
    else if(Token == '#') //comment
    {
      string Comment;
      xReadComment(Comment);
    }
    else if(Token == '[')
    {
      RetVal = xLoadSection(m_RootSection);
    }
    else if(Token == '$') //parameter with $ prefix
    {
      m_Cfg.ignore(1);
      RetVal = xLoadParam(m_RootSection, false);
    }
    else if(xIsAlpha(Token)) //param  
    {
      RetVal = xLoadParam(m_RootSection, false);
    }
    else
    {
      fprintf(stderr,"cfg: wrong start symbol!\n");
      break;
    } 
    if(RetVal==-1)
    {
      xSkipAnyAndEndl(); //skip to the end of line
    }
  }
}
int32 xCfgParser::xLoadSection(xCfgSection &ParrentSection)
{
  char        Token;  
  int32       RetVal;
  xCfgSection Section;

  //fprintf(stdout, "V xCfg: Loading section: ");
  
  //section name extraction
  string SectionName;
  RetVal = xReadSectionName(SectionName);
  if (RetVal) { fprintf(stderr, "cfg: wrong section name"); return RetVal; }
  Section.setName(SectionName);
  //fprintf(stdout, "name=%s \n", SectionName.c_str());

  xSkipBlank();
  Token = m_Cfg.peek();
  if(Token == '#') //comment
  {
    string Comment;
    xReadComment(Comment);
    Section.setComment(Comment);
  }
  xSkipBlankAndEndl();  

  //load section body
  Token = m_Cfg.get();
  if(Token != '{') return -1;
  xSkipBlankAndEndl();

  while (!m_Cfg.eof())
  {
    char Token = m_Cfg.peek();
    if(xIsBlank(Token) || xIsEndl(Token) || Token==EOF) //whitespace
    {
      m_Cfg.ignore(1); //skip one char
    }
    else if(Token == '#') //comment
    {
      string Comment;
      xReadComment(Comment);
    }
    else if(Token == '}') //end of the section
    {
      m_Cfg.ignore(1);
      ParrentSection.addSection(Section);
      //fprintf(stdout, "V xCfg: End of section\n");
      break;
    }
    else if(Token == '[')
    {
      RetVal = xLoadSection(Section);
    }
    else if(Token == '$') //parameter with $ prefix
    {
      m_Cfg.ignore(1);
      RetVal = xLoadParam(Section, false);
    }
    else if(xIsAlpha(Token)) //param  
    {
      RetVal = xLoadParam(Section, false);
    }
    else
    {
      fprintf(stderr,"cfg: wrong start symbol!\n");
      return -1;
    } 
    if(RetVal==-1)
    {
      xSkipAnyAndEndl(); //skip to the end of line
    }
  }
  return 0;
}
int32 xCfgParser::xLoadParam(xCfgSection &ParrentSection, bool OneArgOnly)
{
  char      Token;  
  int32     RetVal;
  string    Separator;
  xCfgParam Param;

  //fprintf(stdout, "V xCfg: Loading parameter; ");

  //param name extraction
  string ParamName;
  RetVal = xReadParamName(ParamName);
  if (RetVal) { fprintf(stderr, "cfg: wrong parameter name"); return RetVal; }
  Param.setName(ParamName);
  //fprintf(stdout, "name=%s ", ParamName.c_str());

  //separator extraction
  RetVal = xSkipBlank();
  Token = m_Cfg.peek();
  if(xIsSeparator(Token))
  {
    Separator = Token;
    m_Cfg.ignore();
    RetVal = xSkipBlank();
  }

  //arguments extraction
  Token = m_Cfg.get();
  if(xIsEndl(Token) || xIsSeparator(Token) || xIsComment(Token)) { fprintf(stderr, "cfg: no argument found"); return -1; };
  
  do
  {
    m_Cfg.unget();
    string ParamArg;
    xReadParamArg(ParamArg);
    Param.addArg(ParamArg);
    //fprintf(stdout, "arg=%s ", ParamArg.c_str());
    xSkipBlank();
    Token = m_Cfg.get();
  }
  while (!xIsEndl(Token) && !xIsComment(Token) && !m_Cfg.eof() && !(OneArgOnly && Param.getArgs().size()>0));
  m_Cfg.unget();

  if(Token == '#') //comment
  {
    string Comment;
    xReadComment(Comment);
    Param.setComment(Comment);
  }

  xSkipAnyAndEndl(); //skip to the end of line  

  ParrentSection.addParam(Param);

  //fprintf(stdout, "\n");
  return 0;  
}
int32 xCfgParser::xSkipBlank()
{
  char Token = m_Cfg.get();
  while(xIsBlank(Token))
  {
    Token = m_Cfg.get();
  }
  if(!m_Cfg.eof()) m_Cfg.unget();
  if(xIsEndl(Token) || m_Cfg.eof()) return -1;
  return 0;
}
int32 xCfgParser::xSkipBlankAndEndl()
{
  char Token = m_Cfg.get();
  while(xIsBlank(Token) || xIsEndl(Token))
  {
    Token = m_Cfg.get();
  }
  if(m_Cfg.eof()) return -1;
  m_Cfg.unget();
  return 0;
}
int32 xCfgParser::xSkipAnyTillEndl()
{
  char Token = m_Cfg.get();
  while(!xIsEndl(Token) && !m_Cfg.eof())
  {
    Token = m_Cfg.get();
  }
  if(m_Cfg.eof()) return -1;
  m_Cfg.unget();
  return 0;
}
int32 xCfgParser::xSkipAnyAndEndl()
{
  char Token = m_Cfg.get();
  while(!xIsEndl(Token) && !m_Cfg.eof())
  {
    Token = m_Cfg.get();
  }
  if(m_Cfg.eof()) return -1;
  return 0;
}
int32 xCfgParser::xReadSectionName(string &SectionName)
{
  char Token = m_Cfg.get();
  if(Token != '[') return -1;

  xSkipBlank();

  Token = m_Cfg.get();
  if(!xIsName(Token)) return -1;

  while(xIsName(Token))
  {
    SectionName += Token;
    Token = m_Cfg.get();
  }
  m_Cfg.unget();

  if(SectionName.length()==0) return -1;

  xSkipBlank();

  Token = m_Cfg.get();
  if(Token != ']') return -1;

  return 0;
}
int32 xCfgParser::xReadParamName(string &ParamName)
{
  char Token = m_Cfg.get();
  if(!xIsName(Token)) return -1;  

  while(xIsName(Token))
  {
    ParamName += Token;
    Token = m_Cfg.get();
  }
  m_Cfg.unget();

  if(ParamName.length()==0) return -1;
  
  return 0;
}
int32 xCfgParser::xReadParamArg(string &ParamArg)
{
  char Token = m_Cfg.get();

  if(Token=='"' || Token=='\'')
  {
    char QuoteMark = Token;
    Token = m_Cfg.get();
    while (!xIsEndl(Token) && Token!=QuoteMark && !m_Cfg.eof())
    {
      ParamArg += Token;
      Token = m_Cfg.get();
    }
    if(!m_Cfg.eof()) m_Cfg.unget();
    if(!xIsEndl(Token)) return -1;
  }
  else
  {  
    while (xIsName(Token) && !m_Cfg.eof())
    {
      ParamArg += Token;
      Token = m_Cfg.get();
    }
    if(!m_Cfg.eof()) m_Cfg.unget();
  } 

  if (ParamArg.length()==0) return -1;

  return 0;
}
int32 xCfgParser::xReadComment(string &Comment)
{ 
  char Token = m_Cfg.get();
  if(Token != '#') return -1;

  xSkipBlank();
  Token = m_Cfg.get();
  while(!xIsEndl(Token) && !m_Cfg.eof())
  {
    Comment += Token;
    Token = m_Cfg.get();
  }

  if(!m_Cfg.eof()) m_Cfg.unget();
  return 0;
} 

//=============================================================================================

void xCfgParser::storeToFile(std::string FileName)
{
  string Buffer;
  store(Buffer);
  ofstream File (FileName.c_str(), ofstream::out);
  File << Buffer;  
  File.close();
}
void xCfgParser::store(std::string& Buffer)
{
  m_Cfg.str("");
  m_Cfg.clear();

  xStoreSection(m_RootSection);

  Buffer = m_Cfg.str();
}
void xCfgParser::xStoreSection(xCfgSection& Section)
{
  for(map<string, xCfgParam>::iterator ParamIter = Section.getParams().begin(); ParamIter != Section.getParams().end(); ParamIter++)
  {
    xCfgParam& Param = ParamIter->second;
    xStoreParam(Param);
  }

  for(map<string, xCfgSection>::iterator SectionIter = Section.getSections().begin(); SectionIter != Section.getSections().end(); SectionIter++)
  {
    xCfgSection& SubSection = SectionIter->second;
    m_Cfg << "[" << SubSection.getName() << "]";
    
    if(!SubSection.getComment().empty())
    {
      m_Cfg << "# " << SubSection.getComment();
    }

    m_Cfg << std::endl << "{" << std::endl;
    xStoreSection(SubSection);
    m_Cfg << "}" << std::endl;
  }
}
void xCfgParser::xStoreParam(xCfgParam& Param)
{
  m_Cfg << Param.getName() << " = ";

  vector<string>& Args = Param.getArgs();
  for(vector<string>::iterator ArgIter = Args.begin(); ArgIter != Args.end(); ArgIter++)
  {
    m_Cfg << (*ArgIter) << " ";
  }

  if(!Param.getComment().empty())
  {
    m_Cfg << "# " << Param.getComment();
  }
  m_Cfg << "\n";
}

//=============================================================================================

string xCfgParser::replaceFirst(string& Source, const string& Token, const string& ReplaceTo)
{
  string Result = Source;
  size_t CurrentPos = Result.find(Token);
  if(CurrentPos != std::string::npos)
  {
    Result.replace(CurrentPos, Token.length(), ReplaceTo);      
  }
  return Result;
}
string xCfgParser::replaceLast(string& Source, const string& Token, const string& ReplaceTo)
{
  string Result = Source;
  size_t CurrentPos = Result.rfind(Token);
  if(CurrentPos != std::string::npos)
  {
    Result.replace(CurrentPos, Token.length(), ReplaceTo);      
  }
  return Result;
}
string xCfgParser::replaceAll(string& Source, const string& Token, const string& ReplaceTo)
{
  string Result = Source;
  size_t CurrentPos = 0;
  while((CurrentPos = Result.find(Token, CurrentPos)) != std::string::npos)
  {
    Result.replace(CurrentPos, Token.length(), ReplaceTo);
    CurrentPos += ReplaceTo.length();
  }
  return Result;
}
string xCfgParser::formatString(const string &fmt, ...)
{
  int Size = 100;
  string Result;
  va_list ap;
  while (1)
  {
    Result.resize(Size);
    va_start(ap, fmt);
    int n = vsnprintf((char*)Result.c_str(), Size, fmt.c_str(), ap);
    va_end(ap);
    if (n > -1 && n < Size) {
      Result.resize(n);
      return Result;
    }
    if (n > -1)
      Size = n + 1;
    else
      Size *= 2;
  }
  return Result;
}

} //end of namespace AVLib
