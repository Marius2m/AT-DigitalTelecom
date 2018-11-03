/*Team Members:
Ilienescu Anca
Mircea Marius
*/
#include <stdio.h>
#include "at.h"

AT_COMMAND_DATA response;
STATE_MACHINE_RETURN_VALUE at_command_parse(uint8_t current_character){
	static uint32_t state			 = 0;
	static uint32_t index_string_arr = 0;

	switch (state){
		case 0:{
			if (current_character == 0x0D){ 
				state = 1;
			}
			else {
				printf("ERROR: Missing <CR> in the beginning.\n");
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 1:{
			if (current_character == 0x0A){
				state = 2;
			}
			else{
				printf("ERROR: Missing <LF> after <CR> in the beginning.\n");
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 2:{
			if (current_character == 'O'){
				state = 3;
			}
			else if (current_character == 'E'){
				state = 6;
			}
			else if (current_character == '+') {
				state = 12;
			}
			else{
				printf("ERROR: Missing OK/ ERROR/ + after <CR> <LF>.\n");
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}

		// START OF
		// OK <CR> <LF>
		case 3:{
			if (current_character == 'K') {
				state = 4;
			}
			else {
				printf("ERROR: Missing OK.\n");
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 4:{
			if (current_character == 0x0D) {
				state = 5;
			}
			else {
				printf("ERROR: Missing <CR> after OK.\n");
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 5:{
			if (current_character == 0x0A) {
				response.ok = 1;
				return STATE_MACHINE_READY_OK;
			}
			else {
				printf("ERROR: Missing <LF> after OK <CR>.\n");
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		// END OF
		// OK <CR> <LF>

		// START OF
		// ERROR <CR> <LF>
		case 6:{
			if (current_character == 'R') {
				state = 7;
			}
			else {
				printf("ERROR: Missing ERROR.\n");
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 7:{
			if (current_character == 'R') {
				state = 8;
			}
			else {
				printf("ERROR: Missing ERROR.\n");
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 8:{
			if (current_character == 'O') {
				state = 9;
			}
			else {
				printf("ERROR: Missing ERROR.\n");
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 9:{
			if (current_character == 'R') {
				state = 10;
			}
			else {
				printf("ERROR: Missing ERROR.\n");
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 10:{
			if (current_character == 0x0A) {
				state = 11;
			}
			else {
				printf("ERROR: Missing <CR> after ERROR.\n");
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 11:{
			if (current_character == 0x0D) {
				response.ok = 0;
				return STATE_MACHINE_READY_OK;
			}
			else {
				printf("ERROR: Missing <LF> after ERROR <CR>.\n");
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		// END OF
		// ERROR <CR> <LF>

		// START OF
		// + <response line> <CR> <LF>
		case 12:{
			// >= 'space' && <+ '~'
			if (current_character >= 32 && current_character <= 126) {
				state = 12;
				if (response.line_count < AT_COMMAND_MAX_LINES) {
					if (index_string_arr < AT_COMMAND_MAX_LINE_SIZE) {
						response.data[response.line_count][index_string_arr] = current_character;
						index_string_arr++;
					}
				}
			}
			else if (current_character == 0x0D) {
				if (response.line_count < AT_COMMAND_MAX_LINES) {
					response.data[response.line_count][index_string_arr] = '\0';
					index_string_arr = 0;
					response.line_count++;
				}
				state = 13;
			}
			else {
				printf("ERROR: Invalid response line character. / Missing <CR> after <+ response line>.\n");
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 13: {
			if (current_character == 0x0A) {
				state = 14;
			}
			else {
				printf("ERROR: Missing <LF> after <+ response line> <CR>.\n");
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 14:{
			if (current_character == '+') {
				state = 12;
			}
			else if (current_character == 0x0D) {
				state = 15;
			}
			else {
				printf("ERROR: Missing +/ <CR> after <+ response line> <CR> <LF>.\n");
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 15:{
			if (current_character == 0x0A) {
				state = 16;
			}
			else {
				printf("ERROR: Missing <CR>.\n");
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 16:{
			if (current_character == 'O') {
				state = 3;
			}
			else if (current_character == 'E') {
				state = 6;
			}
			else {
				printf("ERROR: Missing OK/ ERROR after response line.\n");
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		// END OF
		// + <response line> <CR> <LF>
	}

	return STATE_MACHINE_NOT_READY;
}

void print_AT_command() {
	if (response.line_count == 0) {
		printf("\nThe AT command response is empty.");
	}
	else {
		printf("\nStart of AT command response:\n\n");
		for (uint8_t i = 0; i < response.line_count; i++) {
			printf("%s\n", response.data[i]);
		}
		printf("\nEnd of AT command response.");
	}
}
