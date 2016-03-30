/* Copyright (c) 2002-2012 Croteam Ltd.
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "StdH.h"
#include "Main.h"

FILE *_fInput;
int _iLinesCt = 1;
char *_strInputFileName;
int _bTrackLineInformation=0;   // this is set if #line should be inserted in tokens
bool _bRemoveLineDirective = 0;


FILE *_fImplementation;
FILE *_fDeclaration;
FILE *_fTables;
FILE *_fExports;
char *_strFileNameBase;
char *_strFileNameBaseIdentifier;

extern FILE *yyin;

extern "C" int yywrap(void)
{
  return 1;
}
int ctErrors = 0;

char *stradd(char *str1, char *str2)
{
  char *strResult;
  strResult = (char*)malloc(strlen(str1)+strlen(str2)+1);
  strcpy(strResult, str1);
  strcat(strResult, str2);
  return strResult;
}
char *stradd(char *str1, char *str2, char *str3)
{
  char *strResult;
  strResult = (char*)malloc(strlen(str1)+strlen(str2)+strlen(str3)+1);
  strcpy(strResult, str1);
  strcat(strResult, str2);
  strcat(strResult, str3);
  return strResult;
}

char *LineDirective(int i)
{
  char str[256];
  sprintf(str, "\n#line %d %s\n", i, _strInputFileName);
  return strdup(str);
}

SType SType::operator+(const SType &other)
{
  SType sum;
  sum.strString = stradd(strString, other.strString);
  sum.iLine = -1;
  sum.bCrossesStates = bCrossesStates||other.bCrossesStates;
  return sum;
};

/*
 * Function used for reporting errors.
 */
void yyerror(const char *s)
{
  fprintf( stderr, "%s(%d): Error: %s\n", _strInputFileName, _iLinesCt, s);
  ctErrors++;
}

/*
 * Change the extension of the filename.
 */
char *ChangeFileNameExtension(const char *strFileName, const char *strNewExtension)
{
  char *strChanged = (char*)malloc(strlen(strFileName)+strlen(strNewExtension)+2);
  strcpy(strChanged, strFileName);
  char *pchDot = strrchr(strChanged, '.');
  if (pchDot==NULL) {
    pchDot = strChanged+strlen(strChanged);
  }
  strcpy(pchDot, strNewExtension);
  return strChanged;
}

/*
 * Open a file and report an error if failed.
 */
FILE *FOpen(const char *strFileName, const char *strMode)
{
  // open the input file
  FILE *f = fopen(strFileName, strMode);
  // if not successful
  if (f==NULL) {
    // report error
    fprintf(stderr, "Can't open file '%s': %s\n", strFileName, strerror(errno));
    //quit
    exit(EXIT_FAILURE);
  }
  return f;
}

/*
 * Print a header to an output file.
 */
static void PrintHeader(FILE *f)
{
  fprintf(f, "/*\n");
  fprintf(f, " * This file is generated by Entity Class Compiler, (c) CroTeam 1997-98\n");
  fprintf(f, " */\n");
  fprintf(f, "\n");
}

void TranslateBackSlashes(char *str)
{
  char *strNextSlash = str;
  while((strNextSlash = strchr(strNextSlash, '\\'))!=NULL) {
    *strNextSlash = '/';
  }
}

#define READSIZE  1024
/* Relpace File and remove #line directive from file */
void ReplaceFileRL(const char *strOld, const char *strNew)
{
  char strOldBuff[READSIZE*3+1];
  char strNewBuff[READSIZE+1];
  int iOldch=0;
  FILE *pfNew = NULL;
  FILE *pfOld = NULL;
  bool bQuotes = 0;
  bool bComment = 0;

  // open files
  pfNew = fopen(strNew,"rb");
  if(!pfNew) goto Error;
  pfOld = fopen(strOld,"wb");
  if(!pfOld) goto Error;

  // until eof
  while(!feof(pfNew))
  {
    // clear buffers
    memset(&strOldBuff,0,sizeof(strOldBuff));
    memset(&strNewBuff,0,sizeof(strNewBuff));

    iOldch = 0;
    bQuotes = 0;
    bComment = 0;

    // read one line from file
    int iRead = fread(strNewBuff,1,READSIZE,pfNew);
    char *chLineEnd = strchr(strNewBuff,13);
    if(chLineEnd) *(chLineEnd+2) = 0;
    // get line length
    int ctch = strlen(strNewBuff);
    int iSeek = -iRead+ctch;
    // seek file for extra characters read
    if(iSeek!=0) fseek(pfNew,iSeek ,SEEK_CUR);
    if(strncmp(strNewBuff,"#line",5)==0)
    {
      continue;
    }

    // process each charachter
    for(int ich=0;ich<ctch;ich++)
    {
      char *pchOld = &strOldBuff[iOldch];
      char *pchNew = &strNewBuff[ich];

      if((*pchNew == '{') || (*pchNew == '}') || *pchNew == ';')
      {
        if((!bComment) && (!bQuotes) && (*(pchNew+1) != 13))
        {
          strOldBuff[iOldch++] = strNewBuff[ich];
          strOldBuff[iOldch++] = 13;
          strOldBuff[iOldch++] = 10;
          continue;
        }
      }
      if(*pchNew == '"')
      {
        // if this is quote
        if((ich>0) && (*(pchNew-1)=='\\')) { }
        else bQuotes = !bQuotes;
      }
      else if((*pchNew == '/') && (*(pchNew+1) == '/'))
      {
        // if this is comment
        bComment = 1;
      }
      strOldBuff[iOldch++] = strNewBuff[ich];
    }
    fwrite(&strOldBuff,1,iOldch,pfOld);
  }

  if(pfNew) fclose(pfNew);
  if(pfOld) fclose(pfOld);
  remove(strNew);
  return;
Error:
  if(pfNew) fclose(pfNew);
  if(pfOld) fclose(pfOld);
}

/* Replace a file with a new file. */
void ReplaceFile(const char *strOld, const char *strNew)
{
  if(_bRemoveLineDirective)
  {
    ReplaceFileRL(strOld,strNew);
    return;
  }
  remove(strOld);
  rename(strNew, strOld);
}

enum ESStatus
{
    /* Appears to be non-empty, ready to parse. */
    Good,
    /* Appears to be empty, ignore it. */
    Empty,
    /* Error occured during status check. */
    Error,
};

/* Determine whether or not our target ES file is indeed valid input. */
ESStatus GetESStatus()
{
    ESStatus result = ESStatus::Empty;

    // Read a temporary buffer of the entire file contents
    fseek(_fInput, 0, SEEK_END);
    size_t length = ftell(_fInput);

    // Hard-stop on Empty out of paranoia
    if (length == 0)
        return result;

    char* temporaryBuffer = (char*)malloc(length);

    if (!temporaryBuffer)
        return ESStatus::Error;

    fseek(_fInput, 0, SEEK_SET);
    fread(temporaryBuffer, length, 1, _fInput);
    fseek(_fInput, 0, SEEK_SET);

    // Loop through each line
    char* currentSequence = strtok(temporaryBuffer, "\n");

    // No newlines, but it might still be valid.
    if (!currentSequence)
        currentSequence = temporaryBuffer;

    bool inBlockComment = false;
    do
    {
        size_t sequenceLength = strlen(currentSequence);

        for (size_t iteration = 0; iteration < sequenceLength; iteration++)
        {
            // If we're still in a block comment, find the closing */
            if (inBlockComment)
            {
                char* blockClosing = strstr(currentSequence, "*/");
                if (!blockClosing)
                    break;
                else
                {
                    inBlockComment = false;
                    iteration = ((size_t)blockClosing - (size_t)currentSequence) + 2;
                }
            }

            // If we find a // sequence, simply skip this line
            if (currentSequence[iteration] == '/' && currentSequence[iteration + 1] == '/')
                break;

            // If we find a /* on this line but not a closing */, skip this line
            if (currentSequence[iteration] == '/' && currentSequence[iteration + 1] == '*')
            {
                // Is there a closing */ on this line?
                char* blockClosing = strstr(currentSequence, "*/");

                if (!blockClosing)
                {
                    inBlockComment = true;
                    break;
                }
                else
                {
                    iteration = ((size_t)blockClosing - (size_t)currentSequence) + 2;
                    inBlockComment = false;
                    continue;
                }
            }

            if (iteration >= sequenceLength)
                break;

            // If we got to this point, we should be able to read only a number on this line
            for (size_t checkIteration = 0; checkIteration < sequenceLength; checkIteration++)
                if (currentSequence[checkIteration] != '\n' && currentSequence[checkIteration] != 0x20 && !isdigit(currentSequence[checkIteration]))
                {
                    result = ESStatus::Error;
                    break;
                }
                else if (currentSequence[checkIteration] != '\n' && currentSequence[checkIteration] != 0x20)
                    result = ESStatus::Good;

            free(temporaryBuffer);
            return result;
        }
    }
    while(currentSequence = strtok(NULL, "\n"));

    return result;
}

/* Replace a file with a new file if they are different.
 * Used to keep .h files from constantly changing when you change the implementation.
 */
void ReplaceIfChanged(const char *strOld, const char *strNew)
{
  int iChanged = 1;
  FILE *fOld = fopen(strOld, "r");
  if (fOld!=NULL) {
    iChanged = 0;
    FILE *fNew = FOpen(strNew, "r");
    while (!feof(fOld)) {
      char strOldLine[4096] = "#l";
      char strNewLine[4096] = "#l";

      // skip #line directives
      while(strNewLine[0]=='#' && strNewLine[1]=='l' && !feof(fNew)) {
        fgets(strNewLine, sizeof(strNewLine)-1, fNew);
      }
      while(strOldLine[0]=='#' && strOldLine[1]=='l' && !feof(fOld)) {
        fgets(strOldLine, sizeof(strOldLine)-1, fOld);
      }
      if (strcmp(strNewLine, strOldLine)!=0) {
        iChanged = 1;
        break;
      }
    }
    fclose(fNew);
    fclose(fOld);
  }

  if (iChanged) {
    remove(strOld);
    rename(strNew, strOld);
  } else {
    remove(strNew);
  }
}

int main(int argc, char *argv[])
{
  // if there is not one argument on the command line
  if (argc<1+1) {
    // print usage
    printf("Usage: Ecc <es_file_name>\n       -line\n");
    //quit
    return EXIT_FAILURE;
  }
  if(argc>2)
  {
    if(strcmp(argv[2],"-line")==0)
    {
      _bRemoveLineDirective=1;
    }
  }
  // open the input file
  _fInput = FOpen(argv[1], "r");

  // Make sure we're loading a valid ES file
  ESStatus status = GetESStatus();

  switch (status)
  {
      case ESStatus::Empty:
      {
          fclose(_fInput);
          return EXIT_SUCCESS;
      }
      case ESStatus::Error:
      {
          fclose(_fInput);
          printf("Ecc encountered an error during the es verification.\n");
          return EXIT_FAILURE;
      }
  }
  
  //printf("%s\n", argv[1]);
  // open all the output files
  char *strImplementation    = ChangeFileNameExtension(argv[1], ".cpp_tmp");
  char *strImplementationOld = ChangeFileNameExtension(argv[1], ".cpp");
  char *strDeclaration       = ChangeFileNameExtension(argv[1], ".h_tmp");
  char *strDeclarationOld    = ChangeFileNameExtension(argv[1], ".h");
  char *strTables            = ChangeFileNameExtension(argv[1], "_tables.h_tmp");
  char *strTablesOld         = ChangeFileNameExtension(argv[1], "_tables.h");

  _fImplementation = FOpen(strImplementation, "w");
  _fDeclaration    = FOpen(strDeclaration   , "w");
  _fTables         = FOpen(strTables        , "w");
  // get the filename as preprocessor usable identifier
  _strFileNameBase = ChangeFileNameExtension(argv[1], "");
  _strFileNameBaseIdentifier = strdup(_strFileNameBase);
  {char *strNextSlash = _strFileNameBaseIdentifier;
  while((strNextSlash = strchr(strNextSlash, '/'))!=NULL) {
    *strNextSlash = '_';
  }}
  {char *strNextSlash = _strFileNameBaseIdentifier;
  while((strNextSlash = strchr(strNextSlash, '\\'))!=NULL) {
    *strNextSlash = '_';
  }}
  // print their headers
  PrintHeader(_fImplementation );
  PrintHeader(_fDeclaration    );
  PrintHeader(_fTables         );

  // remember input filename
  char strFullInputName[MAXPATHLEN];
  _fullpath(strFullInputName, argv[1], MAXPATHLEN);
  _strInputFileName = strFullInputName;
  TranslateBackSlashes(_strInputFileName);
  // make lex use the input file
  yyin = _fInput;

  // parse input file and generate the output files
  yyparse();

  // close all files
  fclose(_fImplementation);
  fclose(_fDeclaration);
  fclose(_fTables);

  // if there were no errors
  if (ctErrors==0) {
    // update the files that have changed
    ReplaceFile(strImplementationOld, strImplementation);
    ReplaceIfChanged(strDeclarationOld, strDeclaration);
    ReplaceIfChanged(strTablesOld, strTables);

    return EXIT_SUCCESS;
  // if there were errors
  } else {
    // delete all files (the old declaration file is left intact!)
    remove(strImplementation);
    remove(strDeclaration   );
    remove(strTables        );
    return EXIT_FAILURE;
  }
}
