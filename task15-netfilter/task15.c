/*********************************
 *  Модуль ядра - Netfilter      *
 *  Блокирует траффик для        *
 *  указанного приложения.       *
 *                               *
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

/* Объявление структур. Мы объявим 2-е структуры. */
/* 1-я для входящих пакетов */
/* 2-я для исходящих пакетов */
static struct nf_hook_ops nf_incoming;
static struct nf_hook_ops nf_outgoing;

static struct nf_hook_ops nf_incoming;
static struct sk_buff *skbf;

/* Функция обработки */
unsigned int main_hook (void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
  /* Объявляем структуры заголовков для проверки */
  struct iphdr *ip_header;
  struct udphdr *udp_header;
  skbf = skb; /* работаем с указателем на skb */

  /* Трогаем только IP-пакеты */
  if (skbf->protocol != htons(ETH_P_IP))
    return NF_ACCEPT;
  /* Получаем заголовок сетевого уровня (IP) */
  ip_header = (struct iphdr *)skb_network_header(skbf);
  /* Если это UDP */ 
  if (ip_header -> protocol == IPPROTO_UDP) {
    /* Получаем заоловок транспортного уровня */
    udp_header = (struct udphdr *)(skb_transport_header(skb));  
    pr_info("UDP packet captured\nDPORT:\t%d\n", ntohs(udp_header -> dest));
    /* Если пакеты идет от нашего приложения или к нему - отбрасываем */
    if ((udp_header -> dest == htons(3333)) || (udp_header -> dest == htons(3334)))
      return NF_DROP;
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