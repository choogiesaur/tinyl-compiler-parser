/*
 *********************************************
 Firas Sattar
 Spring 2014
 CS314 Principles of Programming Languages
 *********************************************
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"

void markReadWrite(Instruction * instr);
void markStore(Instruction * instr);
void printCrit(Instruction * head);
void deleteGarbage(Instruction * instr);
void findContributors(Instruction * instr);
void findContributorsArith(Instruction * instr);

int main()
{
	Instruction *head;
	
	head = ReadInstructionList(stdin);
	if (!head) {
		WARNING("No instructions\n");
		exit(EXIT_FAILURE);
	}
	printf("got here\n");

	//this marks all read and writes as critical
	Instruction *curr = head;
	while(curr){
		markReadWrite(curr);
		curr = curr->next;
	}
	/*now finds rest of crit instruc*/
	curr = LastInstruction(head);
	while(curr){
		if(curr->critical == 'c'){
			switch(curr->opcode){
				case ADD:
				case SUB:
				case MUL:
					findContributorsArith(curr);
				case STORE:
				case LOAD:
				case LOADI:
				case READ:
				case WRITE:
					findContributorsArith(curr);
			}
		}
		curr = curr->prev;
	}
	printCrit(head);
	/*curr = head;
	while(curr){
			if(curr->critical != 'c'){
				if(curr == head){
					head = curr->next;
					head->prev = NULL;
					free(curr);
					continue;
				}else{
					deleteGarbage(curr);
				}
			}
			curr = curr->next;
	}*/
	if (head) {
		PrintInstructionList(stdout, head);
		DestroyInstructionList(head);
	}
	printf("Finished this method!");
	return EXIT_SUCCESS;
}

//if the input instruction is read or write mark as crit
void markReadWrite(Instruction * instr) {
	if(instr->opcode == READ){
		instr->critical = 'c';
	}else if(instr->opcode == WRITE){
		instr->critical = 'c';
		markStore(instr);
	}
	printf("Finished markReadWrite!\n");
}

/*takes in a WRITE instruction. finds and marks the STORE instruction that stored ze value into the register being written to output */
void markStore(Instruction * instr) {
	if(instr->opcode != WRITE){
		printf("findStore: Error. Tried to find STORE instruction for a non-WRITE instruction\n");
	}
	int target = instr->field1;
	Instruction *curr = instr->prev;
	while(curr){
		if(curr->opcode == STORE && curr->field1 == target){
			curr->critical = 'c';
			return;
		}
		curr = curr->prev; 
	}
	printf("Finished markStore!\n");
}

void printCrit(Instruction * head) {
	Instruction *curr = head; int ptr = 1;
	while(curr != NULL){
		printf("Instruction: %d Crit: %c\n", ptr, curr->critical);
		curr = curr->next;
		ptr++;
	}
	printf("Finished printCrit!\n");
}

void deleteGarbage(Instruction * instr) {
	Instruction *prev, *next;
    prev = instr->prev;
    next = instr->next;
    if (prev) {
        if (next) {
            prev->next = next; /*input instruc has prev and next*/
            next->prev = prev;
            free(instr);
           // return head;
        } else {
            prev->next = NULL; /*instruc has prev but no next (end of list)*/
            free(instr);
           // return head;
        }
    } else {
        if (next) {
            next->prev = NULL; /*instruc has no prev, but has a next (this is the head)*/
			//head = next;
			free(instr);
			//return head;
        } else {
            free(instr); /*instruction has no prev and no next (sole element)*/
			//head = NULL;
			//return head;
        }
        
    }
    printf("Finished deleteGarbage!\n");
	//return head;
}
/*given a LOAD/STORE instruction, find other instructions that contrib to its value.*/
void findContributors(Instruction * instr){
		int target;
		target = instr->field2;
		Instruction *candid = instr->prev;
		if(candid && candid->critical != 'c'){
			switch(candid->opcode){ /* diff cases depending on type of candidate*/
				case ADD:
				case SUB:
				case MUL:
				case STORE:
				case LOAD:
				case LOADI:
					if(candid->field1 == target){
						candid->critical = 'c';
					}
					break;
				case READ:
					/*mark crit*/
				case WRITE:
					markStore(instr);
					/*mark crit, find store instruc*/
			}
		}
	printf("Finished findContrib!\n");
}

/*given an ARITHMETIC instruction, find other instructions that contrib to its value*/
void findContributorsArith(Instruction * instr){
		int target1, target2;
		target1 = instr->field2;
		target2 = instr->field3;
		Instruction *candid = instr->prev;
		if(candid && candid->critical != 'c'){
			switch(candid->opcode){
				case ADD:
				case SUB:
				case MUL:
				case STORE:
				case LOAD:
				case LOADI:
					if(candid->field1 == target1 || candid->field1 == target2){
						candid->critical = 'c';
					}
				case READ:
				case WRITE:
					break;
			}
		}
	printf("Finished findContribArith!\n");
}
