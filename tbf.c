//Cody McMahon

/********************************************
 *This is a interpreter for the BrainFuck   *
 * programming language made by Urban Müller*
 *                                          *
 * https://en.wikipedia.org/wiki/Brainfuck  *
 ********************************************/

#include <stdlib.h>
#include <stdio.h>
//this is used to close the program if there is an error
//might try error resolutions at a later time
int report(const char* errString);

/*******************************
 *data structures              *
 *******************************/
//as defined by the language it has 8 tokens with added START and FINISH tokens
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
  data.max = 1;
  data.size_limit = 10;
  data.data[data.ptr]=0;
  
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


FILE* verifyFile(const char* filename){
  //can't be an empty string
  int i = 0;
  char* file_type = 0;
  while(filename[i] != '\0'){
    i++;
    //this may happen many times like "this.is.a.file.bf"
    if(filename[i] == '.'){
      file_type = &filename[i + 1];
    }
  }
  // like filename
  if (file_type == 0){
    report("file with no extension");
  }
  // like filename.
  else if (file_type[0] == '\0'){
    report("file ends with a \'.\'");
  }
  // only proper filenames are .b and .bf
  // go in this else if they have a file extension of some sort
  // file_type[1] has to exist or your os gave a broken string
  else{
    if(file_type[0] != 'b'){
      report("only files with \'.bf\' or \'.b\' accepted1");
    }
    /*
      this may look extreme but to put it simple
      if file_type[1] is the null terminator its good otherwise check file_type[2]
      if file_type[2] is not f is it wrong 
      if file_type[2] is f though then the next character must be the null terminator
    */
    if(file_type[1] && (file_type[1] != 'f' || file_type[2])){     //this is nice in some code ninja way but really 
      report("only files with \'.bf\' or \'.b\' accepted2");       //it should be clearer
    }
  }
  //if it gets here the filename is proper
  FILE* f = fopen(filename, "r");
  if(f == NULL){
    report("file not found");
  }
  //good file
  else{
    return f;
  }
}


void parseInputFile(const char* filename){
  FILE* f = verifyFile(filename);
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
      /*
        [ ] [x] [ ]
      */
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
  //end the program if there is no arguments
  if(argc == 0){
    report("no input file");
  }
  initDataStructures();
  parseInputFile(argv[1]);
  runProgram();
}
