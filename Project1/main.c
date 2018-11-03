/*Team Members:
Ilienescu Anca
Mircea Marius
*/
#include <stdio.h>
#include "at.h"

int main(int argc, char ** argv) {
	if (argc < 2) {
		printf("Wrong nr. of params.\n");
		printf("Format: <a.out> <input_file.txt>");
		return -1;
	}

	//printf("argv[0]: %s\n", argv[0]);
	//printf("argv[1]: %s\n", argv[1]);
	//printf("argv[2]: %s\n", argv[2]);

	printf("Parsing file: %s\n------------------------------------------\n\n", argv[1]);
	FILE *fp = fopen(argv[1], "rb");
	if (!fp) {
		printf("Error when opening %s", argv[1]);
		return -1;
	}

	response.line_count = 0; // starting of first + ... line
	char ch;
	while ((ch = fgetc(fp)) != EOF) {
		//printf("%c %d", ch, ch);
		uint8_t parser_result = at_command_parse(ch);
		if (parser_result == STATE_MACHINE_NOT_READY) continue;
		if (parser_result == STATE_MACHINE_READY_OK) {
			printf("State Machine response: ");
			if (response.ok == 1) {
				printf("OK");
				print_AT_command();
				//printf("-%d-", response.line_count);
			}
			else {
				printf("ERROR\n");
			}
			break;
		}
		else if (parser_result == STATE_MACHINE_READY_WITH_ERROR) {
			printf("State Machine ready with ERROR.");
			break;
		}
	}

	if (fclose(fp)) {
		printf("\n\n------------------------------------------\nError when closing file: %s", argv[1]);
		return -1;
	}
	else {
		printf("\n\n------------------------------------------\nSuccessfully closed file: %s", argv[1]);
		return 0;
	}

	return 0;
}