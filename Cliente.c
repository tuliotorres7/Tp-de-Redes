#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    struct sockaddr_in saddr;
    int sockfd, bytes, csize = sizeof saddr;
    char buff[1024],path[30], host[30], *data;//todos os vetores recebem o tamanho do socket
    struct hostent *he;//converte o nome/host para o endereço IP
    int PORT;
    
    if (argc == 3) {
         PORT = 80;
    }else
        if (argc == 4){
            PORT = atoi(argv[3]);//

            }else{
            fprintf(stderr,"Voce digitou a entrada fora do padrao, Lembre: ./Cliente nomedosite.com.br/caminho/ nomeDoArqHaSerSalvo Porta(Opcional) \n");
        exit(1);
        } 

    printf("PORTA: %d\n",PORT);
    int tamNomeSite = 0, tamCaminho = 0, flag = 0;
    //Colocando \0 no final dos vetores
    memset(host,'\0',30);
    memset(path,'\0',30);
    
    printf("Nome do Site e Caminho dentro dele: "); 
    
    while(argv[1][tamNomeSite] != '\0'){
     
        if (argv[1][tamNomeSite] == '/' || flag == 1){
            flag = 1;                
            path[tamCaminho] = argv[1][tamNomeSite];                    
            tamCaminho++; 
        }else{
            host[tamNomeSite] = argv[1][tamNomeSite];
        }    
    tamNomeSite++;
    }
    
     path[tamCaminho] = '\0';
    printf("\nhost: %s", host);
    printf("    path: %s\n", path);
    printf("Requisicao HTTP:    \n");//
    char requisicao[300];
    memset(requisicao,'\0',30);
 
    if (flag == 1){
        strcpy(requisicao,"GET ");
        strcat(requisicao,path);
    }else{
        strcpy(requisicao,"GET /");
    }
        strcat(requisicao, " HTTP/1.1\r\n");
        strcat(requisicao,"Host: "); 
        strcat(requisicao,host);   
        strcat(requisicao, "\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Cliente/5.0 (X11; Linux x86_64)\r\n"
		"Accept: text/html\r\nAccept-Encoding: deflate\r\n"
        "Accept-Language: en-US,en;q=0.9,pt-BR;q=0.8,pt;q=0.7\r\n\r\n");
    printf("%s \n",requisicao);


    if ((he=gethostbyname(host)) == NULL) {  /* erro na tradução do ip */
        printf("uala");
        herror("Buscando host pelo nome");
        printf("uala");
        exit(1);
    }
 
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    
    //Preenchendo os enderecos de rede
    saddr.sin_family = AF_INET;//ipv4        
    saddr.sin_addr = *((struct in_addr *)he->h_addr);
    saddr.sin_port = htons(80);
    bzero(&(saddr.sin_zero), 8);  //o linux diz que nao é necessario zerar este campo
    

    if ((sockfd = socket(AF_INET,SOCK_STREAM, 0)) == -1){//sock_strem implementação do protocolo
        perror("Erro ao criar o socket");
        exit(1);
    }
    
    printf("conectando\n");
    if (connect(sockfd,(struct sockaddr *) &saddr, csize) == -1){
        perror("connect");
        exit(1);        
    }
    
    memset (buff,'\0',1024);        
    FILE *Arquivo;

    Arquivo = fopen(argv[2],"wb");//nomeando o arquivo binario com o nome que vem nos argumentos da main, cria ou reescreve o arquivo caso ja exista
    
    send(sockfd,requisicao,strlen(requisicao),0);//enviando requisição. socket buffer , tamanho ,flag
    bytes = 1;
    char statusRequisicao[4];
    int flagCabecalho =1;
    
     if ((bytes = recv(sockfd,buff, 1023, 0)) == -1){//recebendo dados do socket
            perror("Erro ao receber mensagem");            
        } 
        printf("%s\n",buff);//cabeçalho de resposta http -(Alem do cabeçalho, Doctype, head)
        printf("bytes : %d\n",bytes);
int test=0;
    while(bytes > 0){
         printf("%d--------------------------",test++);
        if (flagCabecalho){
            int i;

      
            for (i = 0; i < 3; i++)		//buscar o status           
            statusRequisicao[i] = buff[i+9];            //200 ok 404 falha de segmentação na resposta da requisição
            
            statusRequisicao[i++] = '\0';
            if (atoi(statusRequisicao) == 200){                

                data = strstr( buff, "\r\n\r\n" );
                if ( data != NULL )
                    data += 4;

                flagCabecalho = 0;
                }else{
                    fwrite(buff,sizeof(char),bytes, Arquivo);//escreve apenas o cabeçalho
                    
                    return 1;                    
                }
        }else{
            printf("%s\n\n",buff);
            fwrite(buff,sizeof(char),bytes, Arquivo);
        }

        memset(buff,'\0',1024);
        if ((bytes = recv(sockfd,buff, 1023, 0)) == -1){
                perror("Erro ao receber mensagem");            
        } 
    }
    fclose(Arquivo);
    close(sockfd);
   return 1;
}

