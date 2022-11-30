/*
 * 6LoWPAN.c
 *
 *  Created on: 22.11.2022
 *      Author: Antti
 */

#include <stdio.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <functions/messaging/wireless.h>

static Char commTaskStack[1024];

void commTaskFxn(UArg arg0, UArg arg1) {

   char payload[16]; // viestipuskuri
   uint16_t senderAddr;

   // Radio alustetaan vastaanottotilaan
   int32_t result = StartReceive6LoWPAN();
   if(result != true) {
      System_abort("Wireless receive start failed");
   }

   // Vastaanotetaan viestejä loopissa
   while (true) {

        // HUOM! VIESTEJÄ EI SAA LÄHETTÄÄ TÄSSÄ SILMUKASSA
        // Viestejä lähtee niin usein, että se tukkii laitteen radion ja
        // kanavan kaikilta muilta samassa harjoituksissa olevilta!!

        // jos true, viesti odottaa
        if (GetRXFlag()) {

           // Tyhjennetään puskuri (ettei sinne jäänyt edellisen viestin jämiä)
           memset(payload,0,16);
           // Luetaan viesti puskuriin payload
           Receive6LoWPAN(&senderAddr, payload, 16);
           // Tulostetaan vastaanotettu viesti konsoli-ikkunaan
           System_printf(payload);
           System_flush();
           }
      }//HUOM NO TASK_SLEEP
}

void Wireless_init() {
    Init6LoWPAN();

    Task_Params commTaskParams;

    Task_Params_init(&commTaskParams);
    commTaskParams.stackSize = 1024;
    commTaskParams.stack = &commTaskStack;
    commTaskParams.priority = 1; // Important to set the priority to 1

    Task_Handle commTask = Task_create(commTaskFxn, &commTaskParams, NULL);
    if (commTask == NULL) {
        System_abort("Task create failed!");
    }
}

