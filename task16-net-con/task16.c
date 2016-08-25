/*********************************
 *  Модуль ядра - Netfilter      *
 *  Перенаправляет пакеты        *
 *  на правильный порт.          *
 *********************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/inet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/icmp.h>
#include <linux/slab.h>

/* Псево-заголовок для подсчета контрольной суммы UDP */
struct pseudo_header {
  u_int32_t source_address;
  u_int32_t dest_address;
  u_int8_t placeholder;
  u_int8_t protocol;
  u_int16_t udp_length;
};

/* Функция подсчета контрольной суммы */
u_short csum(u_short *hdr_pt, int nwords) {
  long sum = 0;
  u_short buf = 0;
  
  while (nwords > 1) {
      sum += *hdr_pt++;
      nwords -= sizeof (u_short);
  }

  if (nwords) {
      buf = *(u_char *) hdr_pt << 8;
      sum += ntohs(buf);
  }

  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);

  return (u_short)(~sum);
}

/* Объявление структур. Мы объявим 2-е структуры. */
/* 1-я для входящих пакетов */
/* 2-я для исходящих пакетов */
static struct nf_hook_ops nf_incoming;
static struct nf_hook_ops nf_outgoing;

static struct nf_hook_ops nf_incoming;
static struct sk_buff *skbf;

/* Функция обработки */
unsigned int main_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
  /* Объявляем структуры заголовков для проверки */
  struct iphdr *ip_header;
  struct udphdr *udp_header;
  struct pseudo_header psh; /* для подсчета контрольной суммы */
  int psize = 0; /* размер псевдо-заголовка */
  char *pseudogram;
  skbf = skb; /* работаем с указателем на skb */

  /* Трогаем только IP-пакеты */
  if (skbf->protocol != htons(ETH_P_IP))
    return NF_ACCEPT;
  /* Получаем заголовок сетевого уровня (IP) */
  ip_header = (struct iphdr *)skb_network_header(skbf);
  /* Если это UDP */ 
  if (ip_header -> protocol == IPPROTO_UDP) {
    /* Получаем заоловок транспортного уровня */
    udp_header = (struct udphdr *)(skb_transport_header(skbf));  
    pr_info("UDP packet captured\nDPORT:\t%d\n", ntohs(udp_header -> dest));
    /* Если пакеты идут на неверный порт - исправляем их 
       и пересчитываем контрольную сумму */
    if (udp_header -> dest == htons(3332)) {
      struct udphdr *myudp = kmalloc(sizeof(struct udphdr), GFP_KERNEL);
      if (myudp == NULL) {
        pr_alert("kmalloc error!\n");
        return NF_DROP;
      }
      /* Исправленый заголовок */
      myudp -> source = udp_header -> source;
      myudp -> dest = htons(3333);
      myudp -> len = udp_header -> len;
      myudp -> check = 0x0000;

      /* Заполняем псевдо-заголовок, необходимый для подсчета контролной суммы UDP-заголовка */
      psh.source_address = ip_header -> saddr;
      psh.dest_address = ip_header -> daddr;
      psh.placeholder = 0;
      psh.protocol = IPPROTO_UDP;
      psh.udp_length = udp_header -> len;
      psize = sizeof(struct pseudo_header) + ntohs(udp_header -> len);
      pseudogram = kmalloc(psize, GFP_KERNEL);

      memcpy(pseudogram, (char *)&psh, sizeof(struct pseudo_header));
      memcpy(pseudogram + sizeof(struct pseudo_header), udp_header, ntohs(udp_header->len));

      myudp -> check = csum((unsigned short *)pseudogram, psize);

      /* Помещаем новый заголовок по указателю на старый */
      memcpy(udp_header, myudp, sizeof(struct udphdr));
      kfree(myudp);
      kfree(pseudogram);
    }
  }
  return NF_ACCEPT;
}

/* При инициализации модуля */
static int __init startblock(void) {
  /* Заполнение структур */
  /* Сначала, заполним структуру для входящих пакетов */
  nf_incoming.hook = main_hook; /* указатель на нашу функцию */
  nf_incoming.pf = PF_INET;
  nf_incoming.hooknum = NF_INET_LOCAL_IN;
  nf_incoming.priority = NF_IP_PRI_FIRST;
  /* Теперь для исходящих */
  nf_outgoing.hook = main_hook;
  nf_outgoing.pf = PF_INET;
  nf_outgoing.hooknum = NF_INET_LOCAL_OUT;
  nf_outgoing.priority = NF_IP_PRI_FIRST;
  /* Вроде все, осталось только зарегистрировать наши функции */
  nf_register_hook(&nf_incoming);
  nf_register_hook(&nf_outgoing);
  pr_info("FireWall loaded \n");
  return 0;
}
/* При выгрузке модуля */
static void __exit stopblock(void) {
  /* Не забываем удалить наши вызовы, а то конфуз может случится */
  nf_unregister_hook(&nf_incoming);
  nf_unregister_hook(&nf_outgoing);
  pr_info("FireWall unload \n");
}

module_init(startblock);
module_exit(stopblock);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me!!!");