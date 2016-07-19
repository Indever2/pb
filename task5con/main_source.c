/* * * * * * * * * * * * * * * * * * * * * * *
*     Задание 5.                             *
*     Перехватывает пакеты с первого в списке* 
*     устройства и выводит их содержимое     *
*     на экран. Разбирает заголовки.         *                    
* * * * * * * * * * * * * * * * * * * * * * */

#include "unp.h"

void callback(u_char *arg, const struct pcap_pkthdr *pkthdr, const u_char *packet) { 

    int eth_addr_pos = ETHER_ADDR_LEN; // для вывода адреса отправителя | назначения
    static int count = 0; 
    u_char *eth_addr_ptr; //буфер для хранения адреса
    struct ether_header *eth_info; //заголовок ethernet

    eth_info = (struct ether_header *) packet;

    switch (htons(eth_info->ether_type)) {

        case ETHERTYPE_IP:

            printf("---> Ethernet type hex:%x dec:%d is an IP packet\n", htons(eth_info->ether_type), htons(eth_info->ether_type));
            break;

        case ETHERTYPE_ARP:

            printf("---> Ethernet type hex:%x dec:%d is an ARP packet\n", htons(eth_info->ether_type), htons(eth_info->ether_type));
            break;

        default:

            printf("---> Ethernet type hex:%x dec:%d is an unknown-protocol packet\n", htons(eth_info->ether_type), htons(eth_info->ether_type));
            break;

    }

    eth_addr_ptr = eth_info->ether_dhost;

    printf("\nDestination Address:\t");
    do {

        printf("%s%x", (eth_addr_pos == ETHER_ADDR_LEN) ? " " : ":", *eth_addr_ptr++);

    } while (--eth_addr_pos > 0);

    eth_addr_pos = ETHER_ADDR_LEN;
    eth_addr_ptr = eth_info->ether_shost;

    printf("\nSource Address:   \t");
    do {

        printf("%s%x", (eth_addr_pos == ETHER_ADDR_LEN) ? " " : ":", *eth_addr_ptr++);

    } while (--eth_addr_pos > 0);

    printf("\n");


    printf("Packet Count:\t%d\n", ++count);             /* Количество пакетов */
    printf("Recieved Packet Size:\t%d\n", pkthdr->len); /* Длина заголовка */
    printf("Payload:\n");                               /* А теперь данные */

    for (int i = 0; i < pkthdr->len; i++) { 

        if (isprint(packet[i]))                         /* Проверка, является ли символ печатаемым */

            printf(" %c ",packet[i]);                    /* Печать символа */

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

    printf("DEVICE: %s \n", dev);

    /* Собираем фильтр */ 
    Pcap_compile(descr, &fp, argv[1], 0, netp);

    /* Применяем фильтр*/ 
    Pcap_setfilter(descr, &fp);

    /* Обратный вызов */ 
    Pcap_loop(descr, -1, callback, NULL);

    pcap_close(descr);

    return 0; 

}
