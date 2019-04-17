import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Scanner;
 
public class Cliente {
	static Scanner s;
	static String navegador;
	static String url;
	static String porta;
	private static FileWriter arquivo;
	private static Scanner sc;
	private static Socket socket;
	static String requisicao;

	public static void main(String[] args) throws IOException {
		System.out.println("Bem vindo ao navegador: digite a url desejada e a porta.\n"
				+ "Digite todo o caminho desejado e lembre-se da / no final");
		System.out.println("navegador>");
		s = new Scanner(System.in);
		String re = s.nextLine();

		String[] parts = re.split(" ");
		if (parts.length == 2) {
			url = parts[0];
			porta = parts[1];
		} else if (parts.length == 1) {
			url = parts[0];
			porta = "80";
		} else {
			System.out.println("Você digitou errado!");
		}
		
		String novourl = url.replace("http://", "");
		String[] vetor = novourl.split("/");
		String caminho = "";
		String extArquivo = null;
		
		for (int i = vetor.length - 1; i > 0; i--) {
			caminho = vetor[i].concat("/" + caminho);
		}

		caminho = "/" + caminho;
		caminho = caminho.substring(0, caminho.length() - 1);
		extArquivo = (caminho.substring(caminho.lastIndexOf("/") + 1));
		
		if (extArquivo.length() == 0) {
			extArquivo = "index";
		}
		
		int porta = Integer.parseInt(Cliente.porta);
		socket = new Socket(vetor[0], porta);

		if (socket.isConnected()) {

			System.out.println("Conectado a " + socket.getInetAddress());

			if (novourl.substring(novourl.length() - 1).equals("/")) {
				requisicao = "GET " + caminho + "/ HTTP/1.1\r\n" + "Host: " + vetor[0] + "\r\n" + "\r\n";
				extArquivo = extArquivo + ".html";
			} else {
				requisicao = "GET " + caminho + " HTTP/1.1\r\n" + "Host: " + vetor[0] + "\r\n" + "\r\n";
			}
			OutputStream envioServ = socket.getOutputStream();
                        
			byte[] b = requisicao.getBytes();
                        System.out.println();
			envioServ.write(b);

			envioServ.flush();

			sc = new Scanner(socket.getInputStream());
			arquivo = new FileWriter(new File(extArquivo));

			ArrayList<String> content = new ArrayList<>();
                        int cabecalho = 0;
                        System.out.println( "Digite 1 para printar somente o cabecalho e 2 para printar todo o arquivo" );
			Scanner menu = new Scanner(System.in);
                        int menu2 = menu.nextInt();
                        
                            while (sc.hasNext()) {
                                String line = sc.nextLine();
                            
			    if( menu2 == 1 ){
				
                               if(cabecalho == 0)
                                {   
                                    System.out.println(line);
                                    if(line.contains("Content-Type"))
                                    cabecalho=1;
                                }
				content.add(line);
                                }else{
                                System.out.println(line);
				content.add(line);}
                        }
			int start_pos = 0;
			for (int i = 0; i < content.size(); i++) {
				if (content.get(i).contains("Content-Type")) {
					start_pos = i + 2;
					break;
				}
			}
			for (int i = start_pos; i < content.size(); i++) {
				arquivo.write(content.get(i));
				arquivo.write("\n");
			}
			arquivo.flush();
			arquivo.close();
		}
	}
}