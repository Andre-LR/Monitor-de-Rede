import numpy as np
from scapy.layers.l2 import ARP
from scapy.layers.inet import IP, TCP, UDP, ICMP
from scapy.layers.inet6 import IPv6, ICMPv6EchoRequest, ICMPv6EchoReply
from scapy.all import sniff


# Endereços MAC dos dispositivos da rede
devices = {"00:11:22:33:44:55": "Dispositivo 1", "AA:BB:CC:DD:EE:FF": "Dispositivo 2"}

# Estatísticas de tráfego
total_packets = 0
total_bytes = 0

# Estatísticas de tráfego por dispositivo
packets_per_device = {mac: 0 for mac in devices}

# Geral - Estatísticas de tamanho dos pacotes
min_packet_size = np.inf
max_packet_size = 0
total_packet_size = 0
packet_count = 0

# Nível de Enlace - Estatísticas de tráfego ARP
arp_requests = 0
arp_replies = 0

# Nível de Rede - Estatísticas de tráfego
ipv4_packets = 0
ipv4_percent = 0

icmp_packets = 0
icmp_percent = 0

icmp_echo_requests = 0
icmp_echo_requests_percent = 0

icmp_echo_replies = 0
icmp_echo_replies_percent = 0

ipv6_packets = 0
ipv6_percent = 0

icmpv6_packets = 0
icmpv6_percent = 0

icmpv6_echo_requests = 0
icmpv6_echo_requests_percent = 0

icmpv6_echo_replies = 0
icmpv6_echo_replies_percent = 0

# Nível de Transporte - Estatísticas de tráfego TCP (CONTINUAR)

# Limiar para geração de alertas
alert_threshold = 100


def process_packet(packet):
    global total_packets, total_bytes, packets_per_device, min_packet_size, max_packet_size, total_packet_size, packet_count, arp_requests, arp_replies, ipv4_packets, ipv4_percent, icmp_packets, icmp_percent, icmp_echo_requests, icmp_echo_requests_percent, icmp_echo_replies, icmp_echo_replies_percent, ipv6_packets, ipv6_percent, icmpv6_packets, icmpv6_percent, icmpv6_echo_requests, icmpv6_echo_requests_percent, icmpv6_echo_replies, icmpv6_echo_replies_percent

    # Incrementa as estatísticas
    total_packets += 1
    total_bytes += len(packet)
    if packet.src in devices:
        packets_per_device[packet.src] += 1

    # Atualiza as estatísticas de tamanho dos pacotes
    packet_size = len(packet)
    min_packet_size = min(min_packet_size, packet_size)
    max_packet_size = max(max_packet_size, packet_size)
    total_packet_size += packet_size
    packet_count += 1

    # Atualiza as estatísticas de tráfego ARP
    if ARP in packet:
        if packet[ARP].op == 1:  # ARP request
            arp_requests += 1
        elif packet[ARP].op == 2:  # ARP reply
            arp_replies += 1

    # Atualiza as estatísticas de tráfego IPv4
    if IP in packet:
        ipv4_packets += 1

        # Atualiza as estatísticas de tráfego ICMP
        if ICMP in packet:
            icmp_packets += 1

            if packet[ICMP].type == 8:
                icmp_echo_requests += 1
            elif packet[ICMP].type == 0:
                icmp_echo_replies += 1

    # Atualiza as estatísticas de tráfego IPv6
    if IPv6 in packet:
        ipv6_packets += 1

        # Atualiza as estatísticas de tráfego ICMPv6
        if ICMPv6EchoRequest in packet:
            icmpv6_echo_requests += 1

        if ICMPv6EchoReply in packet:
            icmpv6_echo_replies += 1

    # Atualiza as porcentagens
    ipv4_percent = (ipv4_packets / total_packets) * 100 if total_packets > 0 else 0
    icmp_percent = (icmp_packets / total_packets) * 100 if total_packets > 0 else 0
    icmp_echo_requests_percent = (
        (icmp_echo_requests / total_packets) * 100 if total_packets > 0 else 0
    )
    icmp_echo_replies_percent = (
        (icmp_echo_replies / total_packets) * 100 if total_packets > 0 else 0
    )
    ipv6_percent = (ipv6_packets / total_packets) * 100 if total_packets > 0 else 0
    icmpv6_percent = (icmpv6_packets / total_packets) * 100 if total_packets > 0 else 0
    icmpv6_echo_requests_percent = (
        (icmpv6_echo_requests / total_packets) * 100 if total_packets > 0 else 0
    )
    icmpv6_echo_replies_percent = (
        (icmpv6_echo_replies / total_packets) * 100 if total_packets > 0 else 0
    )

    # Verifica se deve gerar um alerta
    if total_packets % alert_threshold == 0:
        print_alert()

    # Atualiza a saída no console
    print_stats()

    # Descarta o pacote
    return


def print_stats():
    global total_packets, total_bytes, packets_per_device, min_packet_size, max_packet_size, total_packet_size, packet_count, arp_requests, arp_replies, ipv4_packets, ipv4_percent, icmp_packets, icmp_percent, icmp_echo_requests, icmp_echo_requests_percent, icmp_echo_replies, icmp_echo_replies_percent, ipv6_packets, ipv6_percent, icmpv6_packets, icmpv6_percent, icmpv6_echo_requests, icmpv6_echo_requests_percent, icmpv6_echo_replies, icmpv6_echo_replies_percent

    # Limpa a tela
    print("\033c", end="")

    # Calcula a média do tamanho dos pacotes
    avg_packet_size = total_packet_size / packet_count if packet_count > 0 else 0

    # Calcula as estatísticas de ARP
    total_arp = arp_requests + arp_replies
    arp_request_percent = (arp_requests / total_arp) * 100 if total_arp > 0 else 0
    arp_reply_percent = (arp_replies / total_arp) * 100 if total_arp > 0 else 0

    # Imprime as estatísticas gerais de tráfego
    print("Estatísticas gerais:")
    print(f" - Pacotes: {total_packets}")
    print(f" - Bytes: {total_bytes}")
    print()

    # Imprime as estatísticas de tamanho dos pacotes
    print("Estatísticas de tamanho de pacotes:")
    print(f" - Tamanho mínimo: {min_packet_size} bytes")
    print(f" - Tamanho máximo: {max_packet_size} bytes")
    print(f" - Tamanho médio: {avg_packet_size:.1f} bytes")
    print()

    # Imprime as estatísticas de tráfego ARP
    print("Estatísticas de ARP:")
    print(f" - ARP requests: {arp_requests} ({arp_request_percent:.1f}%)")
    print(f" - ARP replies: {arp_replies} ({arp_reply_percent:.1f}%)")
    print()

    # Imprime as estatísticas de tráfego IPv4
    print("Estatísticas de IPv4:")
    print(f" - Pacotes IPv4: {ipv4_packets} ({ipv4_percent:.1f}%)")
    print()

    # Imprime as estatísticas de tráfego ICMP
    print("Estatísticas de ICMP:")
    print(f" - Pacotes ICMP: {icmp_packets} ({icmp_percent:.1f}%)")
    print(f" - ICMP echo requests: {icmp_echo_requests} ({icmp_echo_requests_percent:.1f}%)")
    print(f" - ICMP echo replies: {icmp_echo_replies} ({icmp_echo_replies_percent:.1f}%)")
    print()

    # Imprime as estatísticas de tráfego IPv6
    print("Estatísticas de IPv6:")
    print(f" - Pacotes IPv6: {ipv6_packets} ({ipv6_percent:.1f}%)")
    print()

    # Imprime as estatísticas de tráfego ICMPv6
    print("Estatísticas de ICMPv6:")
    print(f" - Pacotes ICMPv6: {icmpv6_packets} ({icmpv6_percent:.1f}%)")
    print(f" - ICMPv6 echo requests: {icmpv6_echo_requests} ({icmpv6_echo_requests_percent:.1f}%)")
    print(f" - ICMPv6 echo replies: {icmpv6_echo_replies} ({icmpv6_echo_replies_percent:.1f}%)")
    print()

    # Imprime as estatísticas de tráfego por dispositivo
    print("Estatísticas por dispositivo:")
    for mac, name in devices.items():
        print(f" - {name}: {packets_per_device[mac]} pacotes")
    print()

    print("\nPressione Ctrl+C para sair.")


def print_alert():
    print("ALERTA: alto tráfego de rede!")


# Captura os pacotes em tempo real
sniff(prn=process_packet)
