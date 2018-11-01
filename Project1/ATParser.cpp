#include <stdio.h>
#include <stdarg.h>
#include "ATHeader.h"

AT_COMMAND_DATA response;

STATE_MACHINE_RETURN_VALUE at_command_parse(uint8_t current_character)
{
	static uint32_t state			 = 0;
	static uint32_t index_string_arr = 0;

	switch (state){
		case 0:{
			if (current_character == 0x0D){ 
				state = 1;
			}
			break;
		}
		case 1:{
			if (current_character == 0x0A){
				state = 2;
			}
			else{
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
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 4:{
			if (current_character == 0x0D) {
				state = 5;
			}
			else {
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
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 7:{
			if (current_character == 'R') {
				state = 8;
			}
			else {
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 8:{
			if (current_character == 'O') {
				state = 9;
			}
			else {
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 9:{
			if (current_character == 'R') {
				state = 10;
			}
			else {
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 10:{
			if (current_character == 0x0A) {
				state = 11;
			}
			else {
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
					response.data[response.line_count][index_string_arr] = current_character;
					index_string_arr++;
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
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 13: {
			if (current_character == 0x0A) {
				state = 14;
			}
			else {
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
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		case 15:{
			if (current_character == 0x0A) {
				state = 16;
			}
			else {
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
				return STATE_MACHINE_READY_WITH_ERROR;
			}
			break;
		}
		// END OF
		// + <response line> <CR> <LF>
	}

	return STATE_MACHINE_NOT_READY;
}

void print_at_command() {
	for (uint32_t i = 0; i < response.line_count; i++) {
		printf("%s\n", response.data[i]);
	}
	printf("\nEnd of AT command response.");
}

int main(int argc, char ** argv) {
	if (argc < 2) {
		printf("Wrong nr. of params.");
		exit(1);
	}

	printf("argv[0]: %s\n", argv[0]);
	printf("argv[1]: %s\n", argv[1]);
	printf("argv[2]: %s\n", argv[2]);

	printf("Parsing file: %s\n\n", argv[1]);
	FILE *fp = fopen(argv[1], "rb");
	if (!fp) printf("Cannot open the source file. (%s)", argv[1]);

	response.line_count = 0;
	
	char ch;
	while ((ch = fgetc(fp)) != EOF) {
		if (at_command_parse(ch) == STATE_MACHINE_NOT_READY) continue;
		if (at_command_parse(ch) == STATE_MACHINE_READY_OK) {
			printf("State Machine response (1 - OK, 0 - ERROR): %d\n", response.ok);
			break;
		}
		else if (at_command_parse(ch) == STATE_MACHINE_READY_WITH_ERROR) {
			printf("State Machine ready with ERROR.");
			break;
		}
	}

	if(response.ok == 1)
		print_at_command();

	fclose(fp);

	return 0;
}