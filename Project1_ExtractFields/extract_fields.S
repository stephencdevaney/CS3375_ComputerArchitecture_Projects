.section .text
.global extract_fields
.type extract_fields @function

extract_fields:
            addi x28, x0, 1  // set x28 = to 1 for branch instruction
            addi x29, x0, 2  // set x29 = to 2 for branch instruction
            
    OPCODE: bne x11, x0, RD  // if a1 != 0 jump to rd (opcode label is used for readability only)
            addi x5, x0, 0x7f  // create mask for opcode
            and x10, x10, x5  // remove extra bits from return register
            
    RD:     bne x11, x28, RS1  // if a1 != 1 jump to rs1
            addi x5, x0, 0x1F  // create mask for rd
            slli x5, x5, 7  // shift bits for the masks to match the rd field
            and x10, x10, x5  // remove bits from other fields from the return register
            srli x10, x10, 7  // shift rd field for output
                        
    RS1:    bne x11, x29, EXIT  // if a1 != 2 jump to exit
            addi x5, x0, 0x1f  // create mask for rs1
            slli x5, x5, 15  // shift bits for the masks to match the rs1 field
            and x10, x10, x5  // remove bits from other fields from the return register
            srli x10, x10, 15  // shift rs1 field for output
            
    EXIT:   ret  // return value in x10 register
	
	//Submission number: e04ef10d-262b-4441-bbd8-8e791805b87a
