//Cody McMahon

/********************************************
 *This is a interpreter for the BrainFuck   *
 * programming language made by Urban Müller*
 *                                          *
 * https://en.wikipedia.org/wiki/Brainfuck  *
 ********************************************/

#include <stdlib.h>
#include <stdio.h>

//as defined by the language it has 8 tokens with added START and FINISH tokens
/*******************************
 *data structures              *
 *******************************/
typedef enum { START,LEFT,RIGHT,INC,DEC,IN,OUT,LSTART,LEND,FINISH } tokens;

struct programData{
  char* data;
  int   ptr;
  int   max;
  int   size_limit;
};

struct programInstructions{
  tokens* instructions;
  int     pc;
  int     max;
  int     size_limit;
  
};

struct programData data;
struct programInstructions inst;

void initDataStructures(){
  data.data = malloc(sizeof(char)*10);
  data.ptr = 0;
  data.max = 0;
  data.size_limit = 10;
  
  inst.instructions = malloc(sizeof(tokens)*10);
  inst.pc = -1;
  inst.max = 0;
  inst.size_limit = 10;
}

/*******************************
 *scanning of input file       *
 *******************************/ 
void addTokenToProgram(tokens token){
  if (inst.max == inst.size_limit){
    inst.instructions = realloc(inst.instructions, sizeof(tokens) * (inst.size_limit+=10));
  }
  inst.instructions[inst.max] = token;
  inst.max++;
}

void parseInputFile(const char* filename){
  FILE* f = fopen(filename, "r");
  char c = fgetc(f);
  addTokenToProgram(START);
  while(c != EOF){
    switch(c){
      case '<':addTokenToProgram(LEFT);break;
      case '>':addTokenToProgram(RIGHT);break;
      case '+':addTokenToProgram(INC);break;
      case '-':addTokenToProgram(DEC); break;
      case '.':addTokenToProgram(OUT);break;
      case ',':addTokenToProgram(IN);break;
      case '[':addTokenToProgram(LSTART);break;
      case ']':addTokenToProgram(LEND);break;
      default:break;//ignore all other characters
    }
    c = fgetc(f);
    
  }
  addTokenToProgram(FINISH);
}




/*******************************
 *execution of code            *
 *******************************/
int report(const char* errString){
  printf("\n\n------------------\nRUNTIME ERROR EXITING PROGRAM   error:%s\n",errString);
  exit(1);
}

int excecute_one_token(tokens token){
  int loopDepth = 0;
  switch(token) {
    case START:
      printf("BEGIN EXECUTION\n------------------\n\n");
      return 1;
      
    case LEFT:
      if(data.ptr){
        data.ptr--;
      }
      else{
        report("index below zero");
      }
      return 1;
      
    case RIGHT:
      data.ptr++;
      if (data.ptr >= data.size_limit){
        data.data = realloc(data.data, sizeof(char)*(data.size_limit+=10));
        if(data.data == NULL){
          report("out of memory");
        }
        data.max++;
        data.data[data.ptr] = 0;
      }
      else if (data.ptr >= data.max){
        data.max++;
        data.data[data.ptr] = 0;
      }
      return 1;
    
    case INC:
      data.data[data.ptr]++;
      return 1;
      
    case DEC:
      data.data[data.ptr]--;
      return 1;
      
    case IN:
      scanf("%c",&data.data[data.ptr]);
      return 1;
    
    case OUT:
      printf("%c",data.data[data.ptr]);
      return 1;
    
    case LSTART:
      if(data.data[data.ptr] == 0){
        while(1){
          inst.pc++;
          if(inst.instructions[inst.pc] == FINISH){
            report("uncoupled \'[\' error");
          }
          else if(inst.instructions[inst.pc] == LSTART){
            loopDepth++;
          }
          else if(inst.instructions[inst.pc] == LEND){
            if (loopDepth == 0){
              //inst.pc--;//to counteracter the automatic move fourward in the program counter
              return 1; //PC is the right number
            }
            else{
              loopDepth--;
            }
          }
        }
      }
      return 1;
    
    case LEND:
      //if the loop won't continue don't jump back
      //unchecked because I don't prevarify the code
      //if(data.data[data.ptr]){
      while(1){
        inst.pc--;
        if(inst.instructions[inst.pc] == START){
          report("uncoupled \']\' error");
        }
        else if(inst.instructions[inst.pc] == LEND){
          loopDepth++;
        }
        else if(inst.instructions[inst.pc] == LSTART){
          if (loopDepth == 0){
            inst.pc--;//to counteracter the automatic move fourward in the program counter
            return 1; //PC is the right number
          }
          else{
            loopDepth--;
          }
        }
      }
      //}
      return 1;
    case FINISH:
      printf("\n\n------------------\nEND EXECUTION\n\n");
      return 0;
  }
}

void runProgram(){
  while(excecute_one_token(inst.instructions[++inst.pc])){};
}

int main(int argc, char *argv[]){
  //printf("%s\n",argv[0]);
  initDataStructures();
  parseInputFile(argv[1]);
  runProgram();
}
