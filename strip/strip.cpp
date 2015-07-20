// strip.cpp : Defines the entry point for the console application.
// Martin Brunn 20150720

#include "stdafx.h"

#include <string.h>

#ifdef NEVER
int workline(char * buffer, char * out, bool & bComment,bool & bString)
{
	char * cp;
	if (*buffer == 0)
		return 0;
	if (bString)
	{
		char * e = strchr(buffer, '\"');
		if (e && (e == buffer || (*(e-1) != '\\')))
			return 0 ;
		bString = false;
		return workline(e + 1, out, bComment, bString);
	}
	if (bComment)
	{
		cp = strstr(buffer, "*/");
		if (cp)
		{
			cp += 2;
			bComment = false;
			return workline(cp, out, bComment,bString);
		}
		return 0;
	}


	cp = strstr(buffer, "//");
	if (cp && cp != buffer && *(cp - 1) != '\\')
	{
		char * s = strchr(buffer, '\"');
		if (s && s < cp && (s == buffer || (*(s - 1) == '\\')))
		{
			char * e = strchr(s + 1, '\"');
			if (!e)
			{
				bString = true;
				return 0;
			}
			else if (e == s + 1 || (*(e-1) == '\\'))
			{

			}
		}

		*cp = 0;
	}
	else if (cp  && cp == buffer)
		*cp = 0;
	cp = strstr(buffer, "/*");
	if (cp && (cp == buffer || (cp != buffer && *(cp - 1) != '\\')))
	{
		bComment = true;
		int len = cp - buffer;
		strncpy(out, buffer, len);
		out[len] = 0;
		fprintf(stdout, "%s", out);
		return workline(cp + 2, out, bComment);
	}
	else
	{
		strcpy(out, buffer);
	}
	fprintf(stdout, "%s\n", out);

}
#endif

int work(char*cp,char*out,bool & bLineComment,bool & bComment,bool & bString,bool & bChar)
{
	if (bComment)
	{
		while (*cp && *(cp + 1))
		{
			if (*cp == '*' && *(cp + 1) == '/')
			{
				cp += 2;
				bComment = false;
				break;
			}
			cp++;
		}
		if (bComment)
			return 0;
	}
	while (*cp != 0)
	{
		switch (*cp)
		{
		case '\\':
			*out = *cp;
			cp++;
			out++;
			*out = *cp;
			out++;
			break;
		case '\"':
			if (bString)
			{
				bString = false;
			}
			else
				bString = true;
			*out = *cp;
			out++;
			break;
		case '\'':
			if (bChar)
				bChar = false;
			else
				bChar = true;
			*out = *cp;
			out++;
			break;
		case '/':
			if (bString || bChar)
			{
				*out = *cp;
				out++;
			}
			else if (*(cp + 1) == '/')
			{
				while (*cp)
					cp++;
				bLineComment = true;
				*out = 0;
				return 0;
			}
			else if (*(cp + 1) == '*')
			{
				cp+=2;
				bComment = true;

				while (*cp && *(cp + 1))
				{
					if (*cp == '*' && (*(cp + 1) == '/'))
					{
						cp += 2;
						bComment = false;
						break;
					}
					cp++;
				}
				*out = 0;
				if (bComment)
				{
					return 0;
				}
				continue;
			}
			else
			{
				*out = *cp;
				out++;
			}
			break;
		default:
			*out = *cp;
			++out;
		}
		cp++;
		*out = 0;
	}
	*out = 0;
	return 0;
}
int processFile(wchar_t * in, wchar_t * out);
int _tmain(int argc, _TCHAR** argv)
{
	wchar_t * out = nullptr ;
	while (--argc)
	{
		if (!_wcsicmp(*++argv, L"-out"))
		{
			if (!--argc)
				break;
			out = *++argv;
			if (!--argc)
				break;
			++argv;
		}
		return processFile(*argv,out);
	}
	return processFile(nullptr, nullptr);
}
int process(FILE*fIn, FILE*fOut);
int processFile(wchar_t * in, wchar_t * out)
{
	FILE * fDoIn = stdin;
	FILE *fDoOut = stdout;
	FILE * fIn = nullptr;
	FILE *fOut = nullptr;
	if (in && _wfopen_s(&fIn, in, L"r"))
	{
		fprintf(stderr, "error opening file");
		return 0;
	}
	if (fIn)
	{
		fDoIn = fIn;
	}
	if (out && _wfopen_s(&fOut, out, L"w"))
	{
		fclose(fOut);
		fprintf(stderr, "error opening file");
		return 0;
	}
	if (fOut)
	{
		fDoOut = fOut;
	}
	process(fDoIn, fDoOut);
	if (fIn)
		fclose(fIn);
	if (fOut)
		fclose(fOut);
	return 0;
}
int process(FILE*fIn,FILE*fOut)
{
	char buffer[4096];
	char out[4096];
	bool bComment = false;
	bool bString = false;
	bool bLineComment = false;
	bool bChar = false;
	while (fgets(buffer, sizeof(buffer), fIn))
	{
		bool bTakeCare = false;
		char * cp = strchr(buffer, '\n');
		if (cp)
		{
			*cp = 0;
			if (bLineComment)
			{
				bLineComment = false;
				continue;
			}
		}
		else
			bTakeCare = true;
		if (bLineComment)
			continue;
		work(buffer, out, bLineComment, bComment, bString, bChar);
		if (!bComment || strlen(out))
		{
			fprintf(fOut, "%s", out);
			*out = 0;
			if (!bTakeCare)
			{
				fprintf(fOut, "\n");
				bLineComment = false;
			}
		}
	}
	return 0;
}

