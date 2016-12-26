#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#define OPNUM 54
/*
 * Tokenizer struct with field input that takes in argv[1], currentindex that keeps track of position in input, currenttypeindex that keeps track of type of most recent token
 */
struct TokenizerT_ {
    char *input;
    int currentindex;
    int currenttypeindex;
};
typedef struct TokenizerT_ TokenizerT;

/*
*equivalent to a substring type function, copies string from certain begin index to end index and stores in new string
*/
char* copythat(char *original, int startindex, int endindex){
    int i;
    int k;
    char * finale= (char*)malloc((strlen(original)+2)*sizeof(char));
    for(i=0,k=startindex;i<strlen(original) && k<endindex;i++,k++){
                finale[i]= original[k];
    }
return finale;
}

/*
*Determines whether character input is valid octal
*/
int isOct(char input){
    if(input=='0' || input=='1' || input=='2' || input=='3' || input=='4' || input=='5' || input=='6' || input=='7'){
        return 1;
    }
    else{
        return 0;
    }
}
/*
*Determines whether character input is valid hex
*/
int isHex(char input){
    if(isdigit(input)!=0 || input == 'A' || input == 'B' || input == 'C' || input == 'D' || input == 'E' || input == 'F' || input == 'a' || input == 'b' || input == 'c' || input == 'd' || input == 'e' || input == 'f'){
        return 1;
    }
    else
        return 0;
}
/*
*Function that is called when token is determined to be float and contains 'e' or 'E' and returns appropriate float token
*/
char *Efloat(TokenizerT *tk, char* ftoken, int numindex){
    long int i;
    long int start;
    int count;
    for(i=tk->currentindex,start=numindex;i<strlen(tk->input) && start < strlen(tk->input);i++,start++){
        if(isdigit(tk->input[i])!=0 || tk->input[i]=='-' || tk->input[i]=='+'){
            ftoken[start]= tk->input[i];
        }
        else{
            break;
        }
    }
    if(tk->input[i-1]=='e' || tk->input[i-1]=='E' || tk->input[i-1]=='-' || tk->input[i-1]=='+'){
        tk->currenttypeindex = 45;
        tk->currentindex=i+1;
        return ftoken;
    }
    tk->currentindex=i;
    tk->currenttypeindex = 49;
    return ftoken;
}

/*
*Function that is called when it is determined that token is a float and contains '.', and checks for 'e' or 'E', if found enters Efloat, otherwise returns float token
*/
char * Eordotfloat(TokenizerT *tk, char* ftoken, int numindex){
    long int i;
    long int start;
    int count;
    for(i=tk->currentindex,start=numindex;i<strlen(tk->input) && start < strlen(tk->input);i++,start++){
        if(start==numindex && isdigit(tk->input[start])==0 && tk->input[start]!='e' && tk->input[start]!='E'){
                tk->currenttypeindex = 45;
                tk->currentindex=i;
                return ftoken;
            } 
        else if(tk->input[i]=='e' || tk->input[i]=='E'){
            ftoken[start] = tk->input[i];
            tk->currentindex=i+1;
            return Efloat(tk,ftoken,start+1);
        }
        else if(isdigit(tk->input[i])!=0){
            ftoken[start]= tk->input[i];
        }
        else{
            break;
        }
    }
    tk->currentindex=i;
    tk->currenttypeindex = 49;
    return ftoken;
}

/*
*Function that is called when it is determined that number is not hex or octal, checks for float or decimal, enters appropriate float functions when needed, otherwise returns decimal token
*/
char *decimalorfloatstring(TokenizerT *tk){
    char *num = malloc(strlen(tk->input)*sizeof(char));
    long int i;
    long int start;
    int count;
    for(i=tk->currentindex,start=0;i<strlen(tk->input) && start < strlen(tk->input);i++,start++){
            if(tk->input[i]=='.'){
                if(i!=strlen(tk->input)-1){
                    if(tk->input[i+1]=='e' || tk->input[i+1]=='E' || tk->input[i+1]=='.'){
                        num[start]= tk->input[i];
                        num[start+1]= tk->input[i+1];
                        tk->currenttypeindex = 45;
                        tk->currentindex=i+2;
                        return num;
                    }
                }
                num[start] = tk->input[i];
                tk->currentindex=i+1;
                return Eordotfloat(tk,num,start+1);
            }
            else if(tk->input[i]=='e' || tk->input[i]=='E'){
                if(i!=strlen(tk->input)-1){
                    if(tk->input[i+1]=='e' || tk->input[i+1]=='E' || tk->input[i+1]=='.'){
                        num[start]= tk->input[i];
                        num[start+1]= tk->input[i+1];
                        tk->currenttypeindex = 45;
                        tk->currentindex=i+1;
                        return num;
                    }
                }
                num[start] = tk->input[i];
                tk->currentindex=i+1;
                return Efloat(tk,num,start+1);
            }
            else if(isdigit(tk->input[i])!=0){
                num[start]= tk->input[i];
            }
            else{
                break;
            }
    }
    tk->currenttypeindex = 46;
    tk->currentindex=i;
    return num;
}

/*
*Function that is called when first character of number token is 0, or when first two characters are 0x or 0X, and returns corresponding octal or hex tokens
*/
char *hexoroctnumstring(TokenizerT *tk, char *number){
    long int i;
    long int start;
    if(tk->currenttypeindex==48){
        for(i=(tk->currentindex)+2, start=2 ;i<strlen(tk->input) && start<strlen(tk->input);i++,start++){
            if(isHex(tk->input[i])==1){
                number[start] = tk->input[i];
            }
            else{
                break;
            } 
        }

    }
    else if(tk->currenttypeindex==47){
        for(i=(tk->currentindex)+1, start=1 ;i<strlen(tk->input) && start<strlen(tk->input);i++,start++){
            if(isOct(tk->input[i])==1){
                number[start] = tk->input[i];
            }
            else{
                break;
            }
        }
    }

    tk->currentindex = i;
        return number;
}

/*
*Function that is called when first character of token is a digit, and determines token type and uses other functions to return token and type
*/
char *number(TokenizerT *tk){
    char *num = (char*)malloc(strlen(tk->input)*sizeof(char));
    long int i;
    int j;
    i= tk->currentindex;
    if(tk->input[i]== '0'){
        num[0] = tk->input[i];
        if(i!= strlen(tk->input)-1 && (tk->input[i+1]=='x' || tk->input[i+1]=='X')){
            num[1] = tk->input[i+1];
            if(i!=strlen(tk->input)-2){
                if(isHex(tk->input[i+2])==0){
                    tk->currenttypeindex = 45;
                    if(tk->input[i+1]=='x'){
                        tk->currentindex=i+2;
                        return "0x";
                    }
                    else{
                        tk->currentindex=i+2;
                        return "0X";
                    }
                }
                else{
                    tk->currenttypeindex = 48;
                    return hexoroctnumstring(tk,num);
                }
            
        }   

    }
        else if((isOct(tk->input[i+1])==1)){
            tk->currenttypeindex = 47;
            return hexoroctnumstring(tk,num);
        }
    }
    return decimalorfloatstring(tk);
}

/*
 * TKCreate creates a new TokenizerT object for a given token stream
 * (given as a string).
 *
 * TKCreate should copy the arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * 
 */

 /*
 *Function that creates Tokenizer object, sets fields of Tokenizer to standard beginning
 */
TokenizerT *TKCreate( char * ts ) {
    TokenizerT *tk;
    tk =(TokenizerT*)malloc(sizeof(TokenizerT));
    tk->currentindex= 0;
    tk->currenttypeindex= -1;
    tk->input= (char*)malloc((strlen(ts)+1)*sizeof(char));
    strncpy(tk->input,ts,strlen(ts));
    return tk;
}

/*
*Function that checks validity of single operator.  Returns index of token name array if valid, 45 if not
*/
int singleopchecker(char character){
    int i;
    i = 0;
    char opString[24] = "()[].*&-!~/%+<>&^|,=?:";
    for(i=0;i<strlen(opString);i++){
        if (opString[i] == character){
            return i;
        }
    }
    return 45;
}

/*
*Function that checks validity of double character operator.  Returns index of token name array if valid, 45 if not
*/
int doubleopchecker(char *dops){
    if (strcmp(dops,"++")==0){
        return 41;
    }
    else if (strcmp(dops,"--")==0){
        return 53;
    }
    else if (strcmp(dops,">>")==0){
        return 22;
    }
    else if (strcmp(dops,"<<")==0){
        return 23;
    }
    else if (strcmp(dops,"<=")==0){
        return 24;
    }
    else if (strcmp(dops,">=")==0){
        return 25;
    }
    else if (strcmp(dops,"==")==0){
        return 26;
    }
    else if (strcmp(dops,"!=")==0){
        return 40;
    }
    else if (strcmp(dops,"&&")==0){
        return 27;
    }
    else if (strcmp(dops,"||")==0){
        return 28;
    }
    else if (strcmp(dops,"+=")==0){
        return 29;
    }
    else if (strcmp(dops,"-=")==0){
        return 30;
    }
    else if (strcmp(dops,"*=")==0){
        return 31;
    }
    else if (strcmp(dops,"/=")==0){
        return 32;
    }
    else if (strcmp(dops,"%=")==0){
        return 33;
    }
    else if (strcmp(dops,"&=")==0){
        return 36;
    }
    else if (strcmp(dops,"^=")==0){
        return 37;
    }
    else if (strcmp(dops,"|=")==0){
        return 38;
    }
    else if(strcmp(dops,"->")==0){
        return 39;
    }
    else if(strcmp(dops,"/*")==0){
        return 74;
    }
    else if(strcmp(dops,"//")==0){
        return 75;
    }
    else return 45;
    
}

/*
*Function that checks validity of triple character operator.  Returns index of token name array if valid, 45 if not
*/
int tripleopchecker(char *trops){
    if (strcmp(trops,">>=")==0){
        return 34;
    }
    else if (strcmp(trops,"<<=")==0){
        return 35;
    }
    else{
        return 45;
    }
}

/*
*Function that creates array of character arrays containing C keywords
*/
 char **keys(){
    int i;
    char **keywords = (char**)malloc(32*sizeof(char*));
    for(i=0;i<OPNUM;i++){
        keywords[i]=(char*)malloc(10*sizeof(char));
    }
    keywords[0] = "auto";
    keywords[1] = "break";
    keywords[2] = "case";
    keywords[3] = "char";
    keywords[4] = "const";
    keywords[5] = "continue";
    keywords[6] = "default";
    keywords[7] = "do";
    keywords[8] = "double";
    keywords[9] = "else";
    keywords[10] = "enum";
    keywords[11] = "extern";
    keywords[12] = "float";
    keywords[13] = "for";
    keywords[14] = "goto";
    keywords[15] = "if";
    keywords[16] = "int";
    keywords[17] = "long";
    keywords[18] = "register";
    keywords[19] = "return";
    keywords[20] = "short";
    keywords[21] = "signed";
    keywords[22] = "sizeof";
    keywords[23] = "static";
    keywords[24] = "struct";
    keywords[25] = "switch";
    keywords[26] = "typedef";
    keywords[27] = "union";
    keywords[28] = "unsigned";
    keywords[29] = "void";
    keywords[30] = "volatile";
    keywords[31] = "while";
    return keywords;
}

/*
*Function that checks if word is keyword and if true sets token array name index to refer to "keyword" and returns the keyword
*/
char* isandfindkey(TokenizerT *tk, char* word, char** keywords){
    int i;
    for(i=0;i<OPNUM;i++){
        if(strcmp(word,keywords[i])==0){
          tk->currenttypeindex = 50;
          return keywords[i];
        }
    }
    return NULL;
}

/*
*Function that is called when first character of token is a letter, and continues to concatenate all alphanumeric characters following it until reaches invalid word input. Contains function to check
*if word is keyword
*/
char *word(TokenizerT *tk, char first){
    char *word = (char*)malloc(strlen(tk->input)*sizeof(char));
    char **keywords = keys();
    char *possiblekey = (char*)malloc(strlen(tk->input)*sizeof(char));
    word[0]= first;
    int i;
    int j;
    for(i=tk->currentindex+1,j=1;i<strlen(tk->input) && j<strlen(tk->input);i++,j++){
        if(isalnum(tk->input[i])){
            word[j] = tk->input[i];
        }
        else{
            break;
        }
    }
    tk->currentindex = i;
    possiblekey = isandfindkey(tk,word,keywords);
    if (possiblekey!=NULL){
        tk->currenttypeindex = 50;
        //free(keywords);
        return possiblekey;
    }
    else{
        //free(keywords);
        return word;
    }
}

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * Frees input and tokenizer object memory
 */
void TKDestroy( TokenizerT * tk ) {
    free(tk->input);
    free(tk);
    return;
}

/*
*Function that is called when first character of token is single or double quotes, stores all characters up until next single or double quote character and returns entirety as string token.
*If fails to find second single or double quote, returns malformed object
*/
char* twoquotes(TokenizerT *tk, int quotetype){
    long int i;
    int count;
    int found;
    int x;
    long int j;
    char *word = malloc(strlen(tk->input)*sizeof(char));
    count=0;
    found=0;

    if(quotetype==34){
        for(i=tk->currentindex,j=0;i<strlen(tk->input) && j<strlen(tk->input);i++,j++){
            x = tk->input[i];
            if(x==34){
                count++;
                tk->currentindex+=count;
                tk->currenttypeindex=52;    
                return word;
            }
            else{
                word[j]=tk->input[i];
                count++;
            }
        }
    tk->currenttypeindex=45;
    //tk->currentindex++;
    }

    else{
        for(i=tk->currentindex,j=0;i<strlen(tk->input) && j<strlen(tk->input);i++,j++){
            x = tk->input[i];
            if(x==39){
                count++;
                tk->currentindex+=count;
                tk->currenttypeindex=52;    
                return word;
            }
            else{
                word[j]=tk->input[i];
                count++;
            }
        }   
    tk->currenttypeindex=45;
    //tk->currentindex++; 
    }
    if(quotetype== 34){
        return "\"";
    }
    else{
        return "'";
    }   
}

/*
*Function that is called when first two characters of token are valid single or multi line comment characters.
*Single line comment -> moves current index of tokenizer to next line and ignores all characters on single line
*Multi line comment -> moves current index of tokenizer until after reaches valid end, if not found returns malformed object
*/
int Linecomment(TokenizerT *tk){
    long int i;
    int count;
    int found;
    int x;
    count=0;
    found=0;
    if(tk->currenttypeindex == 74){
        for(i=tk->currentindex;i<strlen(tk->input);i++){
            if(tk->input[i]=='*' && i!=strlen(tk->input-1)){
                if(tk->input[i+1]=='/'){
                    count+=2;
                    found=1;
                    break;
                }
                else{
                    count++;
                }
            }
            else{
                count++;
            }
        }
    }
    else{
        for(i=tk->currentindex;i<strlen(tk->input);i++){
            x = tk->input[i];
            if(x == 10){
                count++;
                break;
            }
            else{
                count++;
            }
        }
    }
    if(tk->currenttypeindex == 74){
        if(found==1){
            return count;
        }
        else{
            count= -1;
            return count;
        }
    }
    else{
        return count;
    }
}

/*
*Function that is called when first character of token is defined by C as punctuation
*Contains calls to functions dealing with strings, comments, operators, and plain words
*/
char* punctuation(TokenizerT *tk, long int i, int previous){
    int start;
    int end;
    char* curr;
    char character;
    int multiresult;
    int x = tk->input[i];
    if(x==34 || x==39){
        tk->currentindex++;
        return twoquotes(tk,x);
    }
                if(i!= strlen(tk->input)-1 && ispunct(tk->input[i+1])){
                        if((i+1)!= strlen(tk->input-1) && ispunct(tk->input[i+2])){
                            curr= copythat(tk->input,tk->currentindex,tk->currentindex+3);
                            tk->currentindex=i+3;
                            tk->currenttypeindex= tripleopchecker(curr);
                            if(tk->currenttypeindex==45){
                               curr=copythat(tk->input,previous,previous+2);
                               tk->currenttypeindex=doubleopchecker(curr);
                               if(tk->currenttypeindex==45){
                               curr=copythat(tk->input,previous,previous+1);
                               character = tk->input[i];
                               tk->currenttypeindex=singleopchecker(character);
                               tk->currentindex-=2;
                               return curr;
                            }
                               tk->currentindex-=1;
                               if(tk->currenttypeindex==74 || tk->currenttypeindex==75){
                                multiresult = Linecomment(tk);
                                if(multiresult==-1){
                                    tk->currenttypeindex=45;
                                    tk->currentindex+=1;
                                    return "/*";
                                }
                                tk->currentindex+=multiresult;
                                tk->currenttypeindex=42;
                                return "comment";
                               }
                               return curr;
                            }
                            return curr;
                        }
                        else{
                            curr=copythat(tk->input,tk->currentindex,tk->currentindex+2);
                            tk->currentindex=i+2;
                            tk->currenttypeindex=doubleopchecker(curr);
                            if(tk->currenttypeindex==45){
                               curr=copythat(tk->input,previous,previous+1);
                               character = tk->input[i];
                               tk->currenttypeindex=singleopchecker(character);
                               tk->currentindex-=1;
                               return curr;
                            }
                            if(tk->currenttypeindex==74 || tk->currenttypeindex==75){
                                multiresult = Linecomment(tk);
                                if(multiresult==-1){
                                    tk->currenttypeindex=45;
                                    tk->currentindex+=1;
                                    return "/*";
                                }
                                tk->currentindex+=multiresult;
                                tk->currenttypeindex=42;
                                return "comment";
                               }
                            return curr;
                        }
                }
                else{
                    curr= copythat(tk->input,tk->currentindex,tk->currentindex+1);
                    character = tk->input[i];
                    tk->currenttypeindex=singleopchecker(character);
                    tk->currentindex=i+1;
                    return curr;
                }
}

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns NULL.
 *
 * Function that determines next token and returns token with correct type set
 */
char *TKGetNextToken( TokenizerT *tk ) {
    long int i;
    int start;
    int end;
    char* curr;
    int previous;
    int x;
    for (i=tk->currentindex;i<strlen(tk->input);i++){
        start=i;
        previous = tk->currentindex;
        x = tk->input[i];

        if(isspace(tk->input[i])){
            tk->currentindex+=1;
            continue;
        }
        else if(ispunct(tk->input[i])){
            curr = punctuation(tk,i,previous);
            if (strcmp(curr, "comment")==0){
                tk->currenttypeindex=42;
                return "comment";
        }
        else{
            return curr;
        }

        }
        else if(isalpha(tk->input[i])){
            tk->currenttypeindex = 44;
            return word(tk,tk->input[i]);
        }
        else if(x>=0 && x<=31){
            tk->currenttypeindex = 42;
            tk->currentindex+=1;
            printf("\"[0x%x]\"\n",x);
            return "escape character";
        }
        else if(isdigit(tk->input[i])!=0){
            return number(tk);
        }
    }
    return NULL;
}

/*
*Function that creates array of character arrays corresponding to types of tokens
*/
char** createTokenNames(){
    int i;
    char **ht = (char**)malloc(OPNUM*sizeof(char*));
    for(i=0;i<OPNUM;i++){
        ht[i]=(char*)malloc(40*sizeof(char));
    }
    ht[0] = "left parentheses";
    ht[1] = "right parentheses";
    ht[2] = "left bracket";
    ht[3] = "right bracket";
    ht[4] = "structure member";
    ht[5] = "indirect or multiply";
    ht[6] = "address";
    ht[7] = "minus or subtract or negative";
    ht[8] = "negate";
    ht[9] = "1's comp";
    ht[10] = "divide";
    ht[11] = "modulus";
    ht[12] = "add";
    ht[13] = "less than";
    ht[14] = "greater than";
    ht[15] = "bitwise and";
    ht[16] = "bitwise exclusive or" ;
    ht[17] = "bitwise or";
    ht[18] = "comma";
    ht[19] = "equals";
    ht[20] = "true conditional";
    ht[21] = "false conditional";
    ht[22] = "shift right";
    ht[23] = "shift left";
    ht[24] = "less or equal";
    ht[25] = "greater or equal";
    ht[26] = "boolean equals";
    ht[27] = "logical and";
    ht[28] = "logical or";
    ht[29] = "plusequals";
    ht[30] = "minusequals";
    ht[31] = "timesequals";
    ht[32] = "divideequals";
    ht[33] = "moduloequals";
    ht[34] = "rightshiftequals";
    ht[35] = "leftshiftequals";
    ht[36] = "bitwiseandequals";
    ht[37] = "bitwiseexclusiveorequals";
    ht[38] = "bitwiseorequals";
    ht[39] = "structure pointer";
    ht[40] = "notequals";
    ht[41] = "increment";
    ht[42] = "multi line comment";
    ht[43] = "single line comment";
    ht[44] = "word";
    ht[45] = "malformed object";
    ht[46] = "decimal";
    ht[47] = "octal";
    ht[48] = "hexadecimal";
    ht[49] = "floating point";
    ht[50] = "keyword";
    ht[51] = "escape character";
    ht[52] = "string";
    ht[53] ="decrement";
    return ht;
}
/*
 * main will have a string argument (in argv[1]).
 * The string argument contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */
int main(int argc, char **argv) {
    if(argc!=2){
        printf("Incorrect number of arguments.");
        return 0;
    }
    char **tokenNames = createTokenNames();
    char *token = malloc(strlen(argv[1])*sizeof(char));
    char *type= malloc(strlen(argv[1])*sizeof(char));
    TokenizerT *tk = TKCreate(argv[1]);
    token = TKGetNextToken(tk);
    while(token!=NULL){
        if(tk->currenttypeindex==42){
            token = TKGetNextToken(tk);
            continue;
        }
        type = tokenNames[tk->currenttypeindex];
        printf("%s \"%s\"\n",type,token);
        token = TKGetNextToken(tk);
    }
    free(tokenNames);
    TKDestroy(tk);
    return 0;
}