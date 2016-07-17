/* * * * * * * * * * * * * * * * * * * * * * *
*     Задание 4.                             *
*     Перехватывает пакеты с первого в списке* 
*     устройства и выводит их содержимое     *
*     на экран.         *                    *
* * * * * * * * * * * * * * * * * * * * * * */

#include "unp.h"

void callback(u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char* packet) { 

    static int count = 0; 

    printf("Packet Count:\t%d\n", ++count);             /* Количество пакетов */
    printf("Recieved Packet Size:\t%d\n", pkthdr->len); /* Длина заголовка */
    printf("Payload:\n");                               /* А теперь данные */

    for (i = 0; i < pkthdr->len; i++) { 

        if (isprint(packet[i]))                         /* Проверка, является ли символ печатаемым */

            printf("%c ",packet[i]);                    /* Печать символа */

        else 

            printf(" _ ");                      /* Если символ непечатаемый, вывод _ */

        if (((i%16 == 0) && (i != 0)) || i == pkthdr->len - 1)

            printf("\n");

    }

}


int main(int argc, char **argv) { 

    char *dev = NULL; 
    char errbuf[PCAP_ERRBUF_SIZE]; 
    pcap_t *descr;
    memset(&descr, 0, sizeof(descr)); 
    const u_char *packet = NULL;

    struct ether_header *eptr; /* net/ethernet.h */
    memset(&eptr, 0, sizeof(eptr));

    struct bpf_program fp;     /*выражение фильтрации в составленном виде */ 
    memset(&fp, 0, sizeof(fp));

    bpf_u_int32 maskp = 0;         /*маска подсети */ 
    bpf_u_int32 netp = 0;          /* ip */ 

    if (argc != 2) {

        fprintf(stdout, "Usage: %s \"expression\"\n", argv[0]); //Если кол-во аргументов <> 2, показываем пользователю, как работать с программой
        exit(0);

    } 

    /* Получение имени сетевого адаптера */
    dev = Pcap_lookupdev(errbuf); //Все функции-обертки для этого задания см. в файле unp.h (line 245+)

    /* Получение ip и маски сети для выбранного адаптера */ 
    Pcap_lookupnet(dev, &netp, &maskp, errbuf); 

    /* Открытие устройства в promiscuous-режиме (требуются права администратора) */ 
    descr = Pcap_open_live(dev, BUFSIZ, 1,-1, errbuf);

    /* Собираем фильтр */ 
    Pcap_compile(descr, &fp, argv[1], 0, netp);

    /* Применяем фильтр*/ 
    Pcap_setfilter(descr, &fp);

    /* Обратный вызов */ 
    Pcap_loop(descr, -1, callback, NULL); 
    return 0; 

}
