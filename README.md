# Monitor-de-Rede
Este é um projeto desenvolvido para a disciplina de Laboratório de Redes de Computadores. O projeto consiste em desenvolver um monitor de rede para geração
de estatísticas e alertas sobre o tráfego da rede. O monitor deve apresentar as informações sobre o tráfego através de uma interface textual. 

## Funcionalidades do monitor
- Geral
    - Apresentar min/max/média do tamanho dos pacotes recebidos
- Nível de Enlace
    - Quantidade e porcentagem de ARP Requests e ARP Reply
- Nível de Rede
    - Quantidade e porcentagem de pacotes IPv4
    - Quantidade e porcentagem de pacotes ICMP
    - Quantidade e porcentagem de ICMP Echo Request e ICMP Echo Reply
    - Quantidade e porcentagem de pacotes IPv6
    - Quantidade e porcentagem de pacotes ICMPv6
    - Quantidade e porcentagem de ICMPv6 Echo Request e ICMPv6 Echo Reply
- Nível de Transporte
    - Quantidade e porcentagem de pacotes UDP
    - Quantidade e porcentagem de pacotes TCP
    - Lista com as 5 portas TCP mais acessadas
    - Lista com as 5 portas UDP mais acessadas
- Nível de Aplicação
    - Quantidade e porcentagem de pacotes HTTP
    - Quantidade e porcentagem de pacotes DNS
    - Quantidade e porcentagem de pacotes DHCP
    - Quantidade e porcentagem para outro protocolo de aplicação qualquer

## Requisitos
Para executar o monitor de rede, serão necessários os seguintes requisitos:

- Sistema operacional Linux
- Compilador de C instalado.

## Como usar
Para usar o monitor de rede, siga os seguintes passos:

- Faça o download do código-fonte e descompacte o arquivo.
- Abra um terminal e navegue até a pasta onde o código-fonte foi descompactado.
- Compile o código. No Linux, você pode usar o GCC digitando 
`gcc arquivo.c -o socket` no terminal.
- Execute o monitor de rede usando o comando `sudo ./socket`.
- O monitor de rede irá exibir as informações sobre o tráfego de rede na interface textual.

## Alunos
- André Luiz Rodrigues
- Fernanda Ferreira de Mello
- Matheus Pozzer Moraes 