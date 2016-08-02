/* * * * * * * * * * * * * * * * * * * * * * * *
*     Задание 10. Клиент.                      *
*     Прием/передача сообщений с RAW-сокета,   * 
*     с нашими собственными заголовками.       *
*     (Ethernet, IP, UDP)                      *
* * * * * * * * * * * * * * * * * * * * * * * */

#include "unp.h"

/* Функция подсчета контрольной суммы */
unsigned short csum(unsigned short *ptr, int nbytes)  {

    register long sum;
    unsigned short oddbyte;
    register short answer;
 
    sum = 0;

    while(nbytes > 1) {

        sum += *ptr++;
        nbytes -= 2;

    }

    if(nbytes == 1) {

        oddbyte = 0;
        *((u_char *)&oddbyte) =* (u_char *)ptr;
        sum += oddbyte;

    }
 
    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short)~sum;
     
    return(answer);

}

int main () {

	int raw_sock; // Дескриптор сокета
	struct ifreq if_idx; // Индекс интерфейса
	struct ifreq if_mac; // Мак-адрес нтерфейса
	struct sockaddr_ll s_addr; // Параметры сервера

	char if_name[IFNAMSIZ]; // Имя интерфейса
	char datagram[PACKET_LEN()], *data, *iph; // Сам пакет
	char *buffer = malloc(sizeof(char) * BUF_SIZE()); // Буффер приема сообщений
	socklen_t len = sizeof(struct sockaddr_ll); // Для Recvfrom

/* Заголовки протоколов */
	struct ether_header *eth_hdr = (struct ether_header *) datagram;
	struct iphdr *ip_hdr = (struct iphdr *) (datagram + sizeof(struct ether_header));
	struct udphdr *udp_hdr = (struct udphdr *) (datagram + sizeof(struct ether_header) + sizeof(struct iphdr));
/* mac-адреса клиента и сервера */
	unsigned char src_mac[6] = {0x80, 0xc1, 0x6e, 0x5c, 0x19, 0x8d};
	unsigned char dest_mac[6] = {0x00, 0x23, 0x8b, 0x06, 0x00, 0x6b};

/* Заполняем пакет нулями перед инициализацией. Добавляем поле с данными */
	memset(&datagram, 0, PACKET_LEN());
	data = datagram + sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct udphdr);
	// memset(&data, 0 , BUF_SIZE());
	strcpy(data, "HELLO");

	strcpy(if_name, "eno1"); // заполняем поле с именем адаптера

/* Создаем сокет */
	raw_sock = Socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

/* Сохраняем mac-адрес и ehternet-индекс устройства */
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, if_name, IFNAMSIZ - 1);
	if (ioctl(raw_sock, SIOCGIFINDEX, &if_idx) < 0) {

		perror("SIOCGIFINDEX error!");

	}

	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, if_name, IFNAMSIZ -1);
	if (ioctl(raw_sock, SIOCGIFHWADDR, &if_mac) < 0) {

		perror ("SIOCGIFHWADDR error!");

	}

/* Собираем sockaddr_ll места назначения */
	s_addr.sll_family = AF_PACKET;
	s_addr.sll_protocol = 0;
	s_addr.sll_ifindex = if_idx.ifr_ifindex;
	s_addr.sll_hatype = 0;
	s_addr.sll_pkttype = 0;
	s_addr.sll_halen = ETH_ALEN;
	memcpy(s_addr.sll_addr, if_mac.ifr_hwaddr.sa_data, ETH_ALEN);
	s_addr.sll_addr[6] = 0x0; // 
	s_addr.sll_addr[7] = 0x0; // MAC-адрес имеет длину 6 байт из 8ми, позволенных в поле структуры. Эти 2 байта не используются.

/* Собираем мethernet заголовок */
	memcpy(eth_hdr->ether_shost, src_mac, sizeof(src_mac));
	memcpy(eth_hdr->ether_dhost, dest_mac, sizeof(dest_mac));
	eth_hdr->ether_type = htons(ETHERTYPE_IP);

/* Собираем IP заголовок */
	ip_hdr->check = 0x0000; 	// Зануляем контрольную сумму перед подсчетом
	ip_hdr->tot_len = 0x0000;	// И длину тоже.
	ip_hdr->ihl = 0x5; 			// Длина поля, задающего версию
	ip_hdr->version = 0x4; 		// Версия
	ip_hdr->tos = 16; 			// 
	ip_hdr->id = htons(54321);	// Идентификатор
	ip_hdr->frag_off = 0x0000;	// Fragment offset
	ip_hdr->ttl = 64;			// Время жизни пакета 
	ip_hdr->protocol = IPPROTO_UDP;				// Протокол					
	ip_hdr->saddr = inet_addr("192.168.1.1");	// Начальный IP
	ip_hdr->daddr = inet_addr("192.168.1.2");	// Конечный IP

	ip_hdr->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(data)); //Длина IP - сегмента

	memcpy(iph, datagram + sizeof(struct ether_header), sizeof(struct iphdr));	// Копируем IP - заголовок в буффер 
																				// для вычисления контрольной суммы
	ip_hdr->check = csum((u_short *) iph, sizeof(struct iphdr)); // Вычисление контрольной суммы

/* Собираем UDP - заголовок */
	udp_hdr->uh_sport = htons(CLIENT_PORT()); // Начальный порт (порт клиента)
	udp_hdr->uh_dport = htons(SERVER_PORT()); // Порт места назначения	
	udp_hdr->uh_ulen = htons(sizeof(struct udphdr) + strlen(data)); // Длина UDP - сегмента
	udp_hdr->uh_sum = 0x0000;

/* Отправка сообщения и прием ответа */
	Sendto(raw_sock, datagram, BUF_SIZE(), 0,(struct sockaddr *)&s_addr, sizeof(struct sockaddr_ll)); 
	Recvfrom(raw_sock, buffer, BUF_SIZE(), 0, (struct sockaddr *)&s_addr, &len);
	
	buffer += 42; // Сдвигаем указатель буффера приема на 42 символа - переходим к полю "DATA" в буффере

	printf("MESSAGE:\t%s\n", buffer); 

/* Закрываем сокет и освобождаем память */
	shutdown(raw_sock, SHUT_RDWR);
	close(raw_sock);

	return 0;

}
