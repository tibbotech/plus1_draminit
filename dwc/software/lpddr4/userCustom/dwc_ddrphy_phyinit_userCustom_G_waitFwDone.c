/** \file
 *  \brief function to implement step G of PUB databook initialization step.
 *  \addtogroup CustFunc
 **  @{
 */
#include "dwc_ddrphy_phyinit.h"

/** \brief Implements the mechanism to wait for completion of training firmware
 * execution.
 *
 * The purpose of user this function is to wait for firmware to finish training.
 * The user can either implement a counter to wait or implement the polling
 * mechanism described in the Training Firmware App Note section "Running the
 * Firmware".  The wait time is highly dependent on the training features
 * enabled via SequenceCtrl input to the message block.  See Training Firmware
 * App note for details. 
 *
 * The default behavior of this function is to print comments relating to this
 * process.  A function call of the same name will be printed in the output text
 * file.
 *
 * The user can choose to leave this function as is, or implement mechanism to
 * trigger mailbox poling event in simulation.

 * \return void
 */

typedef struct debug_t
{
	unsigned int  index;
	char *message;
}debug_s;

//debug_s streaming[379] = 
//{
//	#include <lpddr4_pmu_train.strings>
//};

/*
*1.Poll the UctWriteProShadow, looking for a 0
*2.When a 1 is seen, read the UctWriteOnlyShadow register to get the major message number.If
*   reading a streaming message, also read the UctDatWriteOnlyShadow register.
*3.Write the DctWriteProt to 0 to acknowledge the receipt of the message.
*4.Poll the UctWriteProtShadow, looking for a 1
*5.When a 1 is seen, write the DctWriteProt to 1 to complete the protocal
*6.Go to step 1.
*/
unsigned int get_mail(unsigned short mode)
{
	unsigned int mail;
	while((SP_REG_R(0xd0004)&0x0001) != 0)
	{
	}
	mail = SP_REG_R(0xd0032);//mode=16: major message, mode=32: lower 16 bits for streaming message
	if(mode == 32)
	{
		mail |= (SP_REG_R(0xd0034)<<16); //mode=32: upper 16 bits for streaming message
	}
	SP_REG_W(0xd0031,0);//The user acknowledges the receipt of the message.
	while((SP_REG_R(0xd0004)&0x0001) == 0)
	{
		
	}
	SP_REG_W(0xd0031,1);
	return mail;
}

void decode_streaming_message()
{
	unsigned char i, args[10];
	unsigned int string_index;
	string_index = get_mail(32);
	//lpddr4_pmu_train.strings
	//debug_string = lookup_string_index(string_index);
	
	//if(streaming[1].index == 0x00330002)
	//	prn_string(streaming[1].message);
	prn_dword(string_index);

	i = 0;
	while(i <= (string_index&0xffff))
	{
		prn_dword(get_mail(32));
		i++;
	}
	//dwc_ddrphy_phyinit_cmnt ("%s \n", debug_string);

}


void dwc_ddrphy_phyinit_userCustom_G_waitFwDone () {

    char *printf_header;
	unsigned char major_message;
	
    printf_header = "// [dwc_ddrphy_phyinit_userCustom_G_waitFwDone]";

    dwc_ddrphy_phyinit_cmnt ("%s Wait for the training firmware to complete.  Implement timeout fucntion or follow the procedure in \"3.4 Running the firmware\" of the Training Firmware Application Note to poll the Mailbox message.\n", printf_header);
    dwc_ddrphy_phyinit_print ("dwc_ddrphy_phyinit_userCustom_G_waitFwDone ();\n\n");
	dwc_ddrphy_phyinit_print ("get_mail(16)\n\n");
#if 0
	while (1) {
		major_message = get_mail(16);
		if(major_message == 0x08)
		{
			decode_streaming_message();
		}
		if(major_message == 0x07)
			break;	
		break;//tonyh test
	}
#endif 
    dwc_ddrphy_phyinit_cmnt ("%s End of dwc_ddrphy_phyinit_userCustom_G_waitFwDone()\n", printf_header);

}
/** @} */
