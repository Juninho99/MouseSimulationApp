#define TEMASUBLEDS "hajde"

#include "mbed.h"
#include "stm32f413h_discovery_lcd.h"
#include <stdlib.h>
#include <math.h>

#define MQTTCLIENT_QOS2 0

#include "easy-connect.h"
#include "MQTTNetwork.h"
#include "MQTTmbed.h"
#include "MQTTClient.h"
#include <string.h>

Ticker t;

int arrivedcount = 0;
double pot_value=-1;
bool taster_state=1;

int br = 0;

//Pritigla poruka
char* str;
char* prvi; 
char* drugi; 
char* treci; 


const float T (0.08);
float ax, ay, t1, t2;
int paint = 0, zaslon1 = 0, pocetna = 1, udi = 1, zaslon2 = 0, brisi_sve = 0;
int boja = 0;

float vx = 0, v0x = 0;
float vy = 0, v0y = 0;
float sx = 0, sy = 0;
int int_sx, int_sy;

int treba = 0;

int nizx[1000], nizy[1000], nizb[1000];

void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    str = (char*)message.payload;

    ax = strtof(str, &prvi); 
    ay = strtof(prvi, &drugi); 
    t1 = strtof(drugi, &treci); 
    t2 = strtof(treci, NULL); 
    
    ax *= 2.5;
    ay *= 2.5;
    ax = -ax;
    
    v0x = vx;
    vx = ax*0.25;
    sx = sx + v0x*0.25 + (ax*0.25*0.25)/2;
    
    v0y = vy;
    vy = ay *0.25;
    sy = sy + v0y*0.25 + (ay*0.25*0.25)/2;

    if(sx > 115) sx = 115;
    if(sx < -115) sx = -115;
    
    if(sy > 115) sy = 115;
    if(sy < -115) sy = -115;
    
    //Početna 
    if(t1 == 1 && pocetna == 1 && (120 + sx) >= 58 && (120 + sx) <= 180 && (120 + sy) >= 70 && (120 + sy) <= 130) {paint = 0; zaslon1 = 1; zaslon2 = 0; pocetna = 0; boja = 0;}
    
    //Zaslon1
    if(t1 == 1 && zaslon1 == 1 && (120 + sx) >= 0 && (120 + sx) <= 60 && (120 + sy) >= 220 && (120 + sy) <= 240) {paint = 0; zaslon1 = 0; zaslon2 = 0; pocetna = 1; boja = 0;}
    if(t1 == 1 && zaslon1 == 1 && (120 + sx) >= 9 && (120 + sx) <= 70 && (120 + sy) >= 9 && (120 + sy) <= 61) {paint = 1; zaslon1 = 0; zaslon2 = 0; pocetna = 0; boja = 0;}
    
    //Paint
    //isključi
    if(t1 == 1 && paint == 1 && (120 + sx) >= 0 && (120 + sx) <= 60 && (120 + sy) >= 220 && (120 + sy) <= 240) {paint = 0; zaslon1 = 0; zaslon2 = 0; pocetna = 1; boja = 0;}
    //iks
    if(t1 == 1 && paint == 1 && (120 + sx) >= 220 && (120 + sx) <= 240 && (120 + sy) >= 0 && (120 + sy) <= 20) {paint = 0; zaslon1 = 1; zaslon2 = 0; pocetna = 0; boja = 0; br = 0;}
    //crta
    if(t1 == 1 && paint == 1 && (120 + sx) >= 200 && (120 + sx) <= 220 && (120 + sy) >= 0 && (120 + sy) <= 20) {paint = 0; zaslon1 = 0; zaslon2 = 1; pocetna = 0; boja = 0;}
    
    if(t1 == 1 && paint == 1 && (120 + sx) >= 210 && (120 + sx) <= 230 && (120 + sy) >= 30 && (120 + sy) <= 50) {paint = 1; zaslon1 = 0; zaslon2 = 0; pocetna = 0; boja = 1;}
    if(t1 == 1 && paint == 1 && (120 + sx) >= 210 && (120 + sx) <= 230 && (120 + sy) >= 60 && (120 + sy) <= 80) {paint = 1; zaslon1 = 0; zaslon2 = 0; pocetna = 0; boja = 0;}
    if(t1 == 1 && paint == 1 && (120 + sx) >= 210 && (120 + sx) <= 230 && (120 + sy) >= 90 && (120 + sy) <= 110) {paint = 1; zaslon1 = 0; zaslon2 = 0; pocetna = 0; boja = 2;}
    
    //Zaslon2
    if(t1 == 1 && zaslon2 == 1 && (120 + sx) >= 0 && (120 + sx) <= 60 && (120 + sy) >= 220 && (120 + sy) <= 240) {paint = 0; zaslon1 = 0; zaslon2 = 0; pocetna = 1; boja = 0;}
    if(t1 == 1 && zaslon2 == 1 && (120 + sx) >= 9 && (120 + sx) <= 70 && (120 + sy) >= 9 && (120 + sy) <= 61) {paint = 1; zaslon1 = 0; zaslon2 = 0; pocetna = 0; boja = 0;}
    if(t1 == 1 && zaslon2 == 1 && (120 + sx) >= 61 && (120 + sx) <= 120 && (120 + sy) >= 220 && (120 + sy) <= 240) {paint = 1; zaslon1 = 0; zaslon2 = 0; pocetna = 0; boja = 0;}

    
    BSP_LCD_FillCircle (120+sx, 120+sy, 3);
    
    if(paint == 1 && t1 == 1)
    {
        if(br > 3)
        {
            nizx[br] = sx-1;
            nizy[br] = sy-1;
            if(boja == 0) nizb[br] = 0;
            else if(boja == 2) nizb[br] = 2;
            else nizb[br] = 1;
        }
        br++;
    }
    ++arrivedcount;
    str=(char*)message.payload;
}

void Osvjezi()
{
    if(pocetna == 1)
    {
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
        
        BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
        BSP_LCD_FillRect(58, 70, 120, 60);
        
        BSP_LCD_SetFont(&Font20);
        BSP_LCD_SetBackColor(LCD_COLOR_GREEN);
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DisplayStringAt(0, 92, (uint8_t *)"Pokreni", CENTER_MODE);
    }
    if(zaslon1 == 1)
    {
        BSP_LCD_Clear(LCD_COLOR_WHITE);
        
        BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
        BSP_LCD_DrawRect(9, 9, 61, 52);
   
        BSP_LCD_SetFont(&Font16);
        BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
        BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
        BSP_LCD_DisplayStringAt(-75, 27, (uint8_t *)"Paint", CENTER_MODE);
        
        
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DrawRect(87, 9, 61, 52);
        
        
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DrawRect(166, 9, 61, 52);
        
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
        BSP_LCD_FillRect(0, 220, 60, 240);
        
        BSP_LCD_SetFont(&Font12);
        BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
        BSP_LCD_SetBackColor(LCD_COLOR_RED);
        BSP_LCD_DisplayStringAt(-88,  225, (uint8_t *)"Iskljuci", CENTER_MODE);
    }
    
    if(zaslon2 == 1)
    {
        BSP_LCD_Clear(LCD_COLOR_WHITE);
        
        BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
        BSP_LCD_DrawRect(9, 9, 61, 52);
   
        BSP_LCD_SetFont(&Font16);
        BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
        BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
        BSP_LCD_DisplayStringAt(-75, 27, (uint8_t *)"Paint", CENTER_MODE);
        
        
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DrawRect(87, 9, 61, 52);
        
        
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DrawRect(166, 9, 61, 52);
        
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
        BSP_LCD_FillRect(0, 220, 60, 240);
        
        BSP_LCD_SetFont(&Font12);
        BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
        BSP_LCD_SetBackColor(LCD_COLOR_RED);
        BSP_LCD_DisplayStringAt(-88,  225, (uint8_t *)"Iskljuci", CENTER_MODE);
        
        BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
        BSP_LCD_FillRect(60, 220, 60, 240);
        
        BSP_LCD_SetFont(&Font12);
        BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
        BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
        BSP_LCD_DisplayStringAt(-30,  225, (uint8_t *)"Paint", CENTER_MODE);
    }
    
    if(paint == 1 && t1 == 0)
    {
        BSP_LCD_Clear(LCD_COLOR_WHITE);
        
        for(int i = 3; i < br; i++) 
        {
            if(nizb[i] == 0) BSP_LCD_SetTextColor(LCD_COLOR_RED);
            else if(nizb[i] == 2) BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
            else BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
            
            BSP_LCD_FillCircle (120+nizx[i], 120+nizy[i], 3);
        }
        
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DrawRect(200, 0, 20, 20);
        
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DrawRect(220, 0, 19, 20);
        
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
        BSP_LCD_DrawLine (224, 16, 236, 4);
        BSP_LCD_DrawLine (236, 16, 224, 4);
        
        BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
        BSP_LCD_DrawLine (205, 10, 215, 10);
       
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
        BSP_LCD_FillRect(0, 220, 60, 240);
        
        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        BSP_LCD_FillCircle (120, 120, 3);
        
        BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
        BSP_LCD_FillRect(210, 30, 20, 20);
        
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
        BSP_LCD_FillRect(210, 60, 20, 20);
        
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DrawRect(210, 90, 20, 20);
        
        BSP_LCD_SetFont(&Font12);
        BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
        BSP_LCD_SetBackColor(LCD_COLOR_RED);
        BSP_LCD_DisplayStringAt(-88,  225, (uint8_t *)"Iskljuci", CENTER_MODE);
        
        BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
        BSP_LCD_FillRect(60, 220, 60, 240);
        
        BSP_LCD_SetFont(&Font12);
        BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
        BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
        BSP_LCD_DisplayStringAt(-30,  225, (uint8_t *)"Paint", CENTER_MODE);
    }
    
    if(boja == 0) BSP_LCD_SetTextColor(LCD_COLOR_RED);
    else if(boja == 2) BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    else BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
    
    if(boja == 2)BSP_LCD_DrawCircle (120+sx, 120+sy, 3);
    else BSP_LCD_FillCircle (120+sx, 120+sy, 3);
}

int main(int argc, char* argv[])
{
    printf("Simulacija miša\r\n");

    NetworkInterface *network;
    network = NetworkInterface::get_default_instance();
    
    if (!network) {
        return -1;
    }

    MQTTNetwork mqttNetwork(network);

    MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);

    const char* hostname = "broker.hivemq.com";
    int port = 1883;
    printf("Connecting to %s:%d\r\n", hostname, port);
    int rc = mqttNetwork.connect(hostname, port);
    if (rc != 0)
        printf("rc from TCP connect is %d\r\n", rc);

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = "ugradbeni";
    data.username.cstring = "";
    data.password.cstring = "";
    
    
    if ((rc = client.connect(data)) != 0)
        printf("rc from MQTT connect is %d\r\n", rc);

    if ((rc = client.subscribe(TEMASUBLEDS, MQTT::QOS2, messageArrived)) != 0)
        printf("rc from MQTT subscribe is %d\r\n", rc);

    MQTT::Message message;

    while(1) {
        rc = client.subscribe(TEMASUBLEDS, MQTT::QOS0, messageArrived);
        Osvjezi();
        wait(T);
    }

}