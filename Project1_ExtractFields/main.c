/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>

extern int extract_fields(int RInst, int field); // for field use 0 for opcode, 1 for rd, and 2 for rs1

int main() {
	int opcode_1 = extract_fields(0x015A04B3, 0);  // get value of opcode for 0x015A04B3
	int rd_1 = extract_fields(0x015A04B3, 1);  // get value of rd for 0x015A04B3
	int rs1_1 = extract_fields(0x015A04B3, 2);  // get value of rs1 for 0x015A04B3

	int opcode_2 = extract_fields(0x00578833, 0);  // get value of opcode for 0x00578833
	int rd_2 = extract_fields(0x00578833, 1);  // get value of rd for 0x00578833
	int rs1_2 = extract_fields(0x00578833, 2);  // get value of rs1 for 0x00578833

    printf("opcode for 0x015A04B3 is: 0x%x\n", opcode_1); // print opcode for 0x015A04B3 should be 0110011 or 0x33
    printf("rd for 0x015A04B3 is: 0x%x\n", rd_1); // print rd for 0x015A04B3 should be 01001 or 0x9
    printf("rs1 for 0x015A04B3 is: 0x%x\n", rs1_1); // print rs1 for 0x015A04B3 should be 10100 or 0x14

    printf("\n\n");  // print line breaks to separate 0x015A04B3 printed extracted fields from 0x00578833

    printf("opcode for 0x00578833 is: 0x%x\n", opcode_2); // print opcode for 0x00578833 should be 0110011 or 0x33
    printf("rd for 0x00578833 is: 0x%x\n", rd_2); // print rd for 0x00578833 should be 10000 or 0x10
    printf("rs1 for 0x00578833 is: 0x%x\n", rs1_2); // print rs1 for 0x00578833 should be 01111 or 0xf
}
