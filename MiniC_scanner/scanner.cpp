#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define NO_KEYWORDS 7
#define ID_LENGTH 12
FILE *fp;
enum tsymbol { tnull = -1,
	tnot,	tnotequ,	tmod,		tmodAssign,	tident,		tnumber,
	tand,	tlparen,	trparen,	tmul,		tmulAssign,	tplus,
	tinc,	taddAssign,	tcomma,		tminus,		tdec,		tsubAssign,
	tdiv,	tdivAssign,	tsemicolon,	tless,		tlesse,		tassign,
	tequal,	tgreat,		tgreate,	tlbracket,	trbracket,	teof,
	tconst,	telse,		tif,		tint,		treturn,	tvoid,
	twhile,	tlbrace,	tor,		trbrace
};
char *keyword[NO_KEYWORDS] = {
	"const","else","if","int","return","void","while" };
enum tsymbol tnum[NO_KEYWORDS] = {
	tconst,telse,tif,tint,treturn,tvoid,twhile
};
struct tokenType {
	int number;
	char id[ID_LENGTH] = { '\0', };
	int num;
	bool numflag=false;
};
int hexValue(char);

void lexicalError(int n)
{
	printf(" *** Lexical Error *** : ");
	switch (n) {
	case 1: printf("an identifier length must be less than %d.\n", ID_LENGTH);
		break;
	case 2: printf("next character must be &.\n");
		break;
	case 3: printf("next character must be |.\n");
		break;
	case 4: printf("invalid character!!!\n");
		break;
	}
}
int superLetter(char ch)
{
	if (isalpha(ch) || ch == '_') return 1;
	else return 0;
}
int superLetterOrDigit(char ch)
{
	if (isalnum(ch) || ch == '_') return 1;
	else return 0;
}
int getIntNum(char firstCharacter,char *content)
{
	int num = 0;
	int value;
	int index_source = 0;
	char ch;
	char source[ID_LENGTH] = { '\0', };

	if (firstCharacter != '0') {
		ch = firstCharacter;
		source[index_source++] = ch;
		do {
			num = 10 * num + (int)(ch - '0');
			ch = fgetc(fp);
			source[index_source++] = ch;
		} while (isdigit(ch));
	}
	else {
		ch = fgetc(fp);
		source[index_source++] = ch;
		if ((ch >= '0') && (ch <= '7'))
		{
			do {
				num = 8 * num + (int)(ch - '0');
				ch = fgetc(fp);
				source[index_source++] = ch;
			} while ((ch >= '0') && (ch <= '7'));
		}
		else if ((ch == 'x') || (ch == 'X')) {
			while ((value = hexValue(ch = fgetc(fp))) != -1)
			{
				source[index_source++] = ch;
				num = 16 * num + value;
			}
		}
		else num = 0;
	}
	ungetc(ch, fp);
	index_source--;
	source[index_source] = '\0';
	strcpy_s(content,ID_LENGTH, source);
	return num;
}
int hexValue(char ch)
{
	switch (ch) {
	case '0' : case '1' : case '2' : case '3': case '4':
	case '5' : case '6': case '7': case '8': case '9':
		return (ch - '0');
	case 'A' : case 'B': case 'C': case 'D': case 'E': case 'F':
		return (ch - 'A' + 10);
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		return (ch - 'a' + 10);
	default: return -1;
	}
}

tokenType scanner()
{
	tokenType token;
	int i, index,index_content=0;
	char ch, id[ID_LENGTH];
	char content[ID_LENGTH];

	token.number = tnull;
	token.num = 0;
	do {
		while (isspace(ch = fgetc(fp)));
		if (superLetter(ch)) {
			content[index_content++] = ch;
			i = 0;
			do {
				if (i < ID_LENGTH) id[i++] = ch;
				ch = fgetc(fp);
				content[index_content++] = ch;
			} while (superLetterOrDigit(ch));
			if (i >= ID_LENGTH) lexicalError(1);
			id[i] = '\0';
			ungetc(ch,fp);
			index_content--;
			for (index = 0; index < NO_KEYWORDS; index++)
				if (!strcmp(id, keyword[index])) break;
			if (index < NO_KEYWORDS)
				token.number = tnum[index];
			else {
				token.number = tident;
				strcpy_s(token.id, id);
			}
			content[index_content] = '\0';
		}
		else if (isdigit(ch)) {
			token.number = tnumber;
			token.numflag = true;
			token.num = getIntNum(ch,content);
		}
		else
		{
			switch (ch) {
			case '/':
				content[index_content++] = ch;
				ch = fgetc(fp);
				if (ch == '*')
				{
					index_content--;
					do {
						while (ch != '*') {
							ch = fgetc(fp);
						}
						ch = fgetc(fp);
					} while (ch != '/');
				}
				else if ((ch == '/'))
				{
					index_content--;
					while (fgetc(fp) != '/n');
				}
				else if (ch == '=')
				{
					content[index_content++] = ch;
					token.number = tdivAssign;
				}
				else {
					token.number = tdiv;
					ungetc(ch, fp);
				}
				break;
			case '!':
				content[index_content++] = ch;
				ch = fgetc(fp);
				content[index_content++] = ch;
				if (ch == '=')token.number = tnotequ;
				else {
					token.number = tnot;
					ungetc(ch, fp);
					index_content--;
				}
				break;
			case '%':
				content[index_content++] = ch;
				ch = fgetc(fp);
				content[index_content++] = ch;
				if (ch == '=')
					token.number = tmodAssign;
				else {
					token.number = tmod;
					ungetc(ch, fp);
					index_content--;
				}
				break;
			case '&':
				content[index_content++] = ch;
				ch = fgetc(fp);
				content[index_content++] = ch;
				if (ch == '&') token.number = tand;
				else {
					lexicalError(2);
					ungetc(ch, fp);
					index_content--;
				}
				break;
			case '*':
				content[index_content++] = ch;
				ch = fgetc(fp);
				content[index_content++] = ch;
				if (ch == '=') token.number = tmulAssign;
				else {
					token.number = tmul;
					ungetc(ch, fp);
					index_content--;
				}
				break;
			case '+':
				content[index_content++] = ch;
				ch = fgetc(fp);
				content[index_content++] = ch;
				if (ch == '+') token.number = tinc;
				else if (ch == '=') token.number = taddAssign;
				else
				{
					token.number = tplus;
					ungetc(ch, fp);
					index_content;
				}
				break;
			case '-':
				content[index_content++] = ch;
				ch = fgetc(fp);
				content[index_content++] = ch;
				if (ch == '-') token.number = tdec;
				else if (ch == '=') token.number = tsubAssign;
				else {
					token.number = tminus;
					ungetc(ch, fp);
					index_content--;
				}
				break;
			case '<':
				content[index_content++] = ch;
				ch = fgetc(fp);
				content[index_content++] = ch;
				if (ch == '=') token.number = tlesse;
				else {
					token.number = tless;
					ungetc(ch, fp);
					index_content--;
				}
				break;
			case '=':
				content[index_content++] = ch;
				ch = fgetc(fp);
				content[index_content++] = ch;
				if (ch == '=') token.number = tequal;
				else {
					token.number = tassign;
					ungetc(ch, fp);
					index_content--;
				}
				break;
			case '>':
				content[index_content++] = ch;
				ch = fgetc(fp);
				content[index_content++] = ch;
				if (ch == '=') token.number = tgreate;
				else {
					token.number = tgreat;
					ungetc(ch, fp);
					index_content--;
				}
				break;
			case '|':
				content[index_content++] = ch;
				ch = fgetc(fp);
				content[index_content++] = ch;
				if (ch == '|') token.number = tor;
				else {
					lexicalError(3);
					ungetc(ch, fp);
					index_content--;
				}
				break;
			case '(':
				content[index_content++] = ch;
				token.number = tlparen;
				break;
			case ')':
				content[index_content++] = ch;
				token.number = trparen;
				break;
			case ',':
				content[index_content++] = ch;
				token.number = tcomma;
				break;
			case ';':
				content[index_content++] = ch;
				token.number = tsemicolon;
				break;
			case '[':
				content[index_content++] = ch;
				token.number = tlbracket;
				break;
			case ']':
				content[index_content++] = ch;
				token.number = trbracket;
				break;
			case '{':
				content[index_content++] = ch;
				token.number = tlbrace;
				break;
			case '}':
				content[index_content++] = ch;
				token.number = trbrace;
				break;
			case EOF:
				token.number = teof;
				break;
			default: {
				printf("Current character : %c", ch);
				lexicalError(4);
				break;
			}
			}
			content[index_content] = '\0';
		}
	}while (token.number == tnull);
	printf("Token \t : %s\n", content);
	return token;
}

void main()
{
	fopen_s(&fp, "factorial.txt", "r");
	int c=0;
	char d;
	tokenType token;
	do
	{
		token = scanner();
		//Ãâ·Â
		if(token.numflag==true)
			printf("Token ID : %s\tToken number : %d\tToken num : %d\n", token.id, token.number, token.num);
		else
		{
			printf("Token ID : %s\tToken number : %d\n", token.id, token.number);
		}
		printf("***********************************************************\n");
	} while (token.number != teof);
}